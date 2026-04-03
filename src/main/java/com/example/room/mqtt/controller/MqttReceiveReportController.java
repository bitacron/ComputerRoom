package com.example.room.mqtt.controller;

import com.baomidou.mybatisplus.extension.plugins.pagination.Page;
import com.example.room.mqtt.entity.MqttReceiveReport;
import com.example.room.mqtt.entity.dto.MqttReceiveReportQuery;
import com.example.room.mqtt.service.MqttReceiveReportService;
import com.example.room.util.Result;
import io.swagger.annotations.Api;
import io.swagger.annotations.ApiOperation;
import io.swagger.annotations.ApiParam;
import org.springframework.security.access.prepost.PreAuthorize;
import org.springframework.web.bind.annotation.*;

import javax.annotation.Resource;
import java.util.Date;
import java.util.List;

@Api("MQTT接收消息管理")
@RestController
@RequestMapping("/mqtt/receive/report")
public class MqttReceiveReportController {
    // 把service注入
    @Resource
    private MqttReceiveReportService receiveReportService;

    //1. 查询MQTT发送消息所有数据
    // restful风格
    // 访问地址  http://localhost:8001/mqtt/receive/findAll
    @ApiOperation(value = "所有MQTT接收消息列表")
    @PreAuthorize("hasAuthority('receiveReport.list')")
    @GetMapping("findAll")
    public Result<List<MqttReceiveReport>> findAllReport() {
        // 调用service的方法实现查询所有的操作
        List<MqttReceiveReport> list = receiveReportService.list();
        return Result.ok(list);
    }

    // 2.逻辑删除MQTT接收上报方法
    @ApiOperation(value = "逻辑删除")
    @PreAuthorize("hasAuthority('receiveReport.remove')")
    @DeleteMapping("{id}")
    public Result<String> removeReport(@ApiParam(name = "id", value = "消息id", required = true) @PathVariable String id) {
        boolean flag = receiveReportService.removeById(id);
        if (flag) {
            return Result.ok();
        } else {
            return Result.fail();
        }
    }

    // 4.添加查询带分页的方法
    @ApiOperation(value = "条件查询分页方法")
    @PreAuthorize("hasAuthority('receiveReport.list')")
    @PostMapping("pageReportCondition")
    public Result<Page<MqttReceiveReport>> pageReportCondition(@RequestBody MqttReceiveReportQuery receiveQuery) {
        // 调用方法，实现分页查询
        Page<MqttReceiveReport> resultPage = receiveReportService.pageQuery(receiveQuery);
        return Result.ok(resultPage);
    }

    @ApiOperation(value = "获取最新一条记录")
    @PreAuthorize("hasAuthority('receiveReport.list')")
    @GetMapping("getLastReport")
    public Result<MqttReceiveReport> getLastReport() {
        // 调用方法，实现分页查询
        MqttReceiveReport receive = receiveReportService.getLastReport();
        return Result.ok(receive);
    }

    @ApiOperation("添加MQTT接收消息")
    @PostMapping("addReport")
    public Result<String> addReport(@RequestBody MqttReceiveReport mqttReceiveReport) {
        mqttReceiveReport.setReceiveTime(new Date());
        boolean save = receiveReportService.save(mqttReceiveReport);
        if (save) {
            return Result.ok();
        } else
            return Result.fail();
    }

    @ApiOperation("根据ID查询MQTT接收消息")
    @PreAuthorize("hasAuthority('receiveReport.list')")
    @GetMapping("getReport/{id}")
    public Result<MqttReceiveReport> getReport(@PathVariable String id) {
        MqttReceiveReport byId = receiveReportService.getById(id);
        return Result.ok(byId);
    }

    @ApiOperation("修改MQTT接收消息")
    @PostMapping("updateReport")
    public Result<String> updateReport(@RequestBody MqttReceiveReport mqttReceiveReport) {
        boolean b = receiveReportService.updateById(mqttReceiveReport);
        if (b) {
            return Result.ok();
        } else
            return Result.fail("修改失败");
    }
}
