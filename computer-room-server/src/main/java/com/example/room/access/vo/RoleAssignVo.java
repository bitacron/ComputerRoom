package com.example.room.access.vo;

import com.example.room.access.entity.Role;
import lombok.Data;

import java.util.List;

/**
 * 用户角色分配页数据
 */
@Data
public class RoleAssignVo {

    private List<Role> assignRoles;

    private List<Role> allRolesList;
}
