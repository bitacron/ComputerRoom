package com.example.room.mqtt.service;

import com.baomidou.mybatisplus.extension.plugins.pagination.Page;
import com.baomidou.mybatisplus.extension.service.IService;
import com.example.room.mqtt.entity.MqttReceiveReport;
import com.example.room.mqtt.entity.dto.MqttReceiveReportQuery;

/**
 * <p>
 * Mqtt接收 服务类
 * </p>
 *
 * @author zhmy
 * @since 2023-05-31
 */
public interface MqttReceiveReportService extends IService<MqttReceiveReport> {
    // 条件查询分页方法
    Page<MqttReceiveReport> pageQuery(MqttReceiveReportQuery receiveQuery);

    MqttReceiveReport getLastReport();
}
