package com.example.room.control.entity.vo;

import io.swagger.annotations.ApiModel;
import io.swagger.annotations.ApiModelProperty;
import lombok.Data;

import java.io.Serializable;

@Data
@ApiModel(value = "ControlResult", description = "反控下发结果")
public class ControlResult implements Serializable {

    private static final long serialVersionUID = 1L;

    @ApiModelProperty(value = "指令ID")
    private String cmdId;

    @ApiModelProperty(value = "指令状态：1-已发送")
    private Integer status;

    public static ControlResult sent(String cmdId) {
        ControlResult result = new ControlResult();
        result.setCmdId(cmdId);
        result.setStatus(1);
        return result;
    }
}
