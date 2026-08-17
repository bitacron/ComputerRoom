package com.example.room.device.service;

import com.baomidou.mybatisplus.extension.plugins.pagination.Page;
import com.baomidou.mybatisplus.extension.service.IService;
import com.example.room.device.entity.Device;
import com.example.room.device.entity.query.DeviceQuery;

import java.util.Map;


/**
 * <p>
 * 设备 服务类
 * </p>
 *
 * @author zhmy
 * @since 2023-05-31
 */
public interface DeviceService extends IService<Device> {

    /**
     * 分页查询设备列表
     * @param query 查询信息
     * @return 分页结果
     */
    Page<Device> pageQuery(DeviceQuery query);

    /**
     * 根据 deviceKey 查询设备
     * @param deviceKey 设备标识
     * @return 设备信息
     */
    Device getByDeviceKey(String deviceKey);

    /**
     * 更新设备在线状态
     * @param deviceKey 设备标识
     * @param status 在线状态
     */
    void updateOnlineStatus(String deviceKey, Integer status);

    /**
     * 更新设备活跃时间
     * @param deviceKey 设备标识
     */
    void updateActiveTime(String deviceKey);

    void onlineDevice(String deviceKey);

    void reportData(String deviceKey);
}
