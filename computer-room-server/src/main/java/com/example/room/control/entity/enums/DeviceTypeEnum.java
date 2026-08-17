package com.example.room.control.entity.enums;

import lombok.AllArgsConstructor;
import lombok.Getter;

/**
 * 设备指令枚举
 */
@Getter
@AllArgsConstructor
public enum DeviceTypeEnum {
    
    FAN("FAN", "风扇", 1),
    LED("LED", "LED灯", 0);
    
    private final String code;      // 指令代码：ON/OFF
    private final String name;     // 指令值：1/0
    private final int intValue;     // 整数值：1/0
    
    /**
     * 根据指令代码获取枚举
     * @param code 指令代码（ON/OFF）
     * @return 对应的枚举值，如果未找到则返回 null
     */
    public static DeviceTypeEnum fromCode(String code) {
        if (code == null) {
            return null;
        }
        for (DeviceTypeEnum command : values()) {
            if (command.getCode().equalsIgnoreCase(code)) {
                return command;
            }
        }
        return null;
    }
    
    /**
     * 根据指令值获取枚举
     * @param name 指令值（1/0）
     * @return 对应的枚举值，如果未找到则返回 null
     */
    public static DeviceTypeEnum fromName(String name) {
        if (name == null) {
            return null;
        }
        for (DeviceTypeEnum command : values()) {
            if (command.getName().equalsIgnoreCase(name)) {
                return command;
            }
        }
        return null;
    }
}
