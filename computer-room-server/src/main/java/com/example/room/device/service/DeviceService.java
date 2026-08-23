package com.example.room.device.service;

import com.baomidou.mybatisplus.extension.plugins.pagination.Page;
import com.baomidou.mybatisplus.extension.service.IService;
import com.example.room.device.entity.Device;
import com.example.room.device.entity.query.DeviceQuery;
import com.example.room.environment.entity.Environment;


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

    /**
     * 设备上线/注册：可携带名称与产品标识
     */
    void onlineDevice(String deviceKey, String deviceName, String productKey);

    /**
     * 设备离线（含 Last Will 异常掉线）
     */
    void offlineDevice(String deviceKey);

    void reportData(String deviceKey);

    /**
     * 上报刷新设备当前态。更旧的测量包不覆盖影子；
     * 执行器保护窗口内不覆盖 fan/led。
     *
     * @return 当前设备；设备不存在时返回 null
     */
    Device applyReportSnapshot(String deviceKey, Environment reported);

    /**
     * 反控 ACK 局部更新风扇/LED，并刷新 lastActuatorTime。
     */
    Device applyActuatorSnapshot(String deviceKey, Integer fanStatus, Integer ledStatus);

    /**
     * 将设备当前态转成实时页使用的 Environment 形状；尚无快照时返回 null。
     */
    Environment toRealtimeEnvironment(Device device);
}
