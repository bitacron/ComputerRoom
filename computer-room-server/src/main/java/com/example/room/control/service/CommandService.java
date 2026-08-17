package com.example.room.control.service;

import com.baomidou.mybatisplus.extension.plugins.pagination.Page;
import com.baomidou.mybatisplus.extension.service.IService;
import com.example.room.control.entity.Command;
import com.example.room.control.entity.param.CommandQuery;

public interface CommandService extends IService<Command> {
    Page<Command> pageQuery(CommandQuery commandQuery);
    // 可添加业务方法，如根据cmdId查询、批量更新状态等
}