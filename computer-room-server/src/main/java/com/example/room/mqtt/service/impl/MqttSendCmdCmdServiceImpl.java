package com.example.room.mqtt.service.impl;

import com.baomidou.mybatisplus.core.conditions.query.QueryWrapper;
import com.baomidou.mybatisplus.extension.plugins.pagination.Page;
import com.baomidou.mybatisplus.extension.service.impl.ServiceImpl;
import com.example.room.environment.entity.Environment;
import com.example.room.mqtt.entity.MqttSendCmd;
import com.example.room.mqtt.entity.dto.MqttSendCmdQuery;
import com.example.room.mqtt.mapper.MqttSendCmdMapper;
import com.example.room.mqtt.service.MqttSendCmdService;
import lombok.extern.slf4j.Slf4j;
import org.apache.commons.lang3.StringUtils;
import org.springframework.stereotype.Service;

@Slf4j
@Service
public class MqttSendCmdCmdServiceImpl extends ServiceImpl<MqttSendCmdMapper, MqttSendCmd> implements MqttSendCmdService {
    @Override
    public Page<MqttSendCmd> pageQuery(MqttSendCmdQuery sendQuery) {
        // 创建page
        Page<MqttSendCmd> mqttSendPage = new Page<>(sendQuery.getCurrentPage(), sendQuery.getPageSize());
        // 构建条件
        QueryWrapper<MqttSendCmd> queryWrapper = new QueryWrapper<>();

        if (!StringUtils.isEmpty(sendQuery.getBegin())) {
            queryWrapper.lambda().ge(MqttSendCmd::getSendTime, sendQuery.getBegin()); // ge大于
        }
        if (!StringUtils.isEmpty(sendQuery.getDeviceKey())) {
            queryWrapper.lambda().eq(MqttSendCmd::getDeviceKey, sendQuery.getDeviceKey());
        }
        if (!StringUtils.isEmpty(sendQuery.getEnd())) {
            queryWrapper.lambda().le(MqttSendCmd::getSendTime, sendQuery.getEnd()); // le小于
        }
        // 排序
        queryWrapper.lambda().orderByDesc(MqttSendCmd::getSendTime);
        return baseMapper.selectPage(mqttSendPage, queryWrapper);
    }

    @Override
    public MqttSendCmd getLastSendCmd() {
        QueryWrapper<MqttSendCmd> queryWrapper = new QueryWrapper<>();
        queryWrapper.lambda().orderByDesc(MqttSendCmd::getSendTime);
        queryWrapper.last("limit 1");
        // Service层
        return this.getOne(queryWrapper);
    }
}