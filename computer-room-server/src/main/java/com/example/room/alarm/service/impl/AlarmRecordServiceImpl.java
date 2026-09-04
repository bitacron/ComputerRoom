package com.example.room.alarm.service.impl;

import com.baomidou.mybatisplus.core.conditions.query.QueryWrapper;
import com.baomidou.mybatisplus.extension.plugins.pagination.Page;
import com.baomidou.mybatisplus.extension.service.impl.ServiceImpl;
import com.example.room.alarm.entity.AlarmRecord;
import com.example.room.alarm.entity.dto.AlarmQuery;
import com.example.room.alarm.entity.vo.AlarmSummaryVO;
import com.example.room.alarm.mapper.AlarmRecordMapper;
import com.example.room.alarm.service.AlarmRecordService;
import com.example.room.environment.entity.Environment;
import lombok.extern.slf4j.Slf4j;
import org.apache.commons.lang3.StringUtils;
import org.springframework.stereotype.Service;

import java.text.SimpleDateFormat;
import java.util.Date;
import java.util.Objects;

/**
 * 报警记录：四类独立建单；数字量 Level 2、模拟量/温湿度 Level 1；
 * 处理后 5 分钟内同设备同类型不重复建单，期间若已恢复再超标则立即建单。
 */
@Slf4j
@Service
public class AlarmRecordServiceImpl extends ServiceImpl<AlarmRecordMapper, AlarmRecord> implements AlarmRecordService {

    /** 与固件蜂鸣器阈值一致：温度 50、湿度 60、烟雾 20、火焰数字量 */
    private static final float TEMP_THRESHOLD = 50f;
    private static final float HUMI_THRESHOLD = 60f;
    private static final float GAS_PPM_THRESHOLD = 20f;
    /** 火焰模拟量预警阈值（固件蜂鸣器只看数字量，模拟量单独作为 Level 1） */
    private static final float FLAME_PER_THRESHOLD = 50f;

    private static final String TYPE_TEMP = "TEMP";
    private static final String TYPE_HUMI = "HUMI";
    private static final String TYPE_GAS = "GAS";
    private static final String TYPE_FLAME = "FLAME";

    private static final int LEVEL_WARN = 1;
    private static final int LEVEL_URGENT = 2;

    private static final int STATUS_ALARMING = 0;
    private static final int STATUS_RECOVERED = 1;
    private static final int STATUS_HANDLED = 2;

    private static final long HANDLE_COOLDOWN_MS = 5 * 60 * 1000L;

    @Override
    public Page<AlarmRecord> pageQuery(AlarmQuery query) {
        if (query == null) {
            query = new AlarmQuery();
        }
        if (query.getCurrentPage() == null) {
            query.setCurrentPage(1);
        }
        if (query.getPageSize() == null) {
            query.setPageSize(10);
        }
        Page<AlarmRecord> page = new Page<>(query.getCurrentPage(), query.getPageSize());
        QueryWrapper<AlarmRecord> queryWrapper = new QueryWrapper<>();
        if (!StringUtils.isEmpty(query.getDeviceKey())) {
            queryWrapper.lambda().eq(AlarmRecord::getDeviceKey, query.getDeviceKey());
        }
        if (!StringUtils.isEmpty(query.getAlarmType())) {
            queryWrapper.lambda().eq(AlarmRecord::getAlarmType, query.getAlarmType());
        }
        if (query.getAlarmLevel() != null) {
            queryWrapper.lambda().eq(AlarmRecord::getAlarmLevel, query.getAlarmLevel());
        }
        if (query.getStatus() != null) {
            queryWrapper.lambda().eq(AlarmRecord::getStatus, query.getStatus());
        }
        if (!StringUtils.isEmpty(query.getBegin())) {
            queryWrapper.lambda().ge(AlarmRecord::getGmtCreate, query.getBegin());
        }
        if (!StringUtils.isEmpty(query.getEnd())) {
            queryWrapper.lambda().le(AlarmRecord::getGmtCreate, query.getEnd());
        }
        queryWrapper.lambda()
                .orderByAsc(AlarmRecord::getStatus)
                .orderByDesc(AlarmRecord::getAlarmLevel)
                .orderByDesc(AlarmRecord::getGmtCreate);
        return baseMapper.selectPage(page, queryWrapper);
    }

