package com.example.room.mqtt.handler;

import com.example.room.mqtt.common.MqttProcessMessageService;
import lombok.extern.slf4j.Slf4j;
import org.eclipse.paho.client.mqttv3.MqttMessage;
import org.springframework.integration.mqtt.support.MqttHeaders;
import org.springframework.messaging.Message;
import org.springframework.messaging.MessageHandler;
import org.springframework.messaging.MessagingException;
import org.springframework.stereotype.Component;
import org.springframework.transaction.annotation.Transactional;

import javax.annotation.Resource;
import java.nio.charset.StandardCharsets;

@Slf4j
@Component
public class MqttMessageProcessor implements MessageHandler {

    @Resource
    private MqttProcessMessageService mqttProcessMessageService;

    @Override
    @Transactional(rollbackFor = Exception.class)
    public void handleMessage(Message<?> message) throws MessagingException {
        try {
            // 获取topic
            String topic = message.getHeaders().get(MqttHeaders.RECEIVED_TOPIC, String.class);
            
            // 获取payload
            Object payload = message.getPayload();
            String payloadStr;
            
            if (payload instanceof byte[]) {
                payloadStr = new String((byte[]) payload, StandardCharsets.UTF_8);
            } else if (payload instanceof MqttMessage) {
                payloadStr = new String(((MqttMessage) payload).getPayload(), StandardCharsets.UTF_8);
            } else {
                payloadStr = payload.toString();
            }
            
            log.info("Received MQTT message - Topic: {}, Payload: {}", topic, payloadStr);
            
            // 处理消息
            mqttProcessMessageService.processMessage(topic, payloadStr);
            
        } catch (Exception e) {
            log.error("Process MQTT message error", e);
        }
    }


}