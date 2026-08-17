package com.example.room.access.vo;

import lombok.AllArgsConstructor;
import lombok.Data;
import lombok.NoArgsConstructor;

/**
 * 登录成功响应
 */
@Data
@NoArgsConstructor
@AllArgsConstructor
public class LoginVo {

    private String token;
}
