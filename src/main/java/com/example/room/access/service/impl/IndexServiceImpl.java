package com.example.room.access.service.impl;

import com.alibaba.fastjson.JSONObject;
import com.example.room.access.entity.Role;
import com.example.room.access.entity.User;
import com.example.room.access.service.IndexService;
import com.example.room.access.service.PermissionService;
import com.example.room.access.service.RoleService;
import com.example.room.access.service.UserService;
import com.example.room.access.vo.UserInfoVo;
import org.springframework.data.redis.core.RedisTemplate;
import org.springframework.stereotype.Service;

import javax.annotation.Resource;
import java.util.Collections;
import java.util.List;
import java.util.stream.Collectors;

@Service
public class IndexServiceImpl implements IndexService {

    @Resource
    private UserService userService;

    @Resource
    private RoleService roleService;

    @Resource
    private PermissionService permissionService;

    @Resource
    private RedisTemplate<String, Object> redisTemplate;

    @Override
    public UserInfoVo getUserInfo(String username) {
        UserInfoVo userInfo = new UserInfoVo();
        User user = userService.selectByUsername(username);
        if (user == null) {
            return userInfo;
        }

        List<Role> roleList = roleService.selectRoleByUserId(user.getId());
        List<String> roleNameList = roleList.stream().map(Role::getRoleName).collect(Collectors.toList());
        if (roleNameList.isEmpty()) {
            roleNameList.add("");
        }

        List<String> permissionValueList = permissionService.selectPermissionValueByUserId(user.getId());
        redisTemplate.opsForValue().set(username, permissionValueList);

        userInfo.setId(user.getId());
        userInfo.setName(user.getNickName());
        userInfo.setNickName(user.getNickName());
        userInfo.setCode(user.getUsername());
        userInfo.setAvatar(user.getAvatar());
        userInfo.setPhone(user.getPhone());
        userInfo.setEmail(user.getEmail());
        userInfo.setRoles(roleNameList);
        userInfo.setPermissionValueList(permissionValueList);
        return userInfo;
    }

    @Override
    public List<JSONObject> getMenu(String username) {
        User user = userService.selectByUsername(username);
        if (user == null) {
            return Collections.emptyList();
        }
        return permissionService.selectPermissionByUserId(user.getId());
    }
}
