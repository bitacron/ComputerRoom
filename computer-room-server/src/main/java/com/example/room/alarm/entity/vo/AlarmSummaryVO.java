package com.example.room.alarm.entity.vo;

import io.swagger.annotations.ApiModelProperty;
import lombok.Data;

import java.io.Serializable;

@Data
public class AlarmSummaryVO implements Serializable {

    private static final long serialVersionUID = 1L;

    @ApiModelProperty(value = "报警中数量（Level 1 + 2）")
    private Integer alarming;

    @ApiModelProperty(value = "报警中的紧急数量（仅 Level 2）")
    private Integer urgent;

    @ApiModelProperty(value = "今日报警数量")
    private Integer today;

    @ApiModelProperty(value = "已处理数量")
    private Integer handled;
}
