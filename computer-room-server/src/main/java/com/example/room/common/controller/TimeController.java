package com.example.room.common.controller;

import org.springframework.web.bind.annotation.GetMapping;
import org.springframework.web.bind.annotation.RequestMapping;
import org.springframework.web.bind.annotation.RestController;

import java.text.SimpleDateFormat;
import java.util.Date;
import java.util.HashMap;
import java.util.Map;
import java.util.TimeZone;

/**
 * 设备校时接口：固件在无 SNTP 时通过局域网 HTTP 拉取东八区时间。
 * 注意：需加入 Spring Security 白名单（匿名可访问）。
 */
@RestController
@RequestMapping("/api")
public class TimeController {

    /**
     * GET /api/time
     * 返回示例：{"measureTime":"2026-08-22 17:55:01","epochMs":1724315701000}
     */
    @GetMapping("/time")
    public Map<String, Object> now() {
        Date now = new Date();
        SimpleDateFormat sdf = new SimpleDateFormat("yyyy-MM-dd HH:mm:ss");
        sdf.setTimeZone(TimeZone.getTimeZone("GMT+8"));

        Map<String, Object> result = new HashMap<>(4);
        result.put("measureTime", sdf.format(now));
        result.put("epochMs", now.getTime());
        return result;
    }
}
