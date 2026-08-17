package com.example.room.common.config;

import org.springframework.context.annotation.Configuration;
import org.springframework.messaging.simp.config.MessageBrokerRegistry;
import org.springframework.web.socket.config.annotation.EnableWebSocketMessageBroker;
import org.springframework.web.socket.config.annotation.StompEndpointRegistry;
import org.springframework.web.socket.config.annotation.WebSocketMessageBrokerConfigurer;

@Configuration
@EnableWebSocketMessageBroker
public class WebSocketConfig implements WebSocketMessageBrokerConfigurer {

    // @Override
    // public void registerStompEndpoints(StompEndpointRegistry registry) {
    //     // 前端连接地址为 /ws/environment
    //     registry.addEndpoint("/ws/environment")
    //             .setAllowedOrigins("*")  // 生产环境应指定具体域名
    //             .withSockJS();            // 兼容不支持WebSocket的浏览器（可选）
    // }
    @Override
    public void registerStompEndpoints(StompEndpointRegistry registry) {
        registry.addEndpoint("/ws/environment")
                .setAllowedOriginPatterns("*")   // 使用 patterns 而不是 origins
                .withSockJS();

        // 测试用的原生 WebSocket 端点（仅用于 Postman 等工具）
        registry.addEndpoint("/ws/environment")
                .setAllowedOriginPatterns("*");   // 不加 withSockJS()
    }
    @Override
    public void configureMessageBroker(MessageBrokerRegistry registry) {
        // 启用简单消息代理，用于广播主题（以 /topic 开头）
        registry.enableSimpleBroker("/topic");
        // 应用前缀（客户端发送消息时使用 /app）
        registry.setApplicationDestinationPrefixes("/app");
    }
}