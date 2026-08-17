package com.example.room.mqtt.service;

import com.baomidou.mybatisplus.extension.plugins.pagination.Page;
import com.baomidou.mybatisplus.extension.service.IService;
import com.example.room.mqtt.entity.MqttSendCmd;
import com.example.room.mqtt.entity.dto.MqttSendCmdQuery;

/**
 * <p>
 * MQTT发送指令 服务类
 * </p>
 *
 * @author zhmy
 * @since 2023-05-31
 */
public interface MqttSendCmdService extends IService<MqttSendCmd> {
    // 条件查询分页方法
    Page<MqttSendCmd> pageQuery(MqttSendCmdQuery sendQuery);

    MqttSendCmd getLastSendCmd();
}
