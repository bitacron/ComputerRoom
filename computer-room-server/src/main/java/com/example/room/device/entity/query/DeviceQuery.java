package com.example.room.device.entity.query;

import io.swagger.annotations.ApiModelProperty;
import lombok.Data;

import java.io.Serializable;

// 前端传递过来的数据封装到该类的对象中
@Data
public class DeviceQuery implements Serializable {

    private static final long serialVersionUID = 1L;

    @ApiModelProperty(value = "在线状态：0-离线，1-在线（实时状态）")
    private Integer onlineStatus;

    @ApiModelProperty(value = "设备标识")
    private String deviceKey;

    @ApiModelProperty(value = "设备名称")
    private String deviceName;

    @ApiModelProperty(value = "当前页", example = "1")
    private Integer currentPage;

    @ApiModelProperty(value = "每页记录数", example = "10")
    private Integer pageSize;
}
