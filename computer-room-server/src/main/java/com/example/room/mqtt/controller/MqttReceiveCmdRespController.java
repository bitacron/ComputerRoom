package com.example.room.mqtt.controller;

import com.baomidou.mybatisplus.extension.plugins.pagination.Page;
import com.example.room.mqtt.entity.MqttReceiveCmdResp;
import com.example.room.mqtt.entity.MqttReceiveReport;
import com.example.room.mqtt.entity.dto.MqttReceiveCmdRespQuery;
import com.example.room.mqtt.service.MqttReceiveCmdRespService;
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
@RequestMapping("/mqtt/receive/cmdResp")
public class MqttReceiveCmdRespController {
    // 把service注入
    @Resource
    private MqttReceiveCmdRespService mqttReceiveCmdResp;

    @ApiOperation(value = "所有MQTT接收消息列表")
    @PreAuthorize("hasAuthority('receiveCmdResp.list')")
    @GetMapping("findAll")
    public Result<List<MqttReceiveCmdResp>> findAllReceive() {
        // 调用service的方法实现查询所有的操作
        List<MqttReceiveCmdResp> list = mqttReceiveCmdResp.list();
        return Result.ok(list);
    }

    @ApiOperation(value = "逻辑删除")
    @PreAuthorize("hasAuthority('receiveCmdResp.remove')")
    @DeleteMapping("{id}")
    public Result<String> removeReceive(@ApiParam(name = "id", value = "消息id", required = true) @PathVariable String id) {
        boolean flag = mqttReceiveCmdResp.removeById(id);
        if (flag) {
            return Result.ok();
        } else {
            return Result.fail();
        }
    }

    // 4.添加查询带分页的方法
    @ApiOperation(value = "条件查询分页方法")
    @PreAuthorize("hasAuthority('receiveCmdResp.list')")
    @PostMapping("pageCmdRespCondition")
    public Result<Page<MqttReceiveCmdResp>> pageCmdRespCondition(@RequestBody MqttReceiveCmdRespQuery receiveQuery) {
        // 调用方法，实现分页查询
        Page<MqttReceiveCmdResp> resultPage = mqttReceiveCmdResp.pageQuery(receiveQuery);
        return Result.ok(resultPage);
    }

    @ApiOperation(value = "获取最新一条记录")
    @GetMapping("getLastReceive")
    public Result<MqttReceiveCmdResp> getLastReceive() {
        // 调用方法，实现分页查询
        MqttReceiveCmdResp receive = mqttReceiveCmdResp.getLastReceiveCmdResp();
        return Result.ok(receive);
    }

    @ApiOperation("添加MQTT接收消息")
    @PostMapping("addCmdResp")
    public Result<String> addCmdResp(@RequestBody MqttReceiveCmdResp mqttReceiveReport) {
        mqttReceiveReport.setReceiveTime(new Date());
        boolean save = mqttReceiveCmdResp.save(mqttReceiveReport);
        if (save) {
            return Result.ok();
        } else
            return Result.fail();
    }

    @ApiOperation("根据ID查询MQTT接收消息")
    @PreAuthorize("hasAuthority('receiveCmdResp.list')")
    @GetMapping("getCmdResp/{id}")
    public Result<MqttReceiveCmdResp> getCmdResp(@PathVariable String id) {
        MqttReceiveCmdResp byId = mqttReceiveCmdResp.getById(id);
        return Result.ok(byId);
    }

    @ApiOperation("修改MQTT接收消息")
    @PostMapping("updateCmdResp")
    public Result<String> updateCmdResp(@RequestBody MqttReceiveCmdResp mqttReceiveReport) {
        boolean b = mqttReceiveCmdResp.updateById(mqttReceiveReport);
        if (b) {
            return Result.ok();
        } else
            return Result.fail("修改失败");
    }
}
