package com.example.room.access.controller;

import com.baomidou.mybatisplus.core.conditions.query.QueryWrapper;
import com.baomidou.mybatisplus.extension.plugins.pagination.Page;
import com.example.room.access.entity.Role;
import com.example.room.access.service.RoleService;
import com.example.room.access.vo.ItemVo;
import com.example.room.access.vo.PageVo;
import com.example.room.util.Result;
import io.swagger.annotations.ApiOperation;
import io.swagger.annotations.ApiParam;
import org.springframework.beans.factory.annotation.Autowired;
import org.springframework.security.access.prepost.PreAuthorize;
import org.springframework.util.StringUtils;
import org.springframework.web.bind.annotation.*;

import java.util.List;

@RestController
@RequestMapping("/admin/access/role")
@CrossOrigin
public class RoleController {

    @Autowired
    private RoleService roleService;

    @ApiOperation(value = "获取角色分页列表")
    @PreAuthorize("hasAuthority('role.list')")
    @GetMapping("{page}/{limit}")
    public Result<PageVo<Role>> index(
            @ApiParam(name = "page", value = "当前页码", required = true) @PathVariable Long page,
            @ApiParam(name = "limit", value = "每页记录数", required = true) @PathVariable Long limit,
            Role role) {
        Page<Role> pageParam = new Page<>(page, limit);
        QueryWrapper<Role> wrapper = new QueryWrapper<>();
        if (!StringUtils.isEmpty(role.getRoleName())) {
            wrapper.like("role_name", role.getRoleName());
        }
        roleService.page(pageParam, wrapper);
        return Result.ok(new PageVo<>(pageParam.getRecords(), pageParam.getTotal()));
    }

    @ApiOperation(value = "获取角色")
    @PreAuthorize("hasAuthority('role.list')")
    @GetMapping("get/{id}")
    public Result<ItemVo<Role>> get(@PathVariable Long id) {
        return Result.ok(new ItemVo<>(roleService.getById(id)));
    }

    @ApiOperation(value = "新增角色")
    @PreAuthorize("hasAuthority('role.add')")
    @PostMapping("save")
    public Result<Void> save(@RequestBody Role role) {
        roleService.save(role);
        return Result.ok();
    }

    @ApiOperation(value = "修改角色")
    @PreAuthorize("hasAuthority('role.update')")
    @PutMapping("update")
    public Result<Void> updateById(@RequestBody Role role) {
        roleService.updateById(role);
        return Result.ok();
    }

    @ApiOperation(value = "删除角色")
    @PreAuthorize("hasAuthority('role.remove')")
    @DeleteMapping("remove/{id}")
    public Result<Void> remove(@PathVariable Long id) {
        roleService.removeById(id);
        return Result.ok();
    }

    @ApiOperation(value = "根据 id 列表删除角色")
    @PreAuthorize("hasAuthority('role.remove')")
    @DeleteMapping("batchRemove")
    public Result<Void> batchRemove(@RequestBody List<Long> idList) {
        roleService.removeByIds(idList);
        return Result.ok();
    }
}