    @Override
    public AlarmSummaryVO summary() {
        AlarmSummaryVO vo = new AlarmSummaryVO();
        vo.setAlarming(countByStatus(STATUS_ALARMING));
        vo.setUrgent(countAlarmingByLevel(LEVEL_URGENT));
        vo.setHandled(countByStatus(STATUS_HANDLED));
        QueryWrapper<AlarmRecord> todayWrapper = new QueryWrapper<>();
        todayWrapper.lambda().ge(AlarmRecord::getGmtCreate, todayStart());
        vo.setToday(Math.toIntExact(this.count(todayWrapper)));
        return vo;
    }

    @Override
    public void processEnvironment(Environment environment) {
        if (environment == null || StringUtils.isBlank(environment.getDeviceKey())) {
            return;
        }
        try {
            String deviceKey = environment.getDeviceKey();
            syncAlarm(deviceKey, TYPE_TEMP, judgeTemp(environment));
            syncAlarm(deviceKey, TYPE_HUMI, judgeHumi(environment));
            syncAlarm(deviceKey, TYPE_GAS, judgeGas(environment));
            syncAlarm(deviceKey, TYPE_FLAME, judgeFlame(environment));
        } catch (Exception e) {
            log.error("处理报警记录失败, deviceKey={}", environment.getDeviceKey(), e);
        }
    }

    @Override
    public boolean handle(Long id, String handler) {
        AlarmRecord record = this.getById(id);
        if (record == null || !Objects.equals(record.getStatus(), STATUS_ALARMING)) {
            return false;
        }
        record.setStatus(STATUS_HANDLED);
        record.setHandler(handler);
        record.setHandleTime(new Date());
        return this.updateById(record);
    }

    private void syncAlarm(String deviceKey, String alarmType, JudgeResult judge) {
        AlarmRecord unrecovered = findUnrecovered(deviceKey, alarmType);
        if (judge.alarming) {
            if (unrecovered != null && Objects.equals(unrecovered.getStatus(), STATUS_ALARMING)) {
                upgradeIfNeeded(unrecovered, judge);
                return;
            }
            if (unrecovered != null
                    && Objects.equals(unrecovered.getStatus(), STATUS_HANDLED)
                    && inHandleCooldown(unrecovered)) {
                return;
            }
            AlarmRecord record = new AlarmRecord();
            record.setDeviceKey(deviceKey);
            record.setAlarmType(alarmType);
            record.setAlarmLevel(judge.level);
            record.setContent(judge.content);
            record.setStatus(STATUS_ALARMING);
            record.setGmtCreate(new Date());
            this.save(record);
            return;
        }
        if (unrecovered == null) {
            return;
        }
        unrecovered.setRecoverTime(new Date());
        if (Objects.equals(unrecovered.getStatus(), STATUS_ALARMING)) {
            unrecovered.setStatus(STATUS_RECOVERED);
        }
        this.updateById(unrecovered);
    }

    private void upgradeIfNeeded(AlarmRecord open, JudgeResult judge) {
        if (judge.level == null || open.getAlarmLevel() == null) {
            return;
        }
        if (judge.level > open.getAlarmLevel()) {
            open.setAlarmLevel(judge.level);
            open.setContent(judge.content);
            this.updateById(open);
        }
    }

    private boolean inHandleCooldown(AlarmRecord handled) {
        if (handled.getHandleTime() == null) {
            return false;
        }
        return System.currentTimeMillis() - handled.getHandleTime().getTime() < HANDLE_COOLDOWN_MS;
    }

