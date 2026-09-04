package com.example.room.alarm.entity.dto;

import io.swagger.annotations.ApiModelProperty;
import lombok.Data;

import java.io.Serializable;

@Data
public class AlarmQuery implements Serializable {

    private static final long serialVersionUID = 1L;

    @ApiModelProperty(value = "设备标识")
    private String deviceKey;

    @ApiModelProperty(value = "报警类型：TEMP/HUMI/GAS/FLAME")
    private String alarmType;

    @ApiModelProperty(value = "等级：1预警 2紧急")
    private Integer alarmLevel;

    @ApiModelProperty(value = "状态：0报警中 1已恢复 2已处理")
    private Integer status;

    @ApiModelProperty(value = "查询开始时间", example = "2019-01-01 10:10:10")
    private String begin;

    @ApiModelProperty(value = "查询结束时间", example = "2019-12-01 10:10:10")
    private String end;

    @ApiModelProperty(value = "当前页", example = "1")
    private Integer currentPage;

    @ApiModelProperty(value = "每页记录数", example = "10")
    private Integer pageSize;
}
