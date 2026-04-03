package com.example.room.access.mapper;

import com.example.room.access.entity.Permission;
import com.baomidou.mybatisplus.core.mapper.BaseMapper;

import java.util.List;

public interface PermissionMapper extends BaseMapper<Permission> {

    List<String> selectPermissionValueByUserId(Long id);

    List<String> selectAllPermissionValue();

    List<Permission> selectPermissionByUserId(Long userId);
}
