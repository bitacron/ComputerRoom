package com.example.room.control.service.impl;

import com.alibaba.fastjson.JSON;
import com.alibaba.fastjson.JSONObject;
import com.baomidou.mybatisplus.core.conditions.query.QueryWrapper;
import com.baomidou.mybatisplus.extension.plugins.pagination.Page;
import com.baomidou.mybatisplus.extension.service.impl.ServiceImpl;
import com.example.room.control.entity.Command;
import com.example.room.control.entity.DeviceOption;
import com.example.room.control.service.CommandService;
import com.example.room.environment.entity.Environment;
import com.example.room.control.entity.param.DeviceOptionControl;
import com.example.room.control.entity.param.DeviceOptionQuery;
import com.example.room.control.entity.vo.DeviceOptionVo;
import com.example.room.control.entity.enums.DeviceCommandEnum;
import com.example.room.control.entity.enums.DeviceTypeEnum;
import com.example.room.control.mapper.DeviceOptionMapper;
import com.example.room.control.service.DeviceOptionService;
import com.example.room.environment.service.EnvironmentService;
import com.example.room.access.entity.User;
import com.example.room.access.service.UserService;
import com.example.room.mqtt.common.MqttSendMessageService;
import com.example.room.util.RequestIdGenerator;
import com.example.room.util.WebSocketPushUtil;
import org.apache.commons.lang3.StringUtils;
import org.springframework.beans.BeanUtils;
import org.springframework.stereotype.Service;

import javax.annotation.Resource;
import java.util.*;
import java.util.concurrent.CompletableFuture;
import java.util.concurrent.ConcurrentHashMap;
import java.util.stream.Collectors;

/**
 * <p>
 * 反控记录 服务实现类
 * </p>
 *
 * @author zhmy
 * @since 2023-05-31
 */
@Service
public class DeviceOptionServiceImpl extends ServiceImpl<DeviceOptionMapper, DeviceOption> implements DeviceOptionService {

    @Resource
    private MqttSendMessageService mqttSendMessageService;
    @Resource
    private EnvironmentService environmentService;

    @Resource
    private UserService userService;

    @Resource
    private CommandService commandService;

    @Resource
    private WebSocketPushUtil webSocketPushUtil;
    @Override
    public Page<DeviceOptionVo> pageQuery(DeviceOptionQuery deviceOptionQuery) {
        Page<DeviceOption> page = new Page<>(deviceOptionQuery.getCurrentPage(), deviceOptionQuery.getPageSize());
        QueryWrapper<DeviceOption> queryWrapper = new QueryWrapper<>();
        if (!StringUtils.isEmpty(deviceOptionQuery.getBegin())) {
            queryWrapper.lambda().ge(DeviceOption::getGmtCreate, deviceOptionQuery.getBegin()); // ge大于
        }
        if (!StringUtils.isEmpty(deviceOptionQuery.getEnd())) {
            queryWrapper.lambda().le(DeviceOption::getGmtCreate, deviceOptionQuery.getEnd()); // le小于
        }
        queryWrapper.lambda().orderByDesc(DeviceOption::getGmtCreate);
        Page<DeviceOption> deviceOptionPage = baseMapper.selectPage(page, queryWrapper);

        // 创建新的 Page<DeviceOptionVo>，保持分页参数一致
        Page<DeviceOptionVo> voPage = new Page<>(
                deviceOptionPage.getCurrent(),
                deviceOptionPage.getSize(),
                deviceOptionPage.getTotal()
        );
        List<DeviceOption> records = deviceOptionPage.getRecords();
        List<String> operatorList = records.stream()
                .map(DeviceOption::getOperatorCode)  // 假设有getOperatorId()方法
                .collect(Collectors.toList());

        QueryWrapper<User> userQueryWrapper = new QueryWrapper<>();
        userQueryWrapper.lambda().in(User::getUsername, operatorList);

        List<User> userList = userService.list(userQueryWrapper);
        Map<String, String> userMap = userList.stream()
                .collect(Collectors.toMap(
                        User::getUsername,
                        User::getNickName
                ));
        // 将 DeviceOption 列表转换为 DeviceOptionVo 列表
        List<DeviceOptionVo> voList = new ArrayList<>();
        for (DeviceOption record : records) {
            DeviceOptionVo vo = new DeviceOptionVo();
            BeanUtils.copyProperties(record, vo);

            vo.setOperatorName(userMap.getOrDefault(record.getOperatorCode(), ""));
            voList.add(vo);
        }

        voPage.setRecords(voList);
        return voPage;

        // return baseMapper.selectPage(page, queryWrapper);
    }
    /**
     * 将 DeviceOption 转换为 DeviceOptionVo
     */


