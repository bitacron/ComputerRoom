package com.example.room.mqtt.service.impl;

import com.baomidou.mybatisplus.core.conditions.query.QueryWrapper;
import com.baomidou.mybatisplus.extension.plugins.pagination.Page;
import com.baomidou.mybatisplus.extension.service.impl.ServiceImpl;
import com.example.room.environment.entity.Environment;
import com.example.room.mqtt.entity.MqttReceiveCmdResp;
import com.example.room.mqtt.entity.dto.MqttReceiveCmdRespQuery;
import com.example.room.mqtt.mapper.MqttReceiveCmdRespMapper;
import com.example.room.mqtt.service.MqttReceiveCmdRespService;
import lombok.extern.slf4j.Slf4j;
import org.apache.commons.lang3.StringUtils;
import org.springframework.stereotype.Service;

@Slf4j
@Service
public class MqttReceiveCmdRespServiceImpl extends ServiceImpl<MqttReceiveCmdRespMapper, MqttReceiveCmdResp> implements MqttReceiveCmdRespService {

    @Override
    public Page<MqttReceiveCmdResp> pageQuery(MqttReceiveCmdRespQuery receiveQuery) {
        // 创建page
        Page<MqttReceiveCmdResp> mqttReceivePage = new Page<>(receiveQuery.getCurrentPage(), receiveQuery.getPageSize());
        // 构建条件
        QueryWrapper<MqttReceiveCmdResp> queryWrapper = new QueryWrapper<>();
        if (!StringUtils.isEmpty(receiveQuery.getBegin())) {
            queryWrapper.lambda().ge(MqttReceiveCmdResp::getReceiveTime, receiveQuery.getBegin()); // ge大于
        }
        if (!StringUtils.isEmpty(receiveQuery.getEnd())) {
            queryWrapper.lambda().le(MqttReceiveCmdResp::getReceiveTime, receiveQuery.getEnd()); // le小于
        }
        if (!StringUtils.isEmpty(receiveQuery.getDeviceKey())) {
            queryWrapper.lambda().eq(MqttReceiveCmdResp::getDeviceKey, receiveQuery.getDeviceKey());
        }
        // 排序
        queryWrapper.lambda().orderByDesc(MqttReceiveCmdResp::getReceiveTime);
        Page<MqttReceiveCmdResp> page = this.page(mqttReceivePage, queryWrapper);
        return baseMapper.selectPage(mqttReceivePage, queryWrapper);
    }

    @Override
    public MqttReceiveCmdResp getLastReceiveCmdResp() {
        QueryWrapper<MqttReceiveCmdResp> queryWrapper = new QueryWrapper<>();
        queryWrapper.lambda().orderByDesc(MqttReceiveCmdResp::getReceiveTime);
        queryWrapper.last("limit 1");
        // Service层
        return this.getOne(queryWrapper);
    }
}