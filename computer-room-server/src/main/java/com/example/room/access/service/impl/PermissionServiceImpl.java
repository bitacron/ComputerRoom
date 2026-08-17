package com.example.room.access.service.impl;

import com.alibaba.fastjson.JSONObject;
import com.example.room.access.entity.Permission;
import com.example.room.access.entity.RolePermission;
import com.example.room.access.entity.User;
import com.example.room.access.helper.MenuHelper;
import com.example.room.access.helper.PermissionHelper;
import com.example.room.access.mapper.PermissionMapper;
import com.example.room.access.service.AccessPermissionCacheService;
import com.example.room.access.service.PermissionService;
import com.example.room.access.service.RolePermissionService;
import com.example.room.access.service.UserService;
import com.baomidou.mybatisplus.core.conditions.query.QueryWrapper;
import com.baomidou.mybatisplus.extension.service.impl.ServiceImpl;
import org.springframework.beans.factory.annotation.Autowired;
import org.springframework.stereotype.Service;
import org.springframework.transaction.annotation.Transactional;

import java.util.ArrayList;
import java.util.Date;
import java.util.List;
import java.util.Objects;

@Service
public class PermissionServiceImpl extends ServiceImpl<PermissionMapper, Permission> implements PermissionService {

    @Autowired
    private RolePermissionService rolePermissionService;

    @Autowired
    private UserService userService;

    @Autowired
    private AccessPermissionCacheService accessPermissionCacheService;

    @Override
    public List<Permission> selectAllMenu(Long roleId) {
        List<Permission> allPermissionList = baseMapper.selectList(orderBySort());

        List<RolePermission> rolePermissionList = rolePermissionService.list(
                new QueryWrapper<RolePermission>().eq("role_id", roleId));

        for (Permission permission : allPermissionList) {
            for (RolePermission rolePermission : rolePermissionList) {
                if (rolePermission.getPermissionId().equals(permission.getId())) {
                    permission.setSelect(true);
                }
            }
        }

        return buildPermission(allPermissionList);
    }

    @Override
    public List<String> selectPermissionValueByUserId(Long id) {
        if (this.isSysAdmin(id)) {
            return baseMapper.selectAllPermissionValue();
        }
        return baseMapper.selectPermissionValueByUserId(id);
    }

    @Override
    public List<JSONObject> selectPermissionByUserId(Long userId) {
        List<Permission> selectPermissionList;
        if (this.isSysAdmin(userId)) {
            selectPermissionList = baseMapper.selectList(orderBySort());
        } else {
            selectPermissionList = baseMapper.selectPermissionByUserId(userId);
        }

        List<Permission> permissionList = PermissionHelper.build(selectPermissionList);
        return MenuHelper.build(permissionList);
    }

    private boolean isSysAdmin(Long userId) {
        User user = userService.getById(userId);
        return user != null && "admin".equals(user.getUsername());
    }

    private QueryWrapper<Permission> orderBySort() {
        return new QueryWrapper<Permission>().orderByAsc("sort").orderByAsc("id");
    }

    @Override
    public List<Permission> queryAllMenu() {
        List<Permission> permissionList = baseMapper.selectList(orderBySort());
        return buildPermission(permissionList);
    }

    public static List<Permission> buildPermission(List<Permission> permissionList) {
        List<Permission> finalNode = new ArrayList<>();
        for (Permission permissionNode : permissionList) {
            if (Objects.equals(permissionNode.getPid(), 0L)) {
                permissionNode.setLevel(1);
                finalNode.add(selectChildren(permissionNode, permissionList));
            }
        }
        return finalNode;
    }

    private static Permission selectChildren(Permission permissionNode, List<Permission> permissionList) {
        permissionNode.setChildren(new ArrayList<>());

        for (Permission it : permissionList) {
            if (permissionNode.getId().equals(it.getPid())) {
                int level = permissionNode.getLevel() + 1;
                it.setLevel(level);
                if (permissionNode.getChildren() == null) {
                    permissionNode.setChildren(new ArrayList<>());
                }
                permissionNode.getChildren().add(selectChildren(it, permissionList));
            }
        }
        return permissionNode;
    }

    @Override
    public void removeChildById(Long id) {
        List<Long> idList = new ArrayList<>();
        this.selectPermissionChildById(id, idList);
        idList.add(id);
        baseMapper.deleteBatchIds(idList);
    }

    private void selectPermissionChildById(Long id, List<Long> idList) {
        QueryWrapper<Permission> wrapper = new QueryWrapper<>();
        wrapper.eq("pid", id);
        wrapper.select("id");
        List<Permission> childIdList = baseMapper.selectList(wrapper);
        childIdList.forEach(item -> {
            idList.add(item.getId());
            this.selectPermissionChildById(item.getId(), idList);
        });
    }

    @Override
    @Transactional(rollbackFor = Exception.class)
    public void saveRolePermissionRelationShip(Long roleId, Long[] permissionIds) {
        rolePermissionService.remove(new QueryWrapper<RolePermission>().eq("role_id", roleId));

        List<RolePermission> rolePermissionList = new ArrayList<>();
        for (Long perId : permissionIds) {
            if (perId == null) {
                continue;
            }
            RolePermission rolePermission = new RolePermission();
            rolePermission.setRoleId(roleId);
            rolePermission.setPermissionId(perId);
            Date now = new Date();
            rolePermission.setGmtCreate(now);
            rolePermission.setGmtModified(now);
            rolePermissionList.add(rolePermission);
        }
        if (!rolePermissionList.isEmpty()) {
            rolePermissionService.saveBatch(rolePermissionList);
        }
        accessPermissionCacheService.refreshByRoleId(roleId);
    }
}
