package com.example.room.mqtt.controller;

import com.baomidou.mybatisplus.extension.plugins.pagination.Page;
import com.example.room.mqtt.entity.MqttSendCmd;
import com.example.room.mqtt.entity.dto.MqttSendCmdQuery;
import com.example.room.mqtt.service.MqttSendCmdService;
import com.example.room.util.Result;
import io.swagger.annotations.Api;
import io.swagger.annotations.ApiOperation;
import io.swagger.annotations.ApiParam;
import org.springframework.security.access.prepost.PreAuthorize;
import org.springframework.web.bind.annotation.*;

import javax.annotation.Resource;
import java.util.Date;
import java.util.List;

@Api("MQTT发送消息管理")
@RestController
@RequestMapping("/mqtt/send/cmd")
public class MqttSendCmdController {

    // 把service注入
    @Resource
    private MqttSendCmdService sendCmdService;

    //1. 查询MQTT发送消息所有数据
    // restful风格
    // 访问地址  http://localhost:8001/mqtt/send/cmd/findAll
    @ApiOperation(value = "所有MQTT发送记录列表")
    @PreAuthorize("hasAuthority('sendCmd.list')")
    @GetMapping("findAll")
    public Result<List<MqttSendCmd>> findAllSendCmd() {
        // 调用service的方法实现查询所有的操作
        List<MqttSendCmd> list = sendCmdService.list();
        return Result.ok(list);
    }

    // 2.逻辑删除MQTT发送消息方法
    @ApiOperation(value = "逻辑删除")
    @PreAuthorize("hasAuthority('sendCmd.remove')")
    @DeleteMapping("{id}")
    public Result<String> removeSendCmd(@ApiParam(name = "id", value = "消息id", required = true) @PathVariable String id) {
        boolean flag = sendCmdService.removeById(id);
        if (flag) {
            return Result.ok();
        } else {
            return Result.fail();
        }
    }

    // 4.添加查询带分页的方法
    @ApiOperation(value = "条件查询分页方法")
    @PreAuthorize("hasAuthority('sendCmd.list')")
    @PostMapping("pageSendCmdCondition")
    public Result<Page<MqttSendCmd>> pageSendCmdCondition(@RequestBody(required = false) MqttSendCmdQuery sendQuery) {
        // 调用方法，实现分页查询
        Page<MqttSendCmd> resultPage = sendCmdService.pageQuery(sendQuery);
        return Result.ok(resultPage);
    }

    @ApiOperation(value = "获取最新一条记录")
    @GetMapping("getLastSendCmd")
    public Result<MqttSendCmd> getLastSendCmd() {
        // 调用方法，实现分页查询
        MqttSendCmd send = sendCmdService.getLastSendCmd();
        return Result.ok(send);
    }

    @ApiOperation("添加MQTT发送记录")
    @PostMapping("addSendCmd")
    public Result<String> addSendCmd(@RequestBody MqttSendCmd mqttSendCmd) {
        mqttSendCmd.setSendTime(new Date());
        boolean save = sendCmdService.save(mqttSendCmd);
        if (save) {
            return Result.ok();
        } else
            return Result.fail();
    }

    @ApiOperation("根据ID查询MQTT发送记录")
    @PreAuthorize("hasAuthority('sendCmd.list')")
    @GetMapping("getSendCmd/{id}")
    public Result<MqttSendCmd> getSendCmd(@PathVariable String id) {
        MqttSendCmd byId = sendCmdService.getById(id);
        return Result.ok(byId);
    }

    @ApiOperation("修改MQTT发送记录")
    @PostMapping("updateSendCmd")
    public Result<String> updateSendCmd(@RequestBody MqttSendCmd mqttSendCmd) {
        boolean b = sendCmdService.updateById(mqttSendCmd);
        if (b) {
            return Result.ok();
        } else
            return Result.fail("修改失败");
    }
}
