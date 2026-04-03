package com.example.room.access.vo;

import lombok.AllArgsConstructor;
import lombok.Data;
import lombok.NoArgsConstructor;

import java.util.List;

/**
 * 树形子节点响应
 */
@Data
@NoArgsConstructor
@AllArgsConstructor
public class ChildrenVo<T> {

    private List<T> children;
}
