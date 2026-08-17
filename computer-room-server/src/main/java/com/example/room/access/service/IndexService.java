package com.example.room.access.service;

import com.alibaba.fastjson.JSONObject;
import com.example.room.access.vo.UserInfoVo;

import java.util.List;

public interface IndexService {

    UserInfoVo getUserInfo(String username);

    List<JSONObject> getMenu(String username);
}
