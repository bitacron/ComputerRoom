package com.example.room.mqtt.service.impl;

import com.baomidou.mybatisplus.core.conditions.query.QueryWrapper;
import com.baomidou.mybatisplus.extension.plugins.pagination.Page;
import com.baomidou.mybatisplus.extension.service.impl.ServiceImpl;
import com.example.room.environment.entity.Environment;
import com.example.room.mqtt.entity.MqttReceiveReport;
import com.example.room.mqtt.entity.dto.MqttReceiveReportQuery;
import com.example.room.mqtt.mapper.MqttReceiveReportMapper;
import com.example.room.mqtt.service.MqttReceiveReportService;
import lombok.extern.slf4j.Slf4j;
import org.springframework.stereotype.Service;
import org.apache.commons.lang3.StringUtils;

@Slf4j
@Service
public class MqttReceiveReportReportServiceImpl extends ServiceImpl<MqttReceiveReportMapper, MqttReceiveReport> implements MqttReceiveReportService {

    @Override
    public Page<MqttReceiveReport> pageQuery(MqttReceiveReportQuery receiveQuery) {
        // 创建page
        Page<MqttReceiveReport> mqttReceivePage = new Page<>(receiveQuery.getCurrentPage(), receiveQuery.getPageSize());
        // 构建条件
        QueryWrapper<MqttReceiveReport> queryWrapper = new QueryWrapper<>();
        if (!StringUtils.isEmpty(receiveQuery.getBegin())) {
            queryWrapper.lambda().ge(MqttReceiveReport::getReceiveTime, receiveQuery.getBegin()); // ge大于
        }
        if (!StringUtils.isEmpty(receiveQuery.getDeviceKey())) {
            queryWrapper.lambda().eq(MqttReceiveReport::getDeviceKey, receiveQuery.getDeviceKey());
        }
        if (!StringUtils.isEmpty(receiveQuery.getEnd())) {
            queryWrapper.lambda().le(MqttReceiveReport::getReceiveTime, receiveQuery.getEnd()); // le小于
        }
        // 排序
        queryWrapper.lambda().orderByDesc(MqttReceiveReport::getReceiveTime);
        return baseMapper.selectPage(mqttReceivePage, queryWrapper);
    }

    @Override
    public MqttReceiveReport getLastReport() {
        QueryWrapper<MqttReceiveReport> queryWrapper = new QueryWrapper<>();
        queryWrapper.lambda().orderByDesc(MqttReceiveReport::getReceiveTime);
        queryWrapper.last("limit 1");
        // Service层
        return this.getOne(queryWrapper);
    }
}