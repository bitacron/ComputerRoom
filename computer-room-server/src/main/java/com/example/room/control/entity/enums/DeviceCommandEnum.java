package com.example.room.control.entity.enums;

import lombok.AllArgsConstructor;
import lombok.Getter;

/**
 * 设备指令枚举
 */
@Getter
@AllArgsConstructor
public enum DeviceCommandEnum {
    
    ON("ON", "开启", 1),
    OFF("OFF", "关闭", 0);
    
    private final String code;      // 指令代码：ON/OFF
    private final String value;     // 指令值：开启/关闭
    private final int intValue;     // 整数值：1/0
    
    /**
     * 根据指令代码获取枚举
     * @param code 指令代码（ON/OFF）
     * @return 对应的枚举值，如果未找到则返回 null
     */
    public static DeviceCommandEnum fromCode(String code) {
        if (code == null) {
            return null;
        }
        for (DeviceCommandEnum command : values()) {
            if (command.getCode().equalsIgnoreCase(code)) {
                return command;
            }
        }
        return null;
    }
    
    /**
     * 根据指令值获取枚举
     * @param value 指令值（1/0）
     * @return 对应的枚举值，如果未找到则返回 null
     */
    public static DeviceCommandEnum fromValue(String value) {
        if (value == null) {
            return null;
        }
        for (DeviceCommandEnum command : values()) {
            if (command.getValue().equals(value)) {
                return command;
            }
        }
        return null;
    }
}
