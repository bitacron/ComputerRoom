package com.example.room.mqtt.controller;

import com.example.room.mqtt.common.MqttSendMessageService;
import org.springframework.web.bind.annotation.*;

import javax.annotation.Resource;
import java.util.HashMap;
import java.util.Map;

@RestController
@RequestMapping("/mqtt")
public class MqttController {

    @Resource
    private MqttSendMessageService mqttSendMessageService;

    /**
     * 发送MQTT消息
     * @param topic 主题
     * @param message 消息内容
     * @return 发送结果
     */
    @PostMapping("/send")
    public Map<String, Object> sendMessage(
            @RequestParam String topic,
            @RequestParam String message) {
        
        boolean success = mqttSendMessageService.sendMessage(topic, message);
        
        Map<String, Object> result = new HashMap<>();
        result.put("success", success);
        result.put("topic", topic);
        result.put("message", message);
        result.put("timestamp", System.currentTimeMillis());
        
        return result;
    }

    /**
     * 发送到设备
     * @param productKey 产品Key
     * @param deviceId 设备ID
     * @param message 消息内容
     * @return 发送结果
     */
    @PostMapping("/send/{productKey}/{deviceId}")
    public Map<String, Object> sendToDevice(
            @PathVariable String productKey,
            @PathVariable String deviceId,
            @RequestBody String message) {
        
        String topic = "/" + productKey + "/" + deviceId + "/get";
        boolean success = mqttSendMessageService.sendMessage(topic, message);
        
        Map<String, Object> result = new HashMap<>();
        result.put("success", success);
        result.put("topic", topic);
        result.put("message", message);
        
        return result;
    }
}