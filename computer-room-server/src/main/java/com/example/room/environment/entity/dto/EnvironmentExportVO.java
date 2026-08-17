package com.example.room.environment.entity.dto;

import com.alibaba.excel.annotation.ExcelIgnore;
import com.alibaba.excel.annotation.ExcelProperty;
import com.alibaba.excel.annotation.write.style.ColumnWidth;
import com.alibaba.excel.annotation.write.style.ContentRowHeight;
import com.alibaba.excel.annotation.write.style.HeadRowHeight;
import lombok.Data;

import java.util.Date;

@Data
@HeadRowHeight(25)
@ContentRowHeight(20)
public class EnvironmentExportVO {


    @ExcelProperty("序号")
    @ColumnWidth(8)
    private Integer index;

    @ExcelIgnore
    private Integer source;

    @ExcelProperty("数据来源")
    @ColumnWidth(12)
    private String sourceStr;

    @ExcelProperty("上报设备")
    @ColumnWidth(15)
    private String deviceKey;

    @ExcelProperty("温度 (°C)")
    @ColumnWidth(12)
    private Float temperature;

    @ExcelProperty("湿度 (%)")
    @ColumnWidth(12)
    private Float humidity;

    @ExcelProperty("烟雾浓度")
    @ColumnWidth(12)
    private Float gasPpm;

    @ExcelIgnore
    private Integer gasStatus;

    @ExcelProperty("烟雾状态")
    @ColumnWidth(12)
    private String gasStatusStr;

    @ExcelProperty("光照亮度 (%)")
    @ColumnWidth(16)
    private Float lightPercentage;

    @ExcelIgnore
    private Integer lightStatus;

    @ExcelProperty("光照状态")
    @ColumnWidth(12)
    private String lightStatusStr;

    @ExcelProperty("明火概率 (%)")
    @ColumnWidth(16)
    private Float flamePercentage;

    @ExcelIgnore
    private Integer flameStatus;

    @ExcelProperty("火焰状态")
    @ColumnWidth(12)
    private String flameStatusStr;

    @ExcelIgnore
    private Integer alarmStatus;

    @ExcelProperty("蜂鸣器报警")
    @ColumnWidth(14)
    private String alarmStatusStr;

    @ExcelIgnore
    private Integer fanStatus;

    @ExcelProperty("散热设备")
    @ColumnWidth(12)
    private String fanStatusStr;

    @ExcelIgnore
    private Integer ledStatus;

    @ExcelProperty("LED 开关")
    @ColumnWidth(12)
    private String ledStatusStr;

    @ExcelProperty("检测时间")
    @ColumnWidth(20)
    private Date gmtMeasurement;

    @ExcelProperty("添加时间")
    @ColumnWidth(20)
    private Date gmtCreate;

    public void setSource(Integer source) {
        this.source = source;
        this.sourceStr = source != null && source == 1 ? "硬件上报" : "手动记录";
    }

    public void setGasStatus(Integer gasStatus) {
        this.gasStatus = gasStatus;
        this.gasStatusStr = gasStatus != null && gasStatus == 1 ? "有烟雾" : "无烟雾";
    }

    public void setLightStatus(Integer lightStatus) {
        this.lightStatus = lightStatus;
        this.lightStatusStr = lightStatus != null && lightStatus == 1 ? "无光" : "有光";
    }

    public void setFlameStatus(Integer flameStatus) {
        this.flameStatus = flameStatus;
        this.flameStatusStr = flameStatus != null && flameStatus == 1 ? "有明火" : "无明火";
    }

    public void setAlarmStatus(Integer alarmStatus) {
        this.alarmStatus = alarmStatus;
        this.alarmStatusStr = alarmStatus != null && alarmStatus == 1 ? "报警" : "未报警";
    }

    public void setFanStatus(Integer fanStatus) {
        this.fanStatus = fanStatus;
        this.fanStatusStr = fanStatus != null && fanStatus == 1 ? "开启" : "关闭";
    }

    public void setLedStatus(Integer ledStatus) {
        this.ledStatus = ledStatus;
        this.ledStatusStr = ledStatus != null && ledStatus == 1 ? "开启" : "关闭";
    }

}