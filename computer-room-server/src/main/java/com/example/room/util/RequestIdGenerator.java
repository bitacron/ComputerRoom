package com.example.room.util;

import cn.hutool.core.util.IdUtil;
import org.springframework.stereotype.Component;

/**
 * 反控指令 cmdId。雪花 ID，重启与绕回都不会和历史指令撞号。
 */
@Component
public class RequestIdGenerator {

    public String nextId() {
        return IdUtil.getSnowflakeNextIdStr();
    }
}
