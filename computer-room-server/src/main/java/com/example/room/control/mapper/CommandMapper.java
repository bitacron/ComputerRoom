package com.example.room.control.mapper;

import com.baomidou.mybatisplus.core.mapper.BaseMapper;
import com.example.room.control.entity.Command;
import org.apache.ibatis.annotations.Mapper;

@Mapper
public interface CommandMapper extends BaseMapper<Command> {
    // 如需自定义SQL可在此添加方法
}