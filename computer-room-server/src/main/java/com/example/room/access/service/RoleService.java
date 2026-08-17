package com.example.room.access.service;

import com.baomidou.mybatisplus.extension.service.IService;
import com.example.room.access.entity.Role;
import com.example.room.access.vo.RoleAssignVo;

import java.util.List;

public interface RoleService extends IService<Role> {

    RoleAssignVo findRoleByUserId(Long userId);

    void saveUserRoleRelationShip(Long userId, Long[] roleId);

    List<Role> selectRoleByUserId(Long id);
}
