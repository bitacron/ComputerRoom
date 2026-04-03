package com.example.room.access.controller;

import com.alibaba.fastjson.JSONObject;
import com.example.room.access.service.IndexService;
import com.example.room.access.vo.MenuVo;
import com.example.room.access.vo.UserInfoVo;
import com.example.room.util.Result;
import org.springframework.beans.factory.annotation.Autowired;
import org.springframework.security.core.context.SecurityContextHolder;
import org.springframework.web.bind.annotation.*;

import java.util.List;

@RestController
@RequestMapping("/admin/access/index")
@CrossOrigin
public class IndexController {

    @Autowired
    private IndexService indexService;

    @GetMapping("info")
    public Result<UserInfoVo> info() {
        String username = SecurityContextHolder.getContext().getAuthentication().getName();
        return Result.ok(indexService.getUserInfo(username));
    }

    @GetMapping("menu")
    public Result<MenuVo> getMenu() {
        String username = SecurityContextHolder.getContext().getAuthentication().getName();
        List<JSONObject> permissionList = indexService.getMenu(username);
        return Result.ok(new MenuVo(permissionList));
    }

    @PostMapping("logout")
    public Result<Void> logout() {
        return Result.ok();
    }
}
