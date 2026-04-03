package com.example.room.control.entity.param;

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
 * 设备反控记录实体
 * </p>
 *
 * @author zhmy
 * @since 2025-03-28
 */
@Data
@EqualsAndHashCode(callSuper = false)
@Accessors(chain = true)
@ApiModel(value = "Command对象", description = "设备反控记录")
public class CommandQuery implements Serializable {

    private static final long serialVersionUID = 1L;

    @ApiModelProperty(value = "指令ID（业务唯一标识）")
    private String cmdId;

    @ApiModelProperty(value = "设备ID")
    private String deviceId;

    @ApiModelProperty(value = "控制类型：1-散热")
    private String deviceType;

    @ApiModelProperty(value = "命令：1-ON 2-OFF")
    private String command;

    @ApiModelProperty(value = "指令状态：0-待处理 1-已发送 2-执行成功 3-执行失败 4-超时")
    private Integer status;

    @ApiModelProperty(value = "查询开始时间", example = "2019-01-01 10:10:10")
    private String begin;//注意，这里使用的是String类型，前端传过来的数据无需进行类型转换

    @ApiModelProperty(value = "查询结束时间", example = "2019-12-01 10:10:10")
    private String end;

    @ApiModelProperty(value = "当前页", example = "1")
    private Integer currentPage;

    @ApiModelProperty(value = "每页记录数", example = "10")
    private Integer pageSize;
}