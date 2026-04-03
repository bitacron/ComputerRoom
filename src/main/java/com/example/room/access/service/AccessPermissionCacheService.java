package com.example.room.access.service;

import com.baomidou.mybatisplus.core.conditions.query.QueryWrapper;
import com.example.room.access.entity.User;
import com.example.room.access.entity.UserRole;
import com.example.room.access.mapper.PermissionMapper;
import org.springframework.data.redis.core.RedisTemplate;
import org.springframework.stereotype.Service;

import javax.annotation.Resource;
import java.util.List;

/**
 * 登录后权限缓存（Redis key = username）
 */
@Service
public class AccessPermissionCacheService {

    @Resource
    private UserService userService;

    @Resource
    private PermissionMapper permissionMapper;

    @Resource
    private UserRoleService userRoleService;

    @Resource
    private RedisTemplate<String, Object> redisTemplate;

    public void refreshByUserId(Long userId) {
        if (userId == null) {
            return;
        }
        User user = userService.getById(userId);
        if (user == null) {
            return;
        }
        List<String> permissionValueList = loadPermissionValues(userId, user.getUsername());
        redisTemplate.opsForValue().set(user.getUsername(), permissionValueList);
    }

    public void refreshByRoleId(Long roleId) {
        if (roleId == null) {
            return;
        }
        List<UserRole> userRoleList = userRoleService.list(
                new QueryWrapper<UserRole>().eq("role_id", roleId).select("user_id"));
        for (UserRole userRole : userRoleList) {
            refreshByUserId(userRole.getUserId());
        }
    }

    private List<String> loadPermissionValues(Long userId, String username) {
        if ("admin".equals(username)) {
            return permissionMapper.selectAllPermissionValue();
        }
        return permissionMapper.selectPermissionValueByUserId(userId);
    }
}
