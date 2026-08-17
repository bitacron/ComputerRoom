package com.example.room.access.service.impl;

import com.baomidou.mybatisplus.core.conditions.query.QueryWrapper;
import com.baomidou.mybatisplus.extension.service.impl.ServiceImpl;
import com.example.room.access.entity.Role;
import com.example.room.access.entity.RolePermission;
import com.example.room.access.entity.UserRole;
import com.example.room.access.mapper.RoleMapper;
import com.example.room.access.service.AccessPermissionCacheService;
import com.example.room.access.service.RolePermissionService;
import com.example.room.access.service.RoleService;
import com.example.room.access.service.UserRoleService;
import com.example.room.access.vo.RoleAssignVo;
import org.springframework.beans.factory.annotation.Autowired;
import org.springframework.stereotype.Service;
import org.springframework.transaction.annotation.Transactional;

import java.io.Serializable;
import java.util.ArrayList;
import java.util.Collection;
import java.util.Date;
import java.util.List;
import java.util.stream.Collectors;

@Service
public class RoleServiceImpl extends ServiceImpl<RoleMapper, Role> implements RoleService {

    @Autowired
    private UserRoleService userRoleService;

    @Autowired
    private RolePermissionService rolePermissionService;

    @Autowired
    private AccessPermissionCacheService accessPermissionCacheService;

    @Override
    public RoleAssignVo findRoleByUserId(Long userId) {
        List<Role> allRolesList = baseMapper.selectList(null);
        List<UserRole> existUserRoleList = userRoleService.list(
                new QueryWrapper<UserRole>().eq("user_id", userId).select("role_id"));
        List<Long> existRoleList = existUserRoleList.stream().map(UserRole::getRoleId).collect(Collectors.toList());

        List<Role> assignRoles = new ArrayList<>();
        for (Role role : allRolesList) {
            if (existRoleList.contains(role.getId())) {
                assignRoles.add(role);
            }
        }

        RoleAssignVo roleAssignVo = new RoleAssignVo();
        roleAssignVo.setAssignRoles(assignRoles);
        roleAssignVo.setAllRolesList(allRolesList);
        return roleAssignVo;
    }

    @Override
    @Transactional(rollbackFor = Exception.class)
    public void saveUserRoleRelationShip(Long userId, Long[] roleIds) {
        userRoleService.remove(new QueryWrapper<UserRole>().eq("user_id", userId));

        List<UserRole> userRoleList = new ArrayList<>();
        for (Long roleId : roleIds) {
            if (roleId == null) {
                continue;
            }
            UserRole userRole = new UserRole();
            userRole.setUserId(userId);
            userRole.setRoleId(roleId);
            Date now = new Date();
            userRole.setGmtCreate(now);
            userRole.setGmtModified(now);
            userRoleList.add(userRole);
        }
        if (!userRoleList.isEmpty()) {
            userRoleService.saveBatch(userRoleList);
        }
        accessPermissionCacheService.refreshByUserId(userId);
    }

    @Override
    public List<Role> selectRoleByUserId(Long id) {
        List<UserRole> userRoleList = userRoleService.list(
                new QueryWrapper<UserRole>().eq("user_id", id).select("role_id"));
        List<Long> roleIdList = userRoleList.stream().map(UserRole::getRoleId).collect(Collectors.toList());
        List<Role> roleList = new ArrayList<>();
        if (!roleIdList.isEmpty()) {
            roleList = baseMapper.selectBatchIds(roleIdList);
        }
        return roleList;
    }

    @Override
    @Transactional(rollbackFor = Exception.class)
    public boolean removeById(Serializable id) {
        List<UserRole> userRoleList = userRoleService.list(
                new QueryWrapper<UserRole>().eq("role_id", id).select("user_id"));
        rolePermissionService.remove(new QueryWrapper<RolePermission>().eq("role_id", id));
        userRoleService.remove(new QueryWrapper<UserRole>().eq("role_id", id));
        boolean removed = super.removeById(id);
        if (removed) {
            for (UserRole userRole : userRoleList) {
                accessPermissionCacheService.refreshByUserId(userRole.getUserId());
            }
        }
        return removed;
    }

    @Override
    @Transactional(rollbackFor = Exception.class)
    public boolean removeByIds(Collection<? extends Serializable> idList) {
        boolean removed = true;
        for (Serializable id : idList) {
            removed = removeById(id) && removed;
        }
        return removed;
    }
}
