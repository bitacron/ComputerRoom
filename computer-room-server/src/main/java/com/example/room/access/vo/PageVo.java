package com.example.room.access.vo;

import lombok.AllArgsConstructor;
import lombok.Data;
import lombok.NoArgsConstructor;

import java.util.List;

/**
 * 分页列表响应
 */
@Data
@NoArgsConstructor
@AllArgsConstructor
public class PageVo<T> {

    private List<T> items;

    private Long total;
}
