package com.example.room.security.security;

import com.example.room.util.ResponseUtil;
import com.example.room.util.Result;
import org.springframework.data.redis.core.RedisTemplate;
import org.springframework.security.core.Authentication;
import org.springframework.security.web.authentication.logout.LogoutHandler;
import org.springframework.util.StringUtils;

import javax.servlet.http.HttpServletRequest;
import javax.servlet.http.HttpServletResponse;

/**
 * 登出：吊销 JWT（黑名单）并清除 Redis 权限缓存
 */
public class TokenLogoutHandler implements LogoutHandler {

    private final TokenManager tokenManager;
    private final RedisTemplate<String, Object> redisTemplate;

    public TokenLogoutHandler(TokenManager tokenManager, RedisTemplate<String, Object> redisTemplate) {
        this.tokenManager = tokenManager;
        this.redisTemplate = redisTemplate;
    }

    @Override
    public void logout(HttpServletRequest request, HttpServletResponse response, Authentication authentication) {
        String token = TokenUtils.resolveToken(request);
        String username = null;

        if (authentication != null && StringUtils.hasText(authentication.getName())) {
            username = authentication.getName();
        } else if (StringUtils.hasText(token)) {
            try {
                username = tokenManager.getUserFromToken(token);
            } catch (Exception ignored) {
                // token 无效时仍继续返回成功，避免登出接口 500
            }
        }

        if (StringUtils.hasText(token)) {
            tokenManager.removeToken(token);
        }
        if (StringUtils.hasText(username)) {
            redisTemplate.delete(username);
        }
        ResponseUtil.out(response, Result.ok());
    }
}
