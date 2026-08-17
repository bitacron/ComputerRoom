package com.example.room.control.entity.param;

import io.swagger.annotations.ApiModelProperty;
import lombok.Data;

import java.io.Serializable;

// 前端传递过来的数据封装到该类的对象中
@Data
public class DeviceOptionControl implements Serializable {

    private static final long serialVersionUID = 1L;

    @ApiModelProperty(value = "设备id")
    private String deviceKey;//注意，这里使用的是String类型，前端传过来的数据无需进行类型转换

    @ApiModelProperty(value = "设备指令-ON、OFF")
    private String command;

    @ApiModelProperty(value = "设备类型")
    private String deviceType;

}
