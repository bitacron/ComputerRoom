package com.example.room.access.controller;

import com.example.room.access.entity.Permission;
import com.example.room.access.service.PermissionService;
import com.example.room.access.vo.ChildrenVo;
import com.example.room.util.Result;
import io.swagger.annotations.ApiOperation;
import org.springframework.beans.factory.annotation.Autowired;
import org.springframework.security.access.prepost.PreAuthorize;
import org.springframework.web.bind.annotation.*;

import java.util.List;

@RestController
@RequestMapping("/admin/access/permission")
@CrossOrigin
public class PermissionController {

    @Autowired
    private PermissionService permissionService;

    @ApiOperation(value = "查询所有菜单")
    @PreAuthorize("hasAuthority('permission.list')")
    @GetMapping
    public Result<ChildrenVo<Permission>> indexAllPermission() {
        List<Permission> list = permissionService.queryAllMenu();
        return Result.ok(new ChildrenVo<>(list));
    }

    @ApiOperation(value = "递归删除菜单")
    @PreAuthorize("hasAuthority('permission.remove')")
    @DeleteMapping("remove/{id}")
    public Result<Void> remove(@PathVariable Long id) {
        permissionService.removeChildById(id);
        return Result.ok();
    }

    @ApiOperation(value = "给角色分配权限")
    @PreAuthorize("hasAuthority('role.access')")
    @PostMapping("/doAssign")
    public Result<Void> doAssign(@RequestParam Long roleId, @RequestParam Long[] permissionId) {
        permissionService.saveRolePermissionRelationShip(roleId, permissionId);
        return Result.ok();
    }

    @ApiOperation(value = "根据角色获取菜单")
    @PreAuthorize("hasAuthority('role.access')")
    @GetMapping("toAssign/{roleId}")
    public Result<ChildrenVo<Permission>> toAssign(@PathVariable Long roleId) {
        List<Permission> list = permissionService.selectAllMenu(roleId);
        return Result.ok(new ChildrenVo<>(list));
    }

    @ApiOperation(value = "新增菜单")
    @PreAuthorize("hasAuthority('permission.add')")
    @PostMapping("save")
    public Result<Void> save(@RequestBody Permission permission) {
        permissionService.save(permission);
        return Result.ok();
    }

    @ApiOperation(value = "修改菜单")
    @PreAuthorize("hasAuthority('permission.update')")
    @PutMapping("update")
    public Result<Void> updateById(@RequestBody Permission permission) {
        permissionService.updateById(permission);
        return Result.ok();
    }
}
