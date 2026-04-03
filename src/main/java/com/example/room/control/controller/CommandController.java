package com.example.room.control.controller;

import com.baomidou.mybatisplus.extension.plugins.pagination.Page;
import com.example.room.control.entity.Command;
import com.example.room.control.entity.param.CommandQuery;
import com.example.room.control.service.CommandService;
import com.example.room.util.Result;
import io.swagger.annotations.ApiOperation;
import io.swagger.annotations.ApiParam;
import org.springframework.security.access.prepost.PreAuthorize;
import org.springframework.web.bind.annotation.*;

import javax.annotation.Resource;
import java.util.List;

@RestController
@RequestMapping("/service/command")
public class CommandController {

    @Resource
    private CommandService commandService;

    @ApiOperation(value = "所有指令列表")
    @PreAuthorize("hasAuthority('command.list')")
    @GetMapping("findAll")
    public Result<List<Command>> findAll() {
        return Result.ok(commandService.list());
    }

    @ApiOperation(value = "逻辑删除")
    @PreAuthorize("hasAuthority('command.remove')")
    @DeleteMapping("{id}")
    public Result<String> removeCommand(@PathVariable String id) {
        return commandService.removeById(id) ? Result.ok() : Result.fail();
    }

    @ApiOperation(value = "条件查询分页方法")
    @PreAuthorize("hasAuthority('command.list')")
    @PostMapping("pageCommandCondition")
    public Result<Page<Command>> pageCommandCondition(@RequestBody CommandQuery commandQuery) {
        return Result.ok(commandService.pageQuery(commandQuery));
    }

    @ApiOperation("根据ID查询指令")
    @PreAuthorize("hasAuthority('command.list')")
    @GetMapping("getCommand/{id}")
    public Result<Command> getCommand(@PathVariable String id) {
        return Result.ok(commandService.getById(id));
    }

    @ApiOperation("修改指令")
    @PreAuthorize("hasAuthority('command.list')")
    @PostMapping("updateCommand")
    public Result<String> updateCommand(@RequestBody Command command) {
        return commandService.updateById(command) ? Result.ok() : Result.fail("修改失败");
    }
}
