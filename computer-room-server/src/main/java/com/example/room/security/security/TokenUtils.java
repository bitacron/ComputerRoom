package com.example.room.security.security;

import org.springframework.util.StringUtils;

import javax.servlet.http.HttpServletRequest;

/**
 * 从请求中解析 JWT
 */
public final class TokenUtils {

    private TokenUtils() {
    }

    public static String resolveBearerToken(HttpServletRequest request) {
        String auth = request.getHeader("Authorization");
        if (StringUtils.hasText(auth) && auth.regionMatches(true, 0, "Bearer ", 0, 7)) {
            return auth.substring(7).trim();
        }
        return null;
    }

    /**
     * 优先 Bearer，其次 token 头，再次 query ?token=
     */
    public static String resolveToken(HttpServletRequest request) {
        String bearer = resolveBearerToken(request);
        if (StringUtils.hasText(bearer)) {
            return bearer;
        }
        String token = request.getHeader("token");
        if (StringUtils.hasText(token)) {
            return token.trim();
        }
        token = request.getParameter("token");
        if (StringUtils.hasText(token)) {
            return token.trim();
        }
        return null;
    }
}
