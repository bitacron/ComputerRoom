package com.example.room.alarm.entity;

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
@TableName("alarm_record")
@ApiModel(value = "AlarmRecord对象", description = "报警记录")
public class AlarmRecord implements Serializable {

    private static final long serialVersionUID = 1L;

    @ApiModelProperty(value = "报警记录ID")
    @TableId(value = "id", type = IdType.ASSIGN_ID)
    @JsonSerialize(using = ToStringSerializer.class)
    private Long id;

    @ApiModelProperty(value = "设备标识")
    private String deviceKey;

    @ApiModelProperty(value = "报警类型：TEMP/HUMI/GAS/FLAME")
    private String alarmType;

    @ApiModelProperty(value = "等级：1预警 2紧急")
    private Integer alarmLevel;

    @ApiModelProperty(value = "报警内容")
    private String content;

    @ApiModelProperty(value = "状态：0报警中 1已恢复 2已处理")
    private Integer status;

    @ApiModelProperty(value = "处理人")
    private String handler;

    @ApiModelProperty(value = "处理时间")
    private Date handleTime;

    @ApiModelProperty(value = "恢复时间")
    private Date recoverTime;

    @ApiModelProperty(value = "逻辑删除 1（true）已删除， 0（false）未删除")
    @TableLogic
    private Boolean isDeleted;

    @ApiModelProperty(value = "报警开始时间")
    @TableField(fill = FieldFill.INSERT)
    private Date gmtCreate;
}
