package com.example.room.mqtt.common;

import com.alibaba.fastjson.JSON;
import com.alibaba.fastjson.JSONObject;
import com.example.room.device.service.DeviceService;
import com.example.room.environment.entity.Environment;
import com.example.room.control.service.DeviceOptionService;
import com.example.room.environment.service.EnvironmentService;
import com.example.room.mqtt.entity.MqttReceiveCmdResp;
import com.example.room.mqtt.entity.MqttReceiveReport;
import com.example.room.mqtt.service.MqttReceiveCmdRespService;
import com.example.room.mqtt.service.MqttReceiveReportService;
import com.example.room.util.WebSocketPushUtil;
import lombok.extern.slf4j.Slf4j;
import org.springframework.beans.factory.annotation.Autowired;
import org.springframework.stereotype.Service;
import org.springframework.transaction.annotation.Transactional;

import javax.annotation.Resource;
import java.text.SimpleDateFormat;
import java.util.Date;

@Slf4j
@Service
public class MqttProcessMessageService {

    @Resource
    private MqttReceiveReportService mqttReceiveReportService;

    @Resource
    private MqttReceiveCmdRespService mqttReceiveCmdRespService;

    @Resource
    private EnvironmentService environmentService;

    @Resource
    private DeviceOptionService deviceOptionService;

    @Resource
    private DeviceService deviceService;

    @Autowired
    private WebSocketPushUtil webSocketPushUtil;
    @Transactional(rollbackFor = Exception.class)
    public void processMessage(String topic, String payload) {
        JSONObject jsonObject = JSON.parseObject(payload);
        String deviceKeyStr = "";
        if (jsonObject.containsKey("dev")) {
            Object deviceKey = jsonObject.get("dev");
            deviceKeyStr = deviceKey.toString();
        }
        if (topic.contains("report")) {
            // 1. 原始JSON直接落库
            MqttReceiveReport dataEntity = new MqttReceiveReport();
            dataEntity.setTopic(topic);
            dataEntity.setPayload(payload);
            dataEntity.setReceiveTime(new Date());
            dataEntity.setDeviceKey(deviceKeyStr);
            mqttReceiveReportService.save(dataEntity);
            deviceService.reportData(deviceKeyStr);
            // 2. 解析并处理环境数据
            processEnvironmentData(payload);
        } else if (topic.contains("resp")) {
            // 1. 原始JSON直接落库
            MqttReceiveCmdResp dataEntity = new MqttReceiveCmdResp();
            dataEntity.setTopic(topic);
            dataEntity.setPayload(payload);
            dataEntity.setReceiveTime(new Date());
            dataEntity.setDeviceKey(deviceKeyStr);
            mqttReceiveCmdRespService.save(dataEntity);
            deviceService.reportData(deviceKeyStr);
            // 2. 解析并处理报警数据
            deviceOptionService.onMqttMessage(topic, payload);
        } else if (topic.contains("heartbeat")) {
            deviceService.onlineDevice(deviceKeyStr);
        }
    }

    private void processEnvironmentData(String payload) {
        try {
            // 解析JSON字符串
            JSONObject jsonObject = JSON.parseObject(payload);

            /*
            {
              "dev": "stm32_01",
              "temp": 25,
              "humi": 47,
              "gasPPM": 1,
              "gasDig": 0,
              "ldrDig": 1,
              "ldrPer": 21,
              "flameDig": 0,
              "flamePer": 0,
              "alarm": 0,
              "fan": 0,
              "led": 0
            }
             */
            // 如果包含其中任意一个字段，则插入数据
            Environment environment = new Environment();
            // 设置默认值
            environment.setSource(1); // 来源：1上报
            environment.setGmtCreate(new Date());
            String deviceKeyStr = "";
            if (jsonObject.containsKey("dev")) {
                Object deviceKey = jsonObject.get("dev");
                deviceKeyStr = deviceKey.toString();
                environment.setDeviceKey(deviceKeyStr);
            }
            // 解析并设置各个字段值
            if (jsonObject.containsKey("temp")) {
                Object temperature = jsonObject.get("temp");
                if (temperature != null) {
                    environment.setTemperature(Float.parseFloat(temperature.toString()));
                }
            }

            if (jsonObject.containsKey("humi")) {
                Object humidity = jsonObject.get("humi");
                if (humidity != null) {
                    environment.setHumidity(Float.parseFloat(humidity.toString()));
                }
            }

            if (jsonObject.containsKey("gasPPM")) {
                Object smoke = jsonObject.get("gasPPM");
                if (smoke != null) {
                    environment.setGasPpm(Float.parseFloat(smoke.toString()));
                }
            }
            if (jsonObject.containsKey("gasDig")) {
                Object gasDig = jsonObject.get("gasDig");
                if (gasDig != null) {
                    environment.setGasStatus(Integer.parseInt(gasDig.toString()));
                }
            }
            if (jsonObject.containsKey("ldrDig")) {
                Object ldrDig = jsonObject.get("ldrDig");
                if (ldrDig != null) {
                    environment.setLightStatus(Integer.parseInt(ldrDig.toString()));
                }
            }
            if (jsonObject.containsKey("flameDig")) {
                Object flameDig = jsonObject.get("flameDig");
                if (flameDig != null) {
                    environment.setFlameStatus(Integer.parseInt(flameDig.toString()));
                }
            }
            if (jsonObject.containsKey("ldrPer")) {
                Object ldrPer = jsonObject.get("ldrPer");
                if (ldrPer != null) {
                    environment.setLightPercentage(Float.parseFloat(ldrPer.toString()));
                }
            }
            if (jsonObject.containsKey("flamePer")) {
                Object flamePer = jsonObject.get("flamePer");
                if (flamePer != null) {
                    environment.setFlamePercentage(Float.parseFloat(flamePer.toString()));
                }
            }
            if (jsonObject.containsKey("alarm")) {
                Object beep = jsonObject.get("alarm");
                if (beep != null) {
                    environment.setAlarmStatus(Integer.parseInt(beep.toString()));
                }
            }
            if (jsonObject.containsKey("fan")) {
                Object fan = jsonObject.get("fan");
                if (fan != null) {
                    environment.setFanStatus(Integer.parseInt(fan.toString()));
                }
            }
            if (jsonObject.containsKey("led")) {
                Object led = jsonObject.get("led");
                if (led != null) {
                    environment.setLedStatus(Integer.parseInt(led.toString()));
                }
            }
            // 设置默认值
            environment.setSource(1); // 来源：1上报
            environment.setGmtCreate(new Date());

            // 处理measureTime字段
            Object measureTimeObj = jsonObject.get("measureTime");

            if (measureTimeObj != null) {
                try {
                    // 支持多种日期格式
                    String timeStr = measureTimeObj.toString();
                    SimpleDateFormat sdf = new SimpleDateFormat("yyyy-MM-dd HH:mm:ss");
                    Date measureTime = sdf.parse(timeStr);
                    environment.setGmtMeasurement(measureTime);
                } catch (Exception e) {
                    log.warn("measureTime格式解析失败，使用当前时间: {}", e.getMessage());
                    environment.setGmtMeasurement(new Date());
                }
            } else {
                // 如果没有measureTime字段，使用当前时间
                environment.setGmtMeasurement(new Date());
            }
            // 插入到数据库
            String socketTopic = "/topic/environment/"+ deviceKeyStr;
            webSocketPushUtil.pushToTopic(socketTopic, environment);
            boolean saved = environmentService.save(environment);
        } catch (Exception e) {
            log.error("处理\"environment数据时发生异常: {}", e.getMessage(), e);
        }
    }
}