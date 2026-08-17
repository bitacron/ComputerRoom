package com.example.room.device.service.impl;

import com.baomidou.mybatisplus.core.conditions.query.LambdaQueryWrapper;
import com.baomidou.mybatisplus.core.conditions.query.QueryWrapper;
import com.baomidou.mybatisplus.extension.service.impl.ServiceImpl;
import com.example.room.device.entity.UserDevicePreference;
import com.example.room.device.mapper.UserDevicePreferenceMapper;
import com.example.room.device.service.UserDevicePreferenceService;
import org.springframework.stereotype.Service;
import org.springframework.transaction.annotation.Transactional;

import java.util.List;

/**
 * 用户设备偏好 Service 实现类
 */
@Service
public class UserDevicePreferenceServiceImpl extends ServiceImpl<UserDevicePreferenceMapper, UserDevicePreference> implements UserDevicePreferenceService {

    @Override
    public boolean hasDevice(String userCode, String deviceKey) {
        LambdaQueryWrapper<UserDevicePreference> wrapper = new LambdaQueryWrapper<>();
        wrapper.eq(UserDevicePreference::getUserCode, userCode)
               .eq(UserDevicePreference::getDeviceKey, deviceKey);
        return this.count(wrapper) > 0;
    }

    @Override
    public UserDevicePreference getDefaultDeviceKey(String userCode) {
        QueryWrapper<UserDevicePreference> wrapper = new QueryWrapper<>();
        wrapper.lambda().eq(UserDevicePreference::getUserCode, userCode)
                .orderByDesc(UserDevicePreference::getId);
        wrapper.lambda().last("limit 1");
        return this.getOne(wrapper);
    }

    @Override
    @Transactional(rollbackFor = Exception.class)
    public boolean setDefaultDevice(String userCode, String deviceKey) {
        // 1. 如果 userCode + deviceKey 有记录，什么都不做
        if (hasDevice(userCode, deviceKey)) {
            return true;
        }

        // 2. 查询该用户是否已绑定其他设备
        LambdaQueryWrapper<UserDevicePreference> wrapper = new LambdaQueryWrapper<>();
        wrapper.eq(UserDevicePreference::getUserCode, userCode);
        UserDevicePreference existingDevice = this.getOne(wrapper);

        if (existingDevice != null) {
            // 3. 如果绑定了其他设备，更新为此 deviceKey
            existingDevice.setDeviceKey(deviceKey);
            return this.updateById(existingDevice);
        } else {
            // 4. 如果没有记录，添加一条
            UserDevicePreference userDevice = new UserDevicePreference();
            userDevice.setUserCode(userCode);
            userDevice.setDeviceKey(deviceKey);
            return this.save(userDevice);
        }
    }

}