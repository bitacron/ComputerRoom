package com.example.room.access.controller;

import com.baomidou.mybatisplus.core.conditions.query.QueryWrapper;
import com.baomidou.mybatisplus.core.metadata.IPage;
import com.baomidou.mybatisplus.extension.plugins.pagination.Page;
import com.example.room.access.entity.Permission;
import com.example.room.access.entity.Role;
import com.example.room.access.entity.User;
import com.example.room.access.service.RoleService;
import com.example.room.access.service.UserService;
import com.example.room.access.vo.ItemVo;
import com.example.room.access.vo.PageVo;
import com.example.room.access.vo.RoleAssignVo;
import com.example.room.util.Result;
import io.swagger.annotations.ApiOperation;
import io.swagger.annotations.ApiParam;
import org.springframework.beans.factory.annotation.Autowired;
import org.springframework.security.access.prepost.PreAuthorize;
import org.springframework.security.crypto.password.PasswordEncoder;
import org.springframework.util.StringUtils;
import org.springframework.web.bind.annotation.*;

import java.util.Date;
import java.util.List;

@RestController
@RequestMapping("/admin/access/user")
@CrossOrigin
public class UserController {

    @Autowired
    private UserService userService;

    @Autowired
    private RoleService roleService;

    @Autowired
    private PasswordEncoder passwordEncoder;

    @ApiOperation(value = "获取管理用户分页列表")
    @PreAuthorize("hasAuthority('user.list')")
    @GetMapping("{page}/{limit}")
    public Result<PageVo<User>> index(
            @PathVariable Long page,
            @PathVariable Long limit,
            User userQueryVo) {
        Page<User> pageParam = new Page<>(page, limit);
        QueryWrapper<User> wrapper = new QueryWrapper<>();
        if (!StringUtils.isEmpty(userQueryVo.getUsername())) {
            wrapper.like("username", userQueryVo.getUsername());
        }
        IPage<User> pageModel = userService.page(pageParam, wrapper);
        return Result.ok(new PageVo<>(pageModel.getRecords(), pageModel.getTotal()));
    }

    @ApiOperation(value = "新增管理用户")
    @PreAuthorize("hasAuthority('user.add')")
    @PostMapping("save")
    public Result<Void> save(@RequestBody User user) {
        user.setPassword(passwordEncoder.encode(user.getPassword()));
        user.setGmtCreate(new Date());
        user.setGmtModified(new Date());
        userService.save(user);
        return Result.ok();
    }

    @ApiOperation(value = "获取用户")
    @PreAuthorize("hasAuthority('user.list')")
    @GetMapping("get/{id}")
    public Result<ItemVo<User>> get(@PathVariable Long id) {
        return Result.ok(new ItemVo<>(userService.getById(id)));
    }

    @ApiOperation(value = "修改管理用户")
    @PreAuthorize("hasAuthority('user.update')")
    @PutMapping("update")
    public Result<Void> updateById(@RequestBody User user) {
        userService.updateById(user);
        return Result.ok();
    }

    @ApiOperation(value = "删除管理用户")
    @PreAuthorize("hasAuthority('user.remove')")
    @DeleteMapping("remove/{id}")
    public Result<Void> remove(@PathVariable Long id) {
        userService.removeById(id);
        return Result.ok();
    }

    @ApiOperation(value = "根据id列表删除管理用户")
    @PreAuthorize("hasAuthority('user.remove')")
    @DeleteMapping("batchRemove")
    public Result<Void> batchRemove(@RequestBody List<Long> idList) {
        userService.removeByIds(idList);
        return Result.ok();
    }

    @ApiOperation(value = "根据用户获取角色数据")
    @PreAuthorize("hasAuthority('user.assign')")
    @GetMapping("/toAssign/{userId}")
    public Result<RoleAssignVo> toAssign(@PathVariable Long userId) {
        return Result.ok(roleService.findRoleByUserId(userId));
    }

    @ApiOperation(value = "根据用户分配角色")
    @PreAuthorize("hasAuthority('user.assign')")
    @PostMapping("/doAssign")
    public Result<Void> doAssign(@RequestParam Long userId, @RequestParam Long[] roleId) {
        roleService.saveUserRoleRelationShip(userId, roleId);
        return Result.ok();
    }
}
