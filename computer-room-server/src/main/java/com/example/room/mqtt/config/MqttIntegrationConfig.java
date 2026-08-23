package com.example.room.mqtt.config;

import com.example.room.mqtt.handler.MqttMessageProcessor;
import lombok.extern.slf4j.Slf4j;
import org.eclipse.paho.client.mqttv3.MqttConnectOptions;
import org.springframework.boot.autoconfigure.condition.ConditionalOnProperty;
import org.springframework.context.annotation.Bean;
import org.springframework.context.annotation.Configuration;
import org.springframework.util.StringUtils;
import org.springframework.integration.annotation.ServiceActivator;
import org.springframework.integration.channel.DirectChannel;
import org.springframework.integration.mqtt.core.DefaultMqttPahoClientFactory;
import org.springframework.integration.mqtt.core.MqttPahoClientFactory;
import org.springframework.integration.mqtt.inbound.MqttPahoMessageDrivenChannelAdapter;
import org.springframework.integration.mqtt.outbound.MqttPahoMessageHandler;
import org.springframework.integration.mqtt.support.DefaultPahoMessageConverter;
import org.springframework.messaging.MessageChannel;
import org.springframework.messaging.MessageHandler;

import javax.annotation.Resource;

@Slf4j
@Configuration
@ConditionalOnProperty(prefix = "mqtt", name = "enabled", havingValue = "true")
public class MqttIntegrationConfig {

    @Resource
    private MqttConfig mqttConfig;

    @Bean
    public MqttPahoClientFactory mqttClientFactory() {
        DefaultMqttPahoClientFactory factory = new DefaultMqttPahoClientFactory();
        MqttConnectOptions options = new MqttConnectOptions();
        options.setServerURIs(new String[]{mqttConfig.getBrokerUrl()});
        options.setKeepAliveInterval(60);
        options.setConnectionTimeout(30);
        options.setAutomaticReconnect(true);
        options.setCleanSession(true);

        if (StringUtils.hasText(mqttConfig.getUsername())) {
            options.setUserName(mqttConfig.getUsername());
            options.setPassword(mqttConfig.getPassword().toCharArray());
        }

        factory.setConnectionOptions(options);
        return factory;
    }

    @Bean
    public MessageChannel mqttInputChannel() {
        return new DirectChannel();
    }

    /**
     * 创建MQTT的输入适配器
     * @return adapter
     */
    @Bean
    public MqttPahoMessageDrivenChannelAdapter mqttInbound() {
        // 动态获取topic，支持多个topic
        String[] topics = mqttConfig.getReceiveTopic().split(",");
        String receiverClientId = mqttConfig.getClientId() + "_receiver_" + System.currentTimeMillis();

        MqttPahoMessageDrivenChannelAdapter adapter =
                new MqttPahoMessageDrivenChannelAdapter(
                        mqttConfig.getBrokerUrl(),
                        receiverClientId,
                        mqttClientFactory(),
                        topics
                );

        adapter.setConverter(new DefaultPahoMessageConverter());
        adapter.setQos(mqttConfig.getQos());
        adapter.setOutputChannel(mqttInputChannel());
        adapter.setRecoveryInterval(10000);
        log.info("MQTT接收适配器已创建，Client ID: {}，订阅: {}", receiverClientId, mqttConfig.getReceiveTopic());
        return adapter;
    }

    @ServiceActivator(inputChannel = "mqttInputChannel")
    @Bean
    public MessageHandler mqttMessageHandler(MqttMessageProcessor mqttMessageProcessor) {
        log.info("MqttMessageHandler mqttMessageHandler");
        return mqttMessageProcessor;
        // return message -> {
        //     String topic = message.getHeaders().get("mqtt_receivedTopic").toString();
        //     String strPayload = (String) message.getPayload();
        //     byte[] bytePayload = strPayload.getBytes(StandardCharsets.UTF_8);
        //     // ...处理byte数组
        //     String payload = new String((byte[]) bytePayload);
        //     System.out.printf("收到消息 -> [主题:%s] [内容:%s]%n", topic, payload.toString());
        //
        //     // 在这里添加业务处理逻辑
        // };
    }

    // ============ 新增：发送消息相关的配置 ============
    // 1. 创建输出通道（用于发送消息）
    @Bean
    public MessageChannel mqttOutputChannel() {
        return new DirectChannel();
    }

    // 2. 创建MQTT出站适配器（用于发送消息）
    @Bean
    @ServiceActivator(inputChannel = "mqttOutputChannel")
    public MessageHandler mqttOutbound() {
        // 使用不同的client ID用于发送（避免与接收的client ID冲突）
        String senderClientId = mqttConfig.getClientId() + "_sender_" + System.currentTimeMillis();

        MqttPahoMessageHandler messageHandler =
                new MqttPahoMessageHandler(senderClientId, mqttClientFactory());

        // 配置发送属性
        messageHandler.setAsync(true);  // 异步发送
        messageHandler.setDefaultQos(1);  // 默认QoS
        messageHandler.setDefaultRetained(false);  // 默认不保留消息

        System.out.println("MQTT发送处理器已创建，Client ID: " + senderClientId);
        return messageHandler;
    }

}