    private AlarmRecord findUnrecovered(String deviceKey, String alarmType) {
        QueryWrapper<AlarmRecord> wrapper = new QueryWrapper<>();
        wrapper.lambda()
                .eq(AlarmRecord::getDeviceKey, deviceKey)
                .eq(AlarmRecord::getAlarmType, alarmType)
                .isNull(AlarmRecord::getRecoverTime)
                .orderByDesc(AlarmRecord::getGmtCreate)
                .last("limit 1");
        return this.getOne(wrapper, false);
    }

    private JudgeResult judgeTemp(Environment env) {
        boolean alarming = env.getTemperature() >= TEMP_THRESHOLD;
        if (!alarming) {
            return JudgeResult.normal();
        }
        return JudgeResult.of(LEVEL_WARN,
                String.format("温度 %.1f℃，超过阈值 %.0f℃", env.getTemperature(), TEMP_THRESHOLD));
    }

    private JudgeResult judgeHumi(Environment env) {
        boolean alarming = env.getHumidity() >= HUMI_THRESHOLD;
        if (!alarming) {
            return JudgeResult.normal();
        }
        return JudgeResult.of(LEVEL_WARN,
                String.format("湿度 %.1f%%，超过阈值 %.0f%%", env.getHumidity(), HUMI_THRESHOLD));
    }

    private JudgeResult judgeGas(Environment env) {
        boolean digital = env.getGasStatus() != null && env.getGasStatus() == 1;
        boolean analog = env.getGasPpm() >= GAS_PPM_THRESHOLD;
        if (!digital && !analog) {
            return JudgeResult.normal();
        }
        if (digital && analog) {
            return JudgeResult.of(LEVEL_URGENT,
                    String.format("烟雾数字量报警 + 浓度 %.1fppm", env.getGasPpm()));
        }
        if (digital) {
            return JudgeResult.of(LEVEL_URGENT, "烟雾数字量报警");
        }
        return JudgeResult.of(LEVEL_WARN,
                String.format("烟雾浓度 %.1fppm，超过阈值 %.0fppm", env.getGasPpm(), GAS_PPM_THRESHOLD));
    }

    private JudgeResult judgeFlame(Environment env) {
        boolean digital = env.getFlameStatus() != null && env.getFlameStatus() == 1;
        boolean analog = env.getFlamePercentage() >= FLAME_PER_THRESHOLD;
        if (!digital && !analog) {
            return JudgeResult.normal();
        }
        if (digital && analog) {
            return JudgeResult.of(LEVEL_URGENT,
                    String.format("火焰数字量报警 + 概率 %.1f%%", env.getFlamePercentage()));
        }
        if (digital) {
            return JudgeResult.of(LEVEL_URGENT, "火焰数字量报警");
        }
        return JudgeResult.of(LEVEL_WARN,
                String.format("火焰概率 %.1f%%，超过阈值 %.0f%%", env.getFlamePercentage(), FLAME_PER_THRESHOLD));
    }

    private int countByStatus(int status) {
        QueryWrapper<AlarmRecord> wrapper = new QueryWrapper<>();
        wrapper.lambda().eq(AlarmRecord::getStatus, status);
        return Math.toIntExact(this.count(wrapper));
    }

    private int countAlarmingByLevel(int level) {
        QueryWrapper<AlarmRecord> wrapper = new QueryWrapper<>();
        wrapper.lambda()
                .eq(AlarmRecord::getStatus, STATUS_ALARMING)
                .eq(AlarmRecord::getAlarmLevel, level);
        return Math.toIntExact(this.count(wrapper));
    }

    private String todayStart() {
        return new SimpleDateFormat("yyyy-MM-dd").format(new Date()) + " 00:00:00";
    }

    private static class JudgeResult {
        private final boolean alarming;
        private final Integer level;
        private final String content;

        private JudgeResult(boolean alarming, Integer level, String content) {
            this.alarming = alarming;
            this.level = level;
            this.content = content;
        }

        static JudgeResult normal() {
            return new JudgeResult(false, null, null);
        }

        static JudgeResult of(int level, String content) {
            return new JudgeResult(true, level, content);
        }
    }
}
