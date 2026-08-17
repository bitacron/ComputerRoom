package com.example.room.device.controller;

import com.baomidou.mybatisplus.core.conditions.query.QueryWrapper;
import com.example.room.device.entity.Device;
import com.example.room.device.entity.UserDevicePreference;
import com.example.room.device.service.DeviceService;
import com.example.room.device.service.UserDevicePreferenceService;
import com.example.room.util.Result;
import org.springframework.security.access.prepost.PreAuthorize;
import org.springframework.security.core.context.SecurityContextHolder;
import org.springframework.web.bind.annotation.*;

import javax.annotation.Resource;
import java.util.Objects;

/**
 * 用户设备偏好
 */
@RestController
@RequestMapping("/service/device/default")
public class UserDevicePreferenceController {

    @Resource
    private UserDevicePreferenceService userDevicePreferenceService;

    @Resource
    private DeviceService deviceService;

    @PreAuthorize("hasAuthority('realTime.index')")
    @GetMapping("/getDefaultDevice")
    public Result<UserDevicePreference> getDefaultDevice() {
        String code = SecurityContextHolder.getContext().getAuthentication().getName();
        UserDevicePreference defaultDevice = userDevicePreferenceService.getDefaultDeviceKey(code);
        if (Objects.isNull(defaultDevice)) {
            QueryWrapper<Device> queryWrapper = new QueryWrapper<>();
            queryWrapper.lambda().orderByDesc(Device::getOnlineStatus).orderByDesc(Device::getLastActiveTime);
            queryWrapper.lambda().last("limit 1");
            Device one = deviceService.getOne(queryWrapper);
            defaultDevice = new UserDevicePreference();
            defaultDevice.setDeviceKey(one.getDeviceKey());
        }
        return Result.ok(defaultDevice);
    }

    @PreAuthorize("hasAuthority('realTime.index')")
    @PostMapping("/setDefaultDevice")
    public Result<Boolean> setDefaultDevice(@RequestBody(required = false) UserDevicePreference param) {
        String userCode = SecurityContextHolder.getContext().getAuthentication().getName();
        boolean success = userDevicePreferenceService.setDefaultDevice(userCode, param.getDeviceKey());
        return success ? Result.ok(true) : Result.fail("设置默认设备失败");
    }
}
