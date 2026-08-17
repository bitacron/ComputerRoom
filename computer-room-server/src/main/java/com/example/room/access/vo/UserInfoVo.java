package com.example.room.access.vo;

import com.fasterxml.jackson.databind.annotation.JsonSerialize;
import com.fasterxml.jackson.databind.ser.std.ToStringSerializer;
import lombok.Data;

import java.util.List;

/**
 * 当前登录用户信息
 */
@Data
public class UserInfoVo {

    @JsonSerialize(using = ToStringSerializer.class)
    private Long id;

    private String name;

    private String code;

    private String avatar;

    private String phone;

    private String email;

    private String nickName;

    private List<String> roles;

    private List<String> permissionValueList;
}
