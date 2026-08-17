package com.example.room.device.service;

import com.baomidou.mybatisplus.extension.service.IService;
import com.example.room.device.entity.UserDevicePreference;

import java.util.List;

/**
 * 用户设备偏好 Service 接口
 */
public interface UserDevicePreferenceService extends IService<UserDevicePreference> {

    /**
     * 检查用户是否拥有某设备
     * @param userCode 用户编码
     * @param deviceKey 设备标识
     * @return 是否拥有
     */
    boolean hasDevice(String userCode, String deviceKey);

    /**
     * 获取用户的默认设备（最近添加的）
     * @param userCode 用户编码
     * @return 默认设备Key
     */
    UserDevicePreference getDefaultDeviceKey(String userCode);
    
    /**
     * 设置用户的默认设备（实际是确保该设备存在，默认使用最新选择的）
     * @param userCode 用户编码
     * @param deviceKey 设备标识
     * @return 是否成功
     */
    boolean setDefaultDevice(String userCode, String deviceKey);
}