    private final ConcurrentHashMap<String, CompletableFuture<Boolean>> pendingRequests = new ConcurrentHashMap<>();
    private final RequestIdGenerator idGenerator = new RequestIdGenerator();
    @Override
    public boolean controlDevice(DeviceOptionControl deviceOptionControl, String operatorCode) {
        // 1. send发送MQTT消息到 设备

        String requestId = idGenerator.nextId();
        CompletableFuture<Boolean> future = new CompletableFuture<>();

        // 如果遇到极低概率的冲突（同一ID已存在），可自旋重试
        while (pendingRequests.putIfAbsent(requestId, future) != null) {
            requestId = idGenerator.nextId(); // 重新生成
        }
        String topic = "room" + "/" + deviceOptionControl.getDeviceKey() + "/command";

        // 使用枚举生成消息体
        DeviceCommandEnum commandEnum = DeviceCommandEnum.fromCode(deviceOptionControl.getCommand());
        DeviceTypeEnum typeEnum = DeviceTypeEnum.fromCode(deviceOptionControl.getDeviceType());
        String message = "{\"" + deviceOptionControl.getDeviceType() + "\":" +
                (commandEnum != null ? commandEnum.getIntValue() : 0) +
                ",\"cmdId\":\"" + requestId + "\"" +
                ",\"dev\":\"" + deviceOptionControl.getDeviceKey() + "\"" +
                "}";
        boolean success = mqttSendMessageService.sendMessage(topic, message);
        if (!success) {
            return false;
        }
        // 2. 更新数据库操作记录
        DeviceOption deviceOption = new DeviceOption();
        deviceOption.setDeviceKey(deviceOptionControl.getDeviceKey());
        assert commandEnum != null;
        deviceOption.setAction(commandEnum.getValue()+ "-"+typeEnum.getName());
        deviceOption.setOperatorCode(operatorCode);
        deviceOption.setGmtCreate(new Date());
        deviceOption.setIsDeleted(false);

        Command command = new Command();
        command.setCmdId(requestId);
        command.setDeviceKey(deviceOptionControl.getDeviceKey());
        command.setDeviceType(deviceOptionControl.getDeviceType());
        command.setCommand(deviceOptionControl.getCommand());
        command.setStatus(1);
        command.setGmtCreate(new Date());
        command.setIsDeleted(false);
        commandService.save(command);
        return true;
        // }
    }
    // MQTT 回调方法
    @Override
    public void onMqttMessage(String topic, String payload) {
        JSONObject json = JSON.parseObject(payload);
        // webSocketPushUtil.pushToTopic("/ws/environment", payload);
        String deviceKeyStr = "";
        if (json.containsKey("dev")) {
            Object deviceKey = json.get("dev");
            deviceKeyStr = deviceKey.toString();
        }
        // 重新推送一下环境数据
        Environment environment = environmentService.getLastData(deviceKeyStr);
        if (json.containsKey("fan")) {
            Object fan = json.get("fan");
            if (fan != null) {
                environment.setFanStatus(Integer.parseInt(fan.toString()));
            }
        }
        if (json.containsKey("led")) {
            Object led = json.get("led");
            if (led != null) {
                environment.setLedStatus(Integer.parseInt(led.toString()));
            }
        }
        if (json.containsKey("cmdId")) {
            QueryWrapper<Command> queryWrapper = new QueryWrapper<>();
            queryWrapper.lambda().eq(Command::getCmdId, json.get("cmdId"));
            queryWrapper.lambda().orderByDesc(Command::getGmtCreate);
            queryWrapper.lambda().last("limit 1");
            Command command = commandService.getOne(queryWrapper);
            if (command != null) {
                command.setStatus(2);
            }
            commandService.update(command, queryWrapper);
        }
        String socketTopic = "/topic/environment/"+ deviceKeyStr;
        webSocketPushUtil.pushToTopic(socketTopic, environment);
        // if (json.containsKey("cmdId")) {
        //     Object cmdId = json.get("cmdId").toString();
        //     CompletableFuture<Boolean> future = pendingRequests.remove(cmdId);
        //     if (future != null) {
        //         // 可以进一步检查回复内容是否匹配指令，但一般有 ID 就足够
        //         future.complete(true);
        //     }
        // }
    }
}
