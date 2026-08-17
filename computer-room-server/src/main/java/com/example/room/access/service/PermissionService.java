package com.example.room.access.service;

import com.alibaba.fastjson.JSONObject;
import com.example.room.access.entity.Permission;
import com.baomidou.mybatisplus.extension.service.IService;

import java.util.List;

public interface PermissionService extends IService<Permission> {

    List<Permission> queryAllMenu();

    List<Permission> selectAllMenu(Long roleId);

    void saveRolePermissionRelationShip(Long roleId, Long[] permissionId);

    void removeChildById(Long id);

    List<String> selectPermissionValueByUserId(Long id);

    List<JSONObject> selectPermissionByUserId(Long id);
}
