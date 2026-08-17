package com.example.room.security.filter;

import com.example.room.security.security.TokenManager;
import com.example.room.security.security.TokenUtils;
import org.springframework.data.redis.core.RedisTemplate;
import org.springframework.security.authentication.AuthenticationManager;
import org.springframework.security.authentication.UsernamePasswordAuthenticationToken;
import org.springframework.security.core.GrantedAuthority;
import org.springframework.security.core.authority.SimpleGrantedAuthority;
import org.springframework.security.core.context.SecurityContextHolder;
import org.springframework.security.web.authentication.www.BasicAuthenticationFilter;
import org.springframework.util.StringUtils;

import javax.servlet.FilterChain;
import javax.servlet.ServletException;
import javax.servlet.http.HttpServletRequest;
import javax.servlet.http.HttpServletResponse;
import java.io.IOException;
import java.util.ArrayList;
import java.util.Collection;
import java.util.Collections;
import java.util.List;

/**
 * <p>
 * 访问授权过滤器
 * </p>
 *
 * @author zhmy
 * @since 2019-11-08
 */
public class TokenAuthenticationFilter extends BasicAuthenticationFilter {
    private final TokenManager tokenManager;
    private final RedisTemplate<String, Object> redisTemplate;

    public TokenAuthenticationFilter(AuthenticationManager authManager,
                                     TokenManager tokenManager,
                                     RedisTemplate<String, Object> redisTemplate) {
        super(authManager);
        this.tokenManager = tokenManager;
        this.redisTemplate = redisTemplate;
    }

    @Override
    protected void doFilterInternal(HttpServletRequest req, HttpServletResponse res, FilterChain chain)
            throws IOException, ServletException {
        logger.info("================="+req.getRequestURI());
        // 对所有走 Spring Security 的请求解析 token 并写入 SecurityContext。
        // 原先用 URI 是否包含 "admin" 跳过 /service/** 等路径，导致这些接口从未登录态，
        // anyRequest().authenticated() 会走 UnauthorizedEntryPoint，返回 Result.fail()（业务 code=500），看起来像 500 且无异常日志。

        UsernamePasswordAuthenticationToken authentication = null;
        try {
            authentication = getAuthentication(req);
        } catch (Exception e) {
            logger.error("Token 解析失败", e);
        }

        if (authentication != null) {
            SecurityContextHolder.getContext().setAuthentication(authentication);
        }
        chain.doFilter(req, res);
    }

    private UsernamePasswordAuthenticationToken getAuthentication(HttpServletRequest request) {
        String token = TokenUtils.resolveToken(request);
        if (!StringUtils.hasText(token)) {
            return null;
        }
        if (tokenManager.isTokenBlacklisted(token)) {
            return null;
        }

        String userName = tokenManager.getUserFromToken(token);
        if (!StringUtils.hasText(userName)) {
            return null;
        }

        Object cached = redisTemplate.opsForValue().get(userName);
        List<String> permissionValueList = Collections.emptyList();
        if (cached instanceof List) {
            @SuppressWarnings("unchecked")
            List<String> list = (List<String>) cached;
            permissionValueList = list;
        }
        Collection<GrantedAuthority> authorities = new ArrayList<>();
        for (String permissionValue : permissionValueList) {
            if (StringUtils.isEmpty(permissionValue)) {
                continue;
            }
            authorities.add(new SimpleGrantedAuthority(permissionValue));
        }
        return new UsernamePasswordAuthenticationToken(userName, token, authorities);
    }
}