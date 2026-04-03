package com.example.room.mqtt.common;

import com.alibaba.fastjson.JSON;
import com.alibaba.fastjson.JSONObject;
import com.example.room.mqtt.entity.MqttSendCmd;
import com.example.room.mqtt.service.MqttSendCmdService;
import lombok.extern.slf4j.Slf4j;
import org.springframework.beans.factory.annotation.Autowired;
import org.springframework.beans.factory.annotation.Qualifier;
import org.springframework.integration.support.MessageBuilder;
import org.springframework.messaging.Message;
import org.springframework.messaging.MessageChannel;
import org.springframework.stereotype.Service;

import javax.annotation.Resource;
import java.util.Date;

@Slf4j
@Service
public class MqttSendMessageService {

    @Autowired(required = false)
    @Qualifier("mqttOutputChannel")
    private MessageChannel mqttOutputChannel;

    @Resource
    private MqttSendCmdService mqttSendCmdService;

    /**
     * 发送字符串消息到指定主题
     * @param topic 主题
     * @param payload 消息内容
     * @return 是否发送成功
     */
    public boolean sendMessage(String topic, String payload) {
        if (mqttOutputChannel == null) {
            log.warn("MQTT 未启用，消息未发送 -> [主题:{}] [内容:{}]", topic, payload);
            return false;
        }
        try {
            Message<String> message = MessageBuilder
                    .withPayload(payload)
                    .setHeader("mqtt_topic", topic)
                    .setHeader("mqtt_qos", 0)
                    .build();
            
            boolean sent = mqttOutputChannel.send(message);
            if (sent) {
                System.out.printf("消息发送成功 -> [主题:%s] [内容:%s]%n", topic, payload);
                MqttSendCmd mqttSendCmd = new MqttSendCmd();
                mqttSendCmd.setTopic(topic);
                mqttSendCmd.setPayload(payload);
                JSONObject jsonObject = JSON.parseObject(payload);
                String deviceKeyStr = "";
                if (jsonObject.containsKey("dev")) {
                    Object deviceKey = jsonObject.get("dev");
                    deviceKeyStr = deviceKey.toString();
                }
                mqttSendCmd.setDeviceKey(deviceKeyStr);
                mqttSendCmd.setSendTime(new Date());
                mqttSendCmdService.save(mqttSendCmd);
            } else {
                System.err.printf("消息发送失败 -> [主题:%s] [内容:%s]%n", topic, payload);
            }
            return sent;
        } catch (Exception e) {
            System.err.printf("消息发送异常 -> [主题:%s] [内容:%s] 错误: %s%n", topic, payload, e.getMessage());
            return false;
        }
    }

    /**
     * 发送字节数组消息到指定主题
     * @param topic 主题
     * @param payload 字节数组消息
     * @return 是否发送成功
     */
    public boolean sendMessage(String topic, byte[] payload) {
        if (mqttOutputChannel == null) {
            log.warn("MQTT 未启用，二进制消息未发送 -> [主题:{}]", topic);
            return false;
        }
        try {
            Message<byte[]> message = MessageBuilder
                    .withPayload(payload)
                    .setHeader("mqtt_topic", topic)
                    .setHeader("mqtt_qos", 1)
                    .build();
            
            boolean sent = mqttOutputChannel.send(message);
            if (sent) {
                System.out.printf("二进制消息发送成功 -> [主题:%s] [长度:%d]%n", 
                    topic, payload.length);
            }
            return sent;
        } catch (Exception e) {
            System.err.printf("二进制消息发送异常 -> [主题:%s] 错误: %s%n", topic, e.getMessage());
            return false;
        }
    }

    /**
     * 发送带QoS的消息
     * @param topic 主题
     * @param payload 消息内容
     * @param qos QoS级别 (0,1,2)
     * @param retained 是否保留消息
     * @return 是否发送成功
     */
    public boolean sendMessage(String topic, String payload, int qos, boolean retained) {
        if (mqttOutputChannel == null) {
            log.warn("MQTT 未启用，消息未发送 -> [主题:{}] [内容:{}]", topic, payload);
            return false;
        }
        try {
            Message<String> message = MessageBuilder
                    .withPayload(payload)
                    .setHeader("mqtt_topic", topic)
                    .setHeader("mqtt_qos", qos)
                    .setHeader("mqtt_retained", retained)
                    .build();

            return mqttOutputChannel.send(message);
        } catch (Exception e) {
            e.printStackTrace();
            return false;
        }
    }
}