package com.example.room.environment.entity;

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

/**
 * <p>
 * 环境检测数据
 * </p>
 *
 * @author zhmy
 * @since 2023-05-31
 */
@Data
@EqualsAndHashCode(callSuper = false)
@Accessors(chain = true)
@ApiModel(value="Environment对象", description="环境检测数据")
public class Environment implements Serializable {

    private static final long serialVersionUID = 1L;

    @ApiModelProperty(value = "环境监测数据ID")
    @TableId(value = "id", type = IdType.ASSIGN_ID)
    @JsonSerialize(using = ToStringSerializer.class)
    private Long id;

    @ApiModelProperty(value = "数据来源（1：硬件上报；0：手动记录）")
    private Integer source;

    @ApiModelProperty(value = "上报设备")
    private String deviceKey;

    @ApiModelProperty(value = "温度")
    private float temperature;

    @ApiModelProperty(value = "湿度")
    private float humidity;

    @ApiModelProperty(value = "烟雾浓度")
    private float gasPpm;

    @ApiModelProperty(value = "MQ2烟雾AO状态（1：有烟雾；0：无烟雾）")
    private Integer gasStatus;

    @ApiModelProperty(value = "光敏电阻AO状态（1：无光；0：有光）")
    private Integer lightStatus;

    @ApiModelProperty(value = "火焰传感器AO状态（1：有火；0：无火）")
    private Integer flameStatus;

    @ApiModelProperty(value = "光照强度（0-100）")
    private float lightPercentage;

    @ApiModelProperty(value = "附近有火焰的百分比（0-100）")
    private float flamePercentage;

    @ApiModelProperty(value = "蜂鸣器报警状态（1：报警；0：未报警）")
    private Integer alarmStatus;

    @ApiModelProperty(value = "散热设备开关（1：开启；0：关闭）")
    private Integer fanStatus;

    @ApiModelProperty(value = "led灯开关（1：开启；0：关闭）")
    private Integer ledStatus;

    @ApiModelProperty(value = "逻辑删除 1（true）已删除， 0（false）未删除")
    @TableLogic
    private Boolean isDeleted;

    @ApiModelProperty(value = "创建时间")
    @TableField(fill = FieldFill.INSERT)
    private Date gmtCreate;

    @ApiModelProperty(value = "测量时间")
    private Date gmtMeasurement;

}
