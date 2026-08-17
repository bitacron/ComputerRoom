package com.example.room.security.security;

import io.jsonwebtoken.CompressionCodecs;
import io.jsonwebtoken.Jwts;
import io.jsonwebtoken.SignatureAlgorithm;
import org.springframework.beans.factory.annotation.Value;
import org.springframework.data.redis.core.RedisTemplate;
import org.springframework.stereotype.Component;
import org.springframework.util.StringUtils;

import java.util.Date;
import java.util.concurrent.TimeUnit;

/**
 * JWT 签发与黑名单吊销
 */
@Component
public class TokenManager {

    private static final String BLACKLIST_PREFIX = "jwt:blacklist:";

    private final RedisTemplate<String, Object> redisTemplate;

    @Value("${jwt.secret}")
    private String tokenSignKey;

    @Value("${jwt.expiration:86400000}")
    private long tokenExpiration;

    @Value("${jwt.blacklist.enable:true}")
    private boolean blacklistEnable;

    @Value("${jwt.blacklist.cache-expire:86400000}")
    private long blacklistCacheExpire;

    public TokenManager(RedisTemplate<String, Object> redisTemplate) {
        this.redisTemplate = redisTemplate;
    }

    public String createToken(String username) {
        return Jwts.builder()
                .setSubject(username)
                .setExpiration(new Date(System.currentTimeMillis() + tokenExpiration))
                .signWith(SignatureAlgorithm.HS512, tokenSignKey)
                .compressWith(CompressionCodecs.GZIP)
                .compact();
    }

    public String getUserFromToken(String token) {
        return Jwts.parser().setSigningKey(tokenSignKey).parseClaimsJws(token).getBody().getSubject();
    }

    public boolean isTokenBlacklisted(String token) {
        if (!blacklistEnable || !StringUtils.hasText(token)) {
            return false;
        }
        Boolean exists = redisTemplate.hasKey(BLACKLIST_PREFIX + token);
        return Boolean.TRUE.equals(exists);
    }

    /**
     * 将 token 加入黑名单，TTL 取剩余有效期与配置上限的较小值
     */
    public void removeToken(String token) {
        if (!blacklistEnable || !StringUtils.hasText(token)) {
            return;
        }
        long ttl = blacklistCacheExpire;
        try {
            Date expiration = Jwts.parser().setSigningKey(tokenSignKey).parseClaimsJws(token).getBody().getExpiration();
            if (expiration != null) {
                long remain = expiration.getTime() - System.currentTimeMillis();
                if (remain <= 0) {
                    return;
                }
                ttl = Math.min(remain, blacklistCacheExpire);
            }
        } catch (Exception ignored) {
            // 解析失败仍按配置 TTL 拉黑，防止异常 token 被复用
        }
        redisTemplate.opsForValue().set(BLACKLIST_PREFIX + token, "1", ttl, TimeUnit.MILLISECONDS);
    }
}
