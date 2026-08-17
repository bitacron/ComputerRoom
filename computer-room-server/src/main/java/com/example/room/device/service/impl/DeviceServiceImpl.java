package com.example.room.device.service.impl;

import com.baomidou.mybatisplus.core.conditions.query.QueryWrapper;
import com.baomidou.mybatisplus.extension.plugins.pagination.Page;
import com.baomidou.mybatisplus.extension.service.impl.ServiceImpl;
import com.example.room.device.entity.Device;
import com.example.room.device.entity.query.DeviceQuery;
import com.example.room.device.mapper.DeviceMapper;
import com.example.room.device.service.DeviceService;
import org.springframework.stereotype.Service;
import org.springframework.transaction.annotation.Transactional;

import java.util.Date;
import java.util.Map;

/**
 * <p>
 * 设备 服务实现类
 * </p>
 *
 * @author zhmy
 * @since 2023-05-31
 */
@Service
public class DeviceServiceImpl extends ServiceImpl<DeviceMapper, Device> implements DeviceService {

    @Override
    public Page<Device> pageQuery(DeviceQuery query) {
        Page<Device> page = new Page<>(query.getCurrentPage(), query.getPageSize());
        QueryWrapper<Device> queryWrapper = new QueryWrapper<>();
        if (query.getDeviceKey() != null) {
            queryWrapper.lambda().eq(Device::getDeviceKey, query.getDeviceKey());
        }
        if (query.getOnlineStatus() != null) {
            queryWrapper.lambda().eq(Device::getOnlineStatus, query.getOnlineStatus());
        }
        if (query.getDeviceName() != null) {
            queryWrapper.lambda().like(Device::getDeviceName, query.getDeviceName());
        }
        queryWrapper.lambda().orderByDesc(Device::getLastActiveTime);
        return baseMapper.selectPage(page, queryWrapper);
    }

    @Override
    public Device getByDeviceKey(String deviceKey) {
        QueryWrapper<Device> queryWrapper = new QueryWrapper<>();
        queryWrapper.lambda().eq(Device::getDeviceKey, deviceKey);
        return baseMapper.selectOne(queryWrapper);
    }

    @Override
    public void updateOnlineStatus(String deviceKey, Integer status) {
        QueryWrapper<Device> queryWrapper = new QueryWrapper<>();
        queryWrapper.lambda().eq(Device::getDeviceKey, deviceKey);
        
        Device device = new Device();
        device.setOnlineStatus(status);
        
        if (status == 1) {
            device.setLastOnlineTime(new Date());
        } else {
            device.setLastOfflineTime(new Date());
        }
        
        baseMapper.update(device, queryWrapper);
    }

    @Override
    public void updateActiveTime(String deviceKey) {
        QueryWrapper<Device> queryWrapper = new QueryWrapper<>();
        queryWrapper.lambda().eq(Device::getDeviceKey, deviceKey);
        
        Device device = new Device();
        device.setLastActiveTime(new Date());
        
        baseMapper.update(device, queryWrapper);
    }

    @Transactional
    @Override
    public void onlineDevice(String deviceKey) {
        QueryWrapper<Device> queryWrapper = new QueryWrapper<>();
        queryWrapper.lambda().eq(Device::getDeviceKey, deviceKey);
        Device device = this.getOne(queryWrapper);
        if (device != null) {
            device.setOnlineStatus(1);
            device.setLastOnlineTime(new Date());
            device.setLastActiveTime(new Date());
            this.update(device, queryWrapper);
        } else {
            device = new Device();
            device.setProductKey("room");
            device.setDeviceKey(deviceKey);
            device.setDeviceName(deviceKey);
            device.setOnlineStatus(1);
            device.setLastOnlineTime(new Date());
            device.setLastActiveTime(new Date());
            this.save(device);
        }
    }

    @Override
    public void reportData(String deviceKey) {
        QueryWrapper<Device> queryWrapper = new QueryWrapper<>();
        queryWrapper.lambda().eq(Device::getDeviceKey, deviceKey);
        Device device = this.getOne(queryWrapper);
        if (device != null) {
            device.setLastActiveTime(new Date());
            this.update(device, queryWrapper);
        }
    }
}
