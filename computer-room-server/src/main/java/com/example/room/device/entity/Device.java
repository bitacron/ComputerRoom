package com.example.room.device.entity;

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
@ApiModel(value="Device 对象", description="设备信息")
@TableName("device")
public class Device implements Serializable {

    private static final long serialVersionUID = 1L;

    @ApiModelProperty(value = "主键 ID")
    @TableId(value = "id", type = IdType.AUTO)
    @JsonSerialize(using = ToStringSerializer.class)
    private Long id;

    @ApiModelProperty(value = "产品标识（冗余字段，方便查询）")
    private String productKey;

    @ApiModelProperty(value = "设备唯一标识（用户自定义或系统生成）")
    private String deviceKey;

    @ApiModelProperty(value = "设备名称（用户自定义）")
    private String deviceName;

    @ApiModelProperty(value = "在线状态：0-离线，1-在线（实时状态）")
    private Integer onlineStatus;

    @ApiModelProperty(value = "最后上线时间")
    private Date lastOnlineTime;

    @ApiModelProperty(value = "最后离线时间")
    private Date lastOfflineTime;

    @ApiModelProperty(value = "最后活跃时间（上报数据时间）")
    private Date lastActiveTime;

    @ApiModelProperty(value = "当前温度")
    private Float temperature;

    @ApiModelProperty(value = "当前湿度")
    private Float humidity;

    @ApiModelProperty(value = "当前烟雾浓度")
    private Float gasPpm;

    @ApiModelProperty(value = "当前烟雾状态（1：有烟雾；0：无烟雾）")
    private Integer gasStatus;

    @ApiModelProperty(value = "当前光照状态（1：无光；0：有光）")
    private Integer lightStatus;

    @ApiModelProperty(value = "当前火焰状态（1：有火；0：无火）")
    private Integer flameStatus;

    @ApiModelProperty(value = "当前光照强度（0-100）")
    private Float lightPercentage;

    @ApiModelProperty(value = "当前火焰百分比（0-100）")
    private Float flamePercentage;

    @ApiModelProperty(value = "当前蜂鸣器报警（1：报警；0：未报警）")
    private Integer alarmStatus;

    @ApiModelProperty(value = "当前散热设备开关（1：开启；0：关闭）")
    private Integer fanStatus;

    @ApiModelProperty(value = "当前 LED 开关（1：开启；0：关闭）")
    private Integer ledStatus;

    @ApiModelProperty(value = "影子对应的最近一次测量时间")
    private Date lastReportTime;

    @ApiModelProperty(value = "最近一次执行器（风扇/LED）变更时间")
    private Date lastActuatorTime;

}