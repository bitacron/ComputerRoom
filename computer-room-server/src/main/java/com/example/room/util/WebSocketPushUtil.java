package com.example.room.util;

import lombok.extern.slf4j.Slf4j;
import org.springframework.beans.factory.annotation.Autowired;
import org.springframework.messaging.simp.SimpMessagingTemplate;
import org.springframework.stereotype.Component;

@Slf4j
@Component
public class WebSocketPushUtil {

    @Autowired
    private SimpMessagingTemplate messagingTemplate;

    /**
     * 向指定主题推送消息
     * @param topic   主题（如 "/topic/environment"）
     * @param payload 消息对象（会被自动转为 JSON）
     */
    public void pushToTopic(String topic, Object payload) {
        log.info("推送消息到主题: {}, 消息: {}", topic, payload);
        messagingTemplate.convertAndSend(topic, payload);
    }

    /**
     * 向指定用户推送消息（需配合用户认证）
     * @param user     用户标识
     * @param dest     目标地址
     * @param payload  消息对象
     */
    public void pushToUser(String user, String dest, Object payload) {
        messagingTemplate.convertAndSendToUser(user, dest, payload);
    }
}