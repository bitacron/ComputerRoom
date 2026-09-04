package com.example.room.alarm.controller;

import com.baomidou.mybatisplus.extension.plugins.pagination.Page;
import com.example.room.alarm.entity.AlarmRecord;
import com.example.room.alarm.entity.dto.AlarmQuery;
import com.example.room.alarm.entity.vo.AlarmSummaryVO;
import com.example.room.alarm.service.AlarmRecordService;
import com.example.room.util.Result;
import io.swagger.annotations.Api;
import io.swagger.annotations.ApiOperation;
import org.springframework.security.access.prepost.PreAuthorize;
import org.springframework.security.core.context.SecurityContextHolder;
import org.springframework.web.bind.annotation.*;

import javax.annotation.Resource;

@Api(description = "报警记录管理")
@RestController
@RequestMapping("/service/alarm")
@CrossOrigin
public class AlarmRecordController {

    @Resource
    private AlarmRecordService alarmRecordService;

    @ApiOperation(value = "条件查询分页")
    @PreAuthorize("hasAuthority('alarm.list')")
    @PostMapping("page")
    public Result<Page<AlarmRecord>> page(@RequestBody(required = false) AlarmQuery query) {
        return Result.ok(alarmRecordService.pageQuery(query));
    }

    @ApiOperation(value = "报警统计")
    @PreAuthorize("hasAuthority('alarm.list')")
    @GetMapping("summary")
    public Result<AlarmSummaryVO> summary() {
        return Result.ok(alarmRecordService.summary());
    }

    @ApiOperation(value = "处理报警")
    @PreAuthorize("hasAuthority('alarm.handle')")
    @PostMapping("handle/{id}")
    public Result<String> handle(@PathVariable Long id) {
        String username = SecurityContextHolder.getContext().getAuthentication().getName();
        return alarmRecordService.handle(id, username) ? Result.ok() : Result.fail("处理失败，记录不存在或已处理");
    }

    @ApiOperation(value = "删除报警")
    @PreAuthorize("hasAuthority('alarm.remove')")
    @DeleteMapping("{id}")
    public Result<String> remove(@PathVariable Long id) {
        return alarmRecordService.removeById(id) ? Result.ok() : Result.fail();
    }
}
