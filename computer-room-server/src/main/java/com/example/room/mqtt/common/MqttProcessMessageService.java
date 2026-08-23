package com.example.room.mqtt.common;

import com.alibaba.fastjson.JSON;
import com.alibaba.fastjson.JSONObject;
import com.example.room.device.entity.Device;
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
import java.text.ParseException;
import java.text.SimpleDateFormat;
import java.util.Date;
import java.util.TimeZone;

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
            // 2. 解析并处理环境数据（历史入库 + 刷新 device 当前态）
            processEnvironmentData(payload);
        } else if (topic.contains("/ack") || topic.contains("resp")) {
            log.info("收到指令 ACK topic={}, payload={}", topic, payload);
            // 1. 原始JSON直接落库
            MqttReceiveCmdResp dataEntity = new MqttReceiveCmdResp();
            dataEntity.setTopic(topic);
            dataEntity.setPayload(payload);
            dataEntity.setReceiveTime(new Date());
            dataEntity.setDeviceKey(deviceKeyStr);
            mqttReceiveCmdRespService.save(dataEntity);
            deviceOptionService.onMqttMessage(topic, payload);
        } else if (topic.contains("status") || topic.contains("heartbeat")) {
            // status: 上线声明 / Last Will 离线；heartbeat 兼容旧固件
            Integer online = jsonObject.getInteger("online");
            if (online != null && online == 0) {
                deviceService.offlineDevice(deviceKeyStr);
            } else {
                String name = jsonObject.getString("name");
                String product = jsonObject.getString("product");
                deviceService.onlineDevice(deviceKeyStr, name, product);
            }
        }
    }

    private void processEnvironmentData(String payload) {
        try {
            // 解析JSON字符串
            JSONObject jsonObject = JSON.parseObject(payload);

            /*
            固件 report 示例（B方案：ESP8266 SNTP 校时后带 measureTime）：
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
              "led": 0,
              "measureTime": "2026-08-22 17:05:01"
            }
            无 measureTime 时 gmtMeasurement 使用服务端接收时刻。
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

            // 处理 measureTime：优先用设备侧墙钟（东八区），否则用服务端接收时间
            environment.setGmtMeasurement(parseMeasureTime(jsonObject.get("measureTime")));
            environmentService.save(environment);
            Device device = deviceService.applyReportSnapshot(deviceKeyStr, environment);
            Environment snapshot = deviceService.toRealtimeEnvironment(device);
            String socketTopic = "/topic/environment/"+ deviceKeyStr;
            webSocketPushUtil.pushToTopic(socketTopic, snapshot != null ? snapshot : environment);
        } catch (Exception e) {
            log.error("处理environment数据时发生异常: {}", e.getMessage(), e);
            if (e instanceof RuntimeException) {
                throw (RuntimeException) e;
            }
            throw new RuntimeException(e);
        }
    }

    /**
     * 解析设备上报的 measureTime。
     * 支持：yyyy-MM-dd HH:mm:ss、yyyy-MM-dd'T'HH:mm:ss、秒/毫秒时间戳；
     * 字符串按 GMT+8 解释，与固件 SNTP 东八区一致。
     */
    private Date parseMeasureTime(Object measureTimeObj) {
        Date now = new Date();
        if (measureTimeObj == null) {
            return now;
        }
        try {
            if (measureTimeObj instanceof Number) {
                long v = ((Number) measureTimeObj).longValue();
                // 10 位按秒，13 位按毫秒
                if (v > 0 && v < 100_000_000_000L) {
                    v *= 1000L;
                }
                return new Date(v);
            }
            String timeStr = measureTimeObj.toString().trim();
            // 固件校时失败占位：空串或 0000-00-00 00:00:00 → 用服务端接收时间
            if (timeStr.isEmpty()
                    || "0000-00-00 00:00:00".equals(timeStr)
                    || "0000-00-00T00:00:00".equals(timeStr)) {
                return now;
            }
            // 纯数字时间戳
            if (timeStr.matches("^\\d{10,13}$")) {
                long v = Long.parseLong(timeStr);
                if (timeStr.length() <= 10) {
                    v *= 1000L;
                }
                return new Date(v);
            }
            String[] patterns = {
                    "yyyy-MM-dd HH:mm:ss",
                    "yyyy-MM-dd'T'HH:mm:ss",
                    "yyyy/MM/dd HH:mm:ss"
            };
            TimeZone tz = TimeZone.getTimeZone("GMT+8");
            for (String pattern : patterns) {
                try {
                    SimpleDateFormat sdf = new SimpleDateFormat(pattern);
                    sdf.setTimeZone(tz);
                    sdf.setLenient(false);
                    return sdf.parse(timeStr);
                } catch (ParseException ignore) {
                    // try next
                }
            }
            log.warn("measureTime格式无法识别: {}, 使用当前时间", timeStr);
        } catch (Exception e) {
            log.warn("measureTime解析失败，使用当前时间: {}", e.getMessage());
        }
        return now;
    }
}