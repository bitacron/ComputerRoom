package com.example.room.device.service.impl;

import com.baomidou.mybatisplus.core.conditions.query.QueryWrapper;
import com.baomidou.mybatisplus.extension.plugins.pagination.Page;
import com.baomidou.mybatisplus.extension.service.impl.ServiceImpl;
import com.example.room.device.entity.Device;
import com.example.room.device.entity.query.DeviceQuery;
import com.example.room.device.mapper.DeviceMapper;
import com.example.room.device.service.DeviceService;
import com.example.room.environment.entity.Environment;
import org.apache.commons.lang3.StringUtils;
import org.springframework.stereotype.Service;
import org.springframework.transaction.annotation.Transactional;

import java.util.Date;

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

    /** 反控 ACK 后，这段时间内上报不覆盖 fan/led，避免旧采样包把新状态打回去 */
    private static final long ACTUATOR_PROTECT_MS = 15_000L;

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
        onlineDevice(deviceKey, null, null);
    }

    @Transactional
    @Override
    public void onlineDevice(String deviceKey, String deviceName, String productKey) {
        QueryWrapper<Device> queryWrapper = new QueryWrapper<>();
        queryWrapper.lambda().eq(Device::getDeviceKey, deviceKey);
        Device device = this.getOne(queryWrapper);
        Date now = new Date();
        if (device != null) {
            device.setOnlineStatus(1);
            device.setLastOnlineTime(now);
            device.setLastActiveTime(now);
            if (deviceName != null && !deviceName.isEmpty()) {
                device.setDeviceName(deviceName);
            }
            if (productKey != null && !productKey.isEmpty()) {
                device.setProductKey(productKey);
            }
            this.update(device, queryWrapper);
        } else {
            device = new Device();
            device.setProductKey(productKey != null && !productKey.isEmpty() ? productKey : "room");
            device.setDeviceKey(deviceKey);
            device.setDeviceName(deviceName != null && !deviceName.isEmpty() ? deviceName : deviceKey);
            device.setOnlineStatus(1);
            device.setLastOnlineTime(now);
            device.setLastActiveTime(now);
            this.save(device);
        }
    }

    @Transactional
    @Override
    public void offlineDevice(String deviceKey) {
        if (deviceKey == null || deviceKey.isEmpty()) {
            return;
        }
        QueryWrapper<Device> queryWrapper = new QueryWrapper<>();
        queryWrapper.lambda().eq(Device::getDeviceKey, deviceKey);
        Device device = this.getOne(queryWrapper);
        if (device == null) {
            return;
        }
        device.setOnlineStatus(0);
        device.setLastOfflineTime(new Date());
        this.update(device, queryWrapper);
    }

    @Override
    public void reportData(String deviceKey) {
        QueryWrapper<Device> queryWrapper = new QueryWrapper<>();
        queryWrapper.lambda().eq(Device::getDeviceKey, deviceKey);
        Device device = this.getOne(queryWrapper);
        if (device != null) {
            Date now = new Date();
            // report 同时作为业务存活证明：离线后恢复上报时自动拉回在线
            if (device.getOnlineStatus() == null || device.getOnlineStatus() != 1) {
                device.setOnlineStatus(1);
                device.setLastOnlineTime(now);
            }
            device.setLastActiveTime(now);
            this.update(device, queryWrapper);
        }
    }

    @Transactional
    @Override
    public Device applyReportSnapshot(String deviceKey, Environment reported) {
        if (StringUtils.isBlank(deviceKey) || reported == null) {
            return null;
        }
        QueryWrapper<Device> queryWrapper = new QueryWrapper<>();
        queryWrapper.lambda().eq(Device::getDeviceKey, deviceKey);
        Device device = this.getOne(queryWrapper);
        if (device == null) {
            return null;
        }
        Date now = new Date();
        markOnlineAndActive(device, now);

        Date measureTime = reported.getGmtMeasurement() != null ? reported.getGmtMeasurement() : now;
        if (device.getLastReportTime() != null && measureTime.before(device.getLastReportTime())) {
            this.updateById(device);
            return device;
        }

        device.setTemperature(reported.getTemperature());
        device.setHumidity(reported.getHumidity());
        device.setGasPpm(reported.getGasPpm());
        device.setGasStatus(reported.getGasStatus());
        device.setLightStatus(reported.getLightStatus());
        device.setFlameStatus(reported.getFlameStatus());
        device.setLightPercentage(reported.getLightPercentage());
        device.setFlamePercentage(reported.getFlamePercentage());
        device.setAlarmStatus(reported.getAlarmStatus());
        if (!inActuatorProtectWindow(device, now)) {
            device.setFanStatus(reported.getFanStatus());
            device.setLedStatus(reported.getLedStatus());
        }
        device.setLastReportTime(measureTime);
        this.updateById(device);
        return device;
    }

    @Transactional
    @Override
    public Device applyActuatorSnapshot(String deviceKey, Integer fanStatus, Integer ledStatus) {
        if (StringUtils.isBlank(deviceKey)) {
            return null;
        }
        QueryWrapper<Device> queryWrapper = new QueryWrapper<>();
        queryWrapper.lambda().eq(Device::getDeviceKey, deviceKey);
        Device device = this.getOne(queryWrapper);
        if (device == null) {
            return null;
        }
        Date now = new Date();
        markOnlineAndActive(device, now);
        boolean actuatorChanged = false;
        if (fanStatus != null) {
            device.setFanStatus(fanStatus);
            actuatorChanged = true;
        }
        if (ledStatus != null) {
            device.setLedStatus(ledStatus);
            actuatorChanged = true;
        }
        if (actuatorChanged) {
            device.setLastActuatorTime(now);
        }
        this.updateById(device);
        return device;
    }

    @Override
    public Environment toRealtimeEnvironment(Device device) {
        if (device == null) {
            return null;
        }
        if (device.getLastReportTime() == null
                && device.getTemperature() == null
                && device.getFanStatus() == null
                && device.getLedStatus() == null) {
            return null;
        }
        Environment environment = new Environment();
        environment.setSource(1);
        environment.setDeviceKey(device.getDeviceKey());
        if (device.getTemperature() != null) {
            environment.setTemperature(device.getTemperature());
        }
        if (device.getHumidity() != null) {
            environment.setHumidity(device.getHumidity());
        }
        if (device.getGasPpm() != null) {
            environment.setGasPpm(device.getGasPpm());
        }
        environment.setGasStatus(device.getGasStatus());
        environment.setLightStatus(device.getLightStatus());
        environment.setFlameStatus(device.getFlameStatus());
        if (device.getLightPercentage() != null) {
            environment.setLightPercentage(device.getLightPercentage());
        }
        if (device.getFlamePercentage() != null) {
            environment.setFlamePercentage(device.getFlamePercentage());
        }
        environment.setAlarmStatus(device.getAlarmStatus());
        environment.setFanStatus(device.getFanStatus());
        environment.setLedStatus(device.getLedStatus());
        environment.setGmtMeasurement(device.getLastReportTime());
        environment.setGmtCreate(device.getLastActiveTime());
        return environment;
    }

    private void markOnlineAndActive(Device device, Date now) {
        if (device.getOnlineStatus() == null || device.getOnlineStatus() != 1) {
            device.setOnlineStatus(1);
            device.setLastOnlineTime(now);
        }
        device.setLastActiveTime(now);
    }

    private boolean inActuatorProtectWindow(Device device, Date now) {
        Date lastActuatorTime = device.getLastActuatorTime();
        if (lastActuatorTime == null) {
            return false;
        }
        return now.getTime() - lastActuatorTime.getTime() < ACTUATOR_PROTECT_MS;
    }
}
