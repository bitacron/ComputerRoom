package com.example.room.util;

import org.springframework.stereotype.Component;

import java.util.concurrent.atomic.AtomicInteger;

@Component
public class RequestIdGenerator {
    private final AtomicInteger counter = new AtomicInteger(0);

    public String nextId() {
        int val = counter.incrementAndGet() & 0xFFFF; // 保证在 0~65535 之间
        return String.format("%04X", val); // 输出大写十六进制，如 "00A3"
    }

}
