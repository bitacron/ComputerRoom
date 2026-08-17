package com.example.room.mqtt.config;

import lombok.Data;
import org.springframework.beans.factory.annotation.Value;
import org.springframework.stereotype.Component;

@Data
@Component
// @ConfigurationProperties(prefix = "mqtt")
public class MqttConfig {

    @Value("${mqtt.broker-url}")
    private String brokerUrl;

    @Value("${mqtt.client-id}")
    private String clientId;

    @Value("${mqtt.receive-topic}")
    private String receiveTopic;

    @Value("${mqtt.username}")
    private String username;

    @Value("${mqtt.password}")
    private String password;

    @Value("${mqtt.completion-timeout}")
    private Integer completionTimeout = 5000;

    @Value("${mqtt.qos}")
    private Integer qos = 1;

    @Value("${mqtt.enabled:true}")
    private boolean enabled;

}