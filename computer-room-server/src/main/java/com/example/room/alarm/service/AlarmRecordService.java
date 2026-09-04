package com.example.room.alarm.service;

import com.baomidou.mybatisplus.extension.plugins.pagination.Page;
import com.baomidou.mybatisplus.extension.service.IService;
import com.example.room.alarm.entity.AlarmRecord;
import com.example.room.alarm.entity.dto.AlarmQuery;
import com.example.room.alarm.entity.vo.AlarmSummaryVO;
import com.example.room.environment.entity.Environment;

public interface AlarmRecordService extends IService<AlarmRecord> {

    Page<AlarmRecord> pageQuery(AlarmQuery query);

    AlarmSummaryVO summary();

    void processEnvironment(Environment environment);

    boolean handle(Long id, String handler);
}
