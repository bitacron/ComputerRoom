package com.example.room.control.entity.param;

import io.swagger.annotations.ApiModelProperty;
import lombok.Data;

import java.io.Serializable;

// 前端传递过来的数据封装到该类的对象中
@Data
public class DeviceOptionQuery implements Serializable {

    private static final long serialVersionUID = 1L;

    @ApiModelProperty(value = "查询开始时间", example = "2019-01-01 10:10:10")
    private String begin;//注意，这里使用的是String类型，前端传过来的数据无需进行类型转换

    @ApiModelProperty(value = "查询结束时间", example = "2019-12-01 10:10:10")
    private String end;

    @ApiModelProperty(value = "当前页", example = "1")
    private Integer currentPage;

    @ApiModelProperty(value = "每页记录数", example = "10")
    private Integer pageSize;
}
