package com.example.room.device.controller;

import cn.hutool.core.bean.BeanUtil;
import com.baomidou.mybatisplus.extension.plugins.pagination.Page;
import com.example.room.device.entity.Device;
import com.example.room.device.entity.query.DeviceQuery;
import com.example.room.device.entity.vo.DeviceSelect;
import com.example.room.device.service.DeviceService;
import com.example.room.util.Result;
import io.swagger.annotations.Api;
import io.swagger.annotations.ApiOperation;
import io.swagger.annotations.ApiParam;
import org.springframework.security.access.prepost.PreAuthorize;
import org.springframework.web.bind.annotation.*;

import javax.annotation.Resource;
import java.util.List;

@Api(description = "设备管理")
@RestController
@RequestMapping("/service/device")
public class DeviceController {

    @Resource
    private DeviceService deviceService;

    @ApiOperation(value = "所有设备列表")
    @PreAuthorize("hasAuthority('device.list')")
    @GetMapping("findAll")
    public Result<List<Device>> findAll() {
        return Result.ok(deviceService.list());
    }

    @ApiOperation(value = "分页查询设备列表")
    @PreAuthorize("hasAuthority('device.list')")
    @PostMapping("pageDeviceCondition")
    public Result<Page<Device>> pageQuery(@RequestBody(required = false) DeviceQuery deviceQuery) {
        return Result.ok(deviceService.pageQuery(deviceQuery));
    }

    @ApiOperation(value = "逻辑删除")
    @PreAuthorize("hasAuthority('device.remove')")
    @DeleteMapping("{id}")
    public Result<String> removeDevice(@PathVariable String id) {
        return deviceService.removeById(id) ? Result.ok() : Result.fail();
    }

    @ApiOperation("根据 ID 查询设备")
    @PreAuthorize("hasAuthority('device.list')")
    @GetMapping("getDevice/{id}")
    public Result<Device> getDevice(@PathVariable String id) {
        return Result.ok(deviceService.getById(id));
    }

    @ApiOperation("设备下拉列表")
    @GetMapping("getDeviceSelect")
    public Result<List<DeviceSelect>> getDeviceSelect() {
        List<Device> list = deviceService.list();
        return Result.ok(BeanUtil.copyToList(list, DeviceSelect.class));
    }

    @ApiOperation("根据 deviceKey 查询设备")
    @PreAuthorize("hasAuthority('device.list')")
    @GetMapping("getByDeviceKey/{deviceKey}")
    public Result<Device> getByDeviceKey(@PathVariable String deviceKey) {
        Device device = deviceService.getByDeviceKey(deviceKey);
        return device != null ? Result.ok(device) : Result.fail("设备不存在");
    }

    @ApiOperation("修改设备信息")
    @PreAuthorize("hasAuthority('device.update')")
    @PostMapping("updateDevice")
    public Result<String> updateDevice(@RequestBody Device device) {
        return deviceService.updateById(device) ? Result.ok() : Result.fail("修改失败");
    }

    @ApiOperation("新增设备")
    @PreAuthorize("hasAuthority('device.add')")
    @PostMapping("addDevice")
    public Result<String> addDevice(@RequestBody Device device) {
        return deviceService.save(device) ? Result.ok() : Result.fail("新增失败");
    }

    @ApiOperation("更新设备在线状态")
    @PreAuthorize("hasAuthority('device.update')")
    @PostMapping("updateOnlineStatus")
    public Result<String> updateOnlineStatus(
            @ApiParam(name = "deviceKey", value = "设备标识", required = true) @RequestParam String deviceKey,
            @ApiParam(name = "status", value = "在线状态：0-离线，1-在线", required = true) @RequestParam Integer status) {
        deviceService.updateOnlineStatus(deviceKey, status);
        return Result.ok();
    }
}
