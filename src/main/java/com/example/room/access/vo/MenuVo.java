package com.example.room.access.vo;

import com.alibaba.fastjson.JSONObject;
import lombok.AllArgsConstructor;
import lombok.Data;
import lombok.NoArgsConstructor;

import java.util.List;

/**
 * 动态菜单响应
 */
@Data
@NoArgsConstructor
@AllArgsConstructor
public class MenuVo {

    private List<JSONObject> permissionList;
}
