package com.example.room.control.controller;

import com.baomidou.mybatisplus.extension.plugins.pagination.Page;
import com.example.room.control.entity.DeviceOption;
import com.example.room.control.entity.param.DeviceOptionControl;
import com.example.room.control.entity.param.DeviceOptionQuery;
import com.example.room.control.entity.vo.ControlResult;
import com.example.room.control.entity.vo.DeviceOptionVo;
import com.example.room.control.service.DeviceOptionService;
import com.example.room.util.Result;
import io.swagger.annotations.Api;
import io.swagger.annotations.ApiOperation;
import io.swagger.annotations.ApiParam;
import org.springframework.security.access.prepost.PreAuthorize;
import org.springframework.security.core.context.SecurityContextHolder;
import org.springframework.web.bind.annotation.*;

import javax.annotation.Resource;
import java.util.List;

@Api(description = "反控操作记录数据管理")
@RestController
@RequestMapping("/service/deviceOption")
public class DeviceOptionController {

    @Resource
    private DeviceOptionService deviceOptionService;

    @ApiOperation(value = "所有反控操作记录数据列表")
    @PreAuthorize("hasAuthority('index.list')")
    @GetMapping("findAll")
    public Result<List<DeviceOption>> findAll() {
        return Result.ok(deviceOptionService.list());
    }

    @ApiOperation(value = "逻辑删除")
    @PreAuthorize("hasAuthority('index.remove')")
    @DeleteMapping("{id}")
    public Result<String> removeDeviceOption(@PathVariable String id) {
        return deviceOptionService.removeById(id) ? Result.ok() : Result.fail();
    }

    @ApiOperation(value = "条件查询分页方法")
    @PreAuthorize("hasAuthority('index.list')")
    @PostMapping("pageDeviceOptionCondition")
    public Result<Page<DeviceOptionVo>> pageDeviceOptionCondition(@RequestBody DeviceOptionQuery deviceOptionQuery) {
        return Result.ok(deviceOptionService.pageQuery(deviceOptionQuery));
    }

    @ApiOperation("根据ID查询反控操作记录数据")
    @PreAuthorize("hasAuthority('index.list')")
    @GetMapping("getDeviceOption/{id}")
    public Result<DeviceOption> getDeviceOption(@PathVariable String id) {
        return Result.ok(deviceOptionService.getById(id));
    }

    @ApiOperation("修改反控操作记录数据")
    @PreAuthorize("hasAuthority('index.list')")
    @PostMapping("updateDeviceOption")
    public Result<String> updateDeviceOption(@RequestBody DeviceOption deviceOption) {
        return deviceOptionService.updateById(deviceOption) ? Result.ok() : Result.fail("修改失败");
    }

    @ApiOperation("反控操作（登录即可，小程序通用；后续可单独加权限点）")
    @PostMapping("control")
    public Result<ControlResult> controlDevice(@RequestBody DeviceOptionControl deviceOption) {
        String username = SecurityContextHolder.getContext().getAuthentication().getName();
        try {
            ControlResult result = deviceOptionService.controlDevice(deviceOption, username);
            return Result.ok(result, "指令已下达");
        } catch (IllegalArgumentException | IllegalStateException e) {
            return Result.fail(e.getMessage());
        }
    }
}
