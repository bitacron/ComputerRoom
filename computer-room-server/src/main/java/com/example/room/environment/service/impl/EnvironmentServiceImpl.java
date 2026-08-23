package com.example.room.environment.service.impl;

import com.alibaba.excel.EasyExcel;
import com.baomidou.mybatisplus.core.conditions.query.LambdaQueryWrapper;
import com.baomidou.mybatisplus.core.conditions.query.QueryWrapper;
import com.baomidou.mybatisplus.extension.plugins.pagination.Page;
import com.baomidou.mybatisplus.extension.service.impl.ServiceImpl;
import com.example.room.device.entity.Device;
import com.example.room.device.service.DeviceService;
import com.example.room.environment.entity.Environment;
import com.example.room.environment.entity.dto.EnvironmentQuery;
import com.example.room.environment.entity.dto.EnvironmentStatisticsQuery;
import com.example.room.environment.mapper.EnvironmentMapper;
import com.example.room.environment.service.EnvironmentService;
import org.springframework.stereotype.Service;
import org.apache.commons.lang3.StringUtils;

import javax.annotation.Resource;

import javax.servlet.http.HttpServletResponse;
import java.io.IOException;
import java.io.UnsupportedEncodingException;
import java.net.URLEncoder;
import java.text.SimpleDateFormat;
import java.util.*;

/**
 * <p>
 * 环境数据 服务实现类
 * </p>
 *
 * @author zhmy
 * @since 2023-05-31
 */
@Service
public class EnvironmentServiceImpl extends ServiceImpl<EnvironmentMapper, Environment> implements EnvironmentService {

    @Resource
    private DeviceService deviceService;
    @Override
    public Page<Environment> pageQuery(EnvironmentQuery query) {
        if (query == null) {
            query = new EnvironmentQuery();
        }
        if (query.getCurrentPage() == null) {
            query.setCurrentPage(1);
        }
        if (query.getPageSize() == null) {
            query.setPageSize(10);
        }
        Page<Environment> page = new Page<>(query.getCurrentPage(), query.getPageSize());
        QueryWrapper<Environment> queryWrapper = buildEnvironmentQueryWrapper(query);
        queryWrapper.lambda().orderByDesc(Environment::getGmtCreate);
        return baseMapper.selectPage(page, queryWrapper);
    }

    @Override
    public List<Environment> listByQuery(EnvironmentQuery query) {
        if (query == null) {
            query = new EnvironmentQuery();
        }
        QueryWrapper<Environment> queryWrapper = buildEnvironmentQueryWrapper(query);
        queryWrapper.lambda().orderByDesc(Environment::getGmtCreate);
        return baseMapper.selectList(queryWrapper);
    }

    private QueryWrapper<Environment> buildEnvironmentQueryWrapper(EnvironmentQuery query) {
        QueryWrapper<Environment> queryWrapper = new QueryWrapper<>();
        if (Objects.nonNull(query.getSource())) {
            queryWrapper.lambda().eq(Environment::getSource, query.getSource());
        }
        if (!StringUtils.isEmpty(query.getBegin())) {
            queryWrapper.lambda().ge(Environment::getGmtCreate, query.getBegin());
        }
        if (!StringUtils.isEmpty(query.getEnd())) {
            queryWrapper.lambda().le(Environment::getGmtCreate, query.getEnd());
        }
        if (!StringUtils.isEmpty(query.getDeviceKey())) {
            queryWrapper.lambda().eq(Environment::getDeviceKey, query.getDeviceKey());
        }
        return queryWrapper;
    }

