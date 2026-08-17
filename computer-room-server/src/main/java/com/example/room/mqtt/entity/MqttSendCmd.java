package com.example.room.mqtt.entity;

import com.baomidou.mybatisplus.annotation.*;
import com.fasterxml.jackson.databind.annotation.JsonSerialize;
import com.fasterxml.jackson.databind.ser.std.ToStringSerializer;
import io.swagger.annotations.ApiModel;
import io.swagger.annotations.ApiModelProperty;
import lombok.Data;
import lombok.EqualsAndHashCode;
import lombok.experimental.Accessors;

import java.io.Serializable;
import java.util.Date;

@Data
@EqualsAndHashCode(callSuper = false)
@Accessors(chain = true)
@ApiModel(value="MqttSendCmd", description="MQTT发送记录-指令")
public class MqttSendCmd implements Serializable {

    private static final long serialVersionUID = 1L;

    @ApiModelProperty(value = "主键ID")
    @TableId(value = "id", type = IdType.ASSIGN_ID)
    @JsonSerialize(using = ToStringSerializer.class)
    private Long id;
    
    @ApiModelProperty(value = "topic主题")
    private String topic;
    
    @ApiModelProperty(value = "payload有效载荷")
    private String payload;

    @ApiModelProperty(value = "发送到的设备id")
    private String deviceKey;

    @ApiModelProperty(value = "发送时间")
    @TableField(fill = FieldFill.INSERT)
    private Date sendTime;

    @ApiModelProperty(value = "逻辑删除 1（true）已删除， 0（false）未删除")
    @TableLogic
    private Boolean isDeleted;
}