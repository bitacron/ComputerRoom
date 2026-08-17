package com.example.room.control.entity.vo;

import com.baomidou.mybatisplus.annotation.*;
import io.swagger.annotations.ApiModelProperty;
import lombok.Data;

import java.io.Serializable;
import java.util.Date;

// 前端传递过来的数据封装到该类的对象中
@Data
public class DeviceOptionVo implements Serializable {

    private static final long serialVersionUID = 1L;

    @ApiModelProperty(value = "数据来源（1：硬件上报；0：手动记录）")
    private String deviceKey;

    @ApiModelProperty(value = "操作")
    private String action;

    @ApiModelProperty(value = "操作人")
    private String operator;

    @ApiModelProperty(value = "操作人姓名")
    private String operatorName;

    @ApiModelProperty(value = "逻辑删除 1（true）已删除， 0（false）未删除")
    @TableLogic
    private Boolean isDeleted;

    @ApiModelProperty(value = "创建时间")
    @TableField(fill = FieldFill.INSERT)
    private Date gmtCreate;

}