    @Override
    public List<Environment> getStatistics(EnvironmentStatisticsQuery query) {
        QueryWrapper<Environment> queryWrapper = new QueryWrapper<>();
        if (!StringUtils.isEmpty(query.getBegin())) {
            queryWrapper.lambda().ge(Environment::getGmtCreate, query.getBegin()); // ge大于等于
        }
        if (!StringUtils.isEmpty(query.getDeviceKey())) {
            queryWrapper.lambda().eq(Environment::getDeviceKey, query.getDeviceKey());
        }
        if (!StringUtils.isEmpty(query.getEnd())) {
            queryWrapper.lambda().le(Environment::getGmtCreate, query.getEnd()); // le小于等于
        }
        queryWrapper.lambda().orderByDesc(Environment::getGmtCreate);
        return baseMapper.selectList(queryWrapper);
    }

    @Override
    public Environment getLastData() {
        QueryWrapper<Environment> queryWrapper = new QueryWrapper<>();
        queryWrapper.lambda().orderByDesc(Environment::getGmtCreate);
        queryWrapper.last("limit 1");
        // Service层
        return this.getOne(queryWrapper);
    }
    @Override
    public Environment getLastData(String deviceKey) {
        if (StringUtils.isBlank(deviceKey)) {
            return getLastData();
        }
        Device device = deviceService.getByDeviceKey(deviceKey);
        return deviceService.toRealtimeEnvironment(device);
    }

    @Override
    public void exportExcel(List<Environment> list, HttpServletResponse response) throws IOException {
        // 设置响应头
        response.setContentType("application/vnd.openxmlformats-officedocument.spreadsheetml.sheet");
        response.setCharacterEncoding("utf-8");
        String fileName = URLEncoder.encode("环境监测数据_" +
                new SimpleDateFormat("yyyyMMdd_HHmmss").format(new Date()), "UTF-8");
        response.setHeader("Content-disposition", "attachment;filename=" + fileName + ".xlsx");

        // 表头
        String[] headers = {
                "序号", "数据来源", "上报设备", "温度(°C)", "湿度(%)",
                "烟雾浓度", "烟雾状态", "光照亮度(%)", "光照状态",
                "明火概率(%)", "火焰状态", "蜂鸣器报警", "散热设备",
                "LED开关", "检测时间", "添加时间"
        };

        // 写入数据
        EasyExcel.write(response.getOutputStream())
                .head(head(headers))
                .sheet("环境监测数据")
                .doWrite(data(list));
    }

    private List<List<String>> head(String[] headers) {
        List<List<String>> head = new ArrayList<>();
        for (String header : headers) {
            List<String> headColumn = new ArrayList<>();
            headColumn.add(header);
            head.add(headColumn);
        }
        return head;
    }

    private List<List<Object>> data(List<Environment> list) {
        List<List<Object>> dataList = new ArrayList<>();
        int index = 1;
        for (Environment env : list) {
            List<Object> row = new ArrayList<>();
            row.add(index++);  // 序号
            row.add(env.getSource() != null && env.getSource() == 1 ? "硬件上报" : "手动记录");
            row.add(env.getDeviceKey());
            row.add(env.getTemperature());
            row.add(env.getHumidity());
            row.add(env.getGasPpm());
            row.add(env.getGasStatus() != null && env.getGasStatus() == 1 ? "有烟雾" : "无烟雾");
            row.add(env.getLightPercentage());
            row.add(env.getLightStatus() != null && env.getLightStatus() == 1 ? "无光" : "有光");
            row.add(env.getFlamePercentage());
            row.add(env.getFlameStatus() != null && env.getFlameStatus() == 1 ? "有明火" : "无明火");
            row.add(env.getAlarmStatus() != null && env.getAlarmStatus() == 1 ? "报警" : "未报警");
            row.add(env.getFanStatus() != null && env.getFanStatus() == 1 ? "开启" : "关闭");
            row.add(env.getLedStatus() != null && env.getLedStatus() == 1 ? "开启" : "关闭");
            row.add(formatDate(env.getGmtMeasurement()));
            row.add(formatDate(env.getGmtCreate()));
            dataList.add(row);
        }
        return dataList;
    }

    private String formatDate(Date date) {
        if (date == null) return "";
        return new SimpleDateFormat("yyyy-MM-dd HH:mm:ss").format(date);
    }
}
