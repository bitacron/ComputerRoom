package com.example.room.mqtt.service;

import com.baomidou.mybatisplus.extension.plugins.pagination.Page;
import com.baomidou.mybatisplus.extension.service.IService;
import com.example.room.mqtt.entity.MqttReceiveCmdResp;
import com.example.room.mqtt.entity.dto.MqttReceiveCmdRespQuery;

/**
 * <p>
 * Mqtt接收 服务类
 * </p>
 *
 * @author zhmy
 * @since 2023-05-31
 */
public interface MqttReceiveCmdRespService extends IService<MqttReceiveCmdResp> {
    // 条件查询分页方法
    Page<MqttReceiveCmdResp> pageQuery(MqttReceiveCmdRespQuery receiveQuery);

    MqttReceiveCmdResp getLastReceiveCmdResp();
}
