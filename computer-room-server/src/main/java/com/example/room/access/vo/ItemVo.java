package com.example.room.access.vo;

import lombok.AllArgsConstructor;
import lombok.Data;
import lombok.NoArgsConstructor;

/**
 * 单条记录响应
 */
@Data
@NoArgsConstructor
@AllArgsConstructor
public class ItemVo<T> {

    private T item;
}
