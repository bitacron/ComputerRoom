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
import com.example.room.control.entity.vo.ControlResult;
import com.example.room.control.entity.vo.DeviceOptionVo;
import com.example.room.control.entity.enums.DeviceCommandEnum;
import com.example.room.control.entity.enums.DeviceTypeEnum;
import com.example.room.control.mapper.DeviceOptionMapper;
import com.example.room.control.service.DeviceOptionService;
import com.example.room.device.entity.Device;
import com.example.room.device.service.DeviceService;
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
    private UserService userService;

    @Resource
    private CommandService commandService;

    @Resource
    private DeviceService deviceService;

    @Resource
    private WebSocketPushUtil webSocketPushUtil;

    @Resource
    private RequestIdGenerator requestIdGenerator;
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
                .map(DeviceOption::getOperatorCode)
                .filter(StringUtils::isNotBlank)
                .distinct()
                .collect(Collectors.toList());

        Map<String, String> userMap = new HashMap<>();
        if (!operatorList.isEmpty()) {
            QueryWrapper<User> userQueryWrapper = new QueryWrapper<>();
            userQueryWrapper.lambda().in(User::getUsername, operatorList);
            List<User> userList = userService.list(userQueryWrapper);
            userMap = userList.stream()
                    .collect(Collectors.toMap(
                            User::getUsername,
                            User::getNickName,
                            (a, b) -> a
                    ));
        }
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


    @Override
    public ControlResult controlDevice(DeviceOptionControl deviceOptionControl, String operatorCode) {
        if (deviceOptionControl == null || StringUtils.isBlank(deviceOptionControl.getDeviceKey())) {
            throw new IllegalArgumentException("请选择设备");
        }
        DeviceCommandEnum commandEnum = DeviceCommandEnum.fromCode(deviceOptionControl.getCommand());
        DeviceTypeEnum typeEnum = DeviceTypeEnum.fromCode(deviceOptionControl.getDeviceType());
        if (commandEnum == null || typeEnum == null) {
            throw new IllegalArgumentException("指令无效");
        }
        Device device = deviceService.getByDeviceKey(deviceOptionControl.getDeviceKey());
        if (device == null) {
            throw new IllegalStateException("设备不存在");
        }
        if (device.getOnlineStatus() == null || device.getOnlineStatus() != 1) {
            throw new IllegalStateException("设备离线，无法下发指令");
        }

        String cmdId = requestIdGenerator.nextId();
        String act = typeEnum.getCode().toLowerCase();
        int val = commandEnum.getIntValue();
        String topic = "room/" + deviceOptionControl.getDeviceKey() + "/command";
        JSONObject payload = new JSONObject();
        payload.put("cmdId", cmdId);
        payload.put("dev", deviceOptionControl.getDeviceKey());
        payload.put("act", act);
        payload.put("val", val);
        boolean success = mqttSendMessageService.sendMessage(topic, payload.toJSONString());
        if (!success) {
            throw new IllegalStateException("指令发送失败，请稍后重试");
        }

        Date now = new Date();
        DeviceOption deviceOption = new DeviceOption();
        deviceOption.setDeviceKey(deviceOptionControl.getDeviceKey());
        deviceOption.setAction(commandEnum.getValue() + "-" + typeEnum.getName());
        deviceOption.setOperatorCode(operatorCode);
        deviceOption.setGmtCreate(now);
        deviceOption.setIsDeleted(false);
        this.save(deviceOption);

        Command command = new Command();
        command.setCmdId(cmdId);
        command.setDeviceKey(deviceOptionControl.getDeviceKey());
        command.setDeviceType(act);
        command.setCommand(deviceOptionControl.getCommand());
        command.setStatus(1);
        command.setGmtCreate(now);
        command.setIsDeleted(false);
        commandService.save(command);
        return ControlResult.sent(cmdId);
    }

    @Override
    public void onMqttMessage(String topic, String payload) {
        JSONObject json = JSON.parseObject(payload);
        String deviceKeyStr = json.getString("dev");
        if (deviceKeyStr == null) {
            deviceKeyStr = "";
        }
        Integer fanStatus = json.getInteger("fan");
        Integer ledStatus = json.getInteger("led");
        Integer result = json.getInteger("result");
        String act = json.getString("act");
        Integer val = json.getInteger("val");
        Device device = deviceService.applyActuatorSnapshot(deviceKeyStr, fanStatus, ledStatus);

        Command command = null;
        if (json.containsKey("cmdId") && json.get("cmdId") != null) {
            QueryWrapper<Command> queryWrapper = new QueryWrapper<>();
            queryWrapper.lambda().eq(Command::getCmdId, json.get("cmdId").toString());
            queryWrapper.lambda().eq(Command::getStatus, 1);
            queryWrapper.lambda().orderByDesc(Command::getGmtCreate);
            queryWrapper.lambda().last("limit 1");
            command = commandService.getOne(queryWrapper);
            if (command != null) {
                boolean gpioMatched = result != null && result == 0;
                if (gpioMatched && "fan".equalsIgnoreCase(act) && fanStatus != null && val != null) {
                    gpioMatched = fanStatus.equals(val);
                } else if (gpioMatched && "led".equalsIgnoreCase(act) && ledStatus != null && val != null) {
                    gpioMatched = ledStatus.equals(val);
                }
                command.setStatus(gpioMatched ? 2 : 3);
                commandService.updateById(command);
                pushCommandResult(command, result, fanStatus, ledStatus);
            }
        }

        Environment snapshot = deviceService.toRealtimeEnvironment(device);
        if (snapshot != null) {
            webSocketPushUtil.pushToTopic("/topic/environment/" + deviceKeyStr, snapshot);
        }
    }

    @Override
    public void markCommandTimeout(Command command) {
        if (command == null || command.getStatus() == null || command.getStatus() != 1) {
            return;
        }
        command.setStatus(4);
        commandService.updateById(command);
        pushCommandResult(command, null, null, null);
    }

    private void pushCommandResult(Command command, Integer result, Integer fan, Integer led) {
        JSONObject body = new JSONObject();
        body.put("cmdId", command.getCmdId());
        body.put("deviceKey", command.getDeviceKey());
        body.put("status", command.getStatus());
        body.put("act", command.getDeviceType());
        body.put("command", command.getCommand());
        if (result != null) {
            body.put("result", result);
        }
        if (fan != null) {
            body.put("fan", fan);
        }
        if (led != null) {
            body.put("led", led);
        }
        webSocketPushUtil.pushToTopic("/topic/command/" + command.getDeviceKey(), body);
    }
}
