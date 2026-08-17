package com.example.room.control.service.impl;

import com.baomidou.mybatisplus.core.conditions.query.QueryWrapper;
import com.baomidou.mybatisplus.extension.plugins.pagination.Page;
import com.baomidou.mybatisplus.extension.service.impl.ServiceImpl;
import com.example.room.control.entity.Command;
import com.example.room.control.entity.DeviceOption;
import com.example.room.control.entity.param.CommandQuery;
import com.example.room.control.mapper.CommandMapper;
import com.example.room.control.service.CommandService;
import org.apache.commons.lang3.StringUtils;
import org.springframework.stereotype.Service;

@Service
public class CommandServiceImpl extends ServiceImpl<CommandMapper, Command> implements CommandService {
    @Override
    public Page<Command> pageQuery(CommandQuery commandQuery) {
        Page<Command> page = new Page<>(commandQuery.getCurrentPage(), commandQuery.getPageSize());
        QueryWrapper<Command> queryWrapper = new QueryWrapper<>();
        if (!StringUtils.isEmpty(commandQuery.getBegin())) {
            queryWrapper.lambda().ge(Command::getGmtCreate, commandQuery.getBegin()); // ge大于
        }
        if (!StringUtils.isEmpty(commandQuery.getEnd())) {
            queryWrapper.lambda().le(Command::getGmtCreate, commandQuery.getEnd()); // le小于
        }
        queryWrapper.lambda().orderByDesc(Command::getGmtCreate);
        Page<Command> commandPage = baseMapper.selectPage(page, queryWrapper);
        return commandPage;
    }
    // 如果需要自定义业务逻辑，在此实现
}