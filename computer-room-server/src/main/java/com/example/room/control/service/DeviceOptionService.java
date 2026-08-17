package com.example.room.control.service;

import com.baomidou.mybatisplus.extension.plugins.pagination.Page;
import com.baomidou.mybatisplus.extension.service.IService;
import com.example.room.control.entity.DeviceOption;
import com.example.room.control.entity.param.DeviceOptionControl;
import com.example.room.control.entity.param.DeviceOptionQuery;
import com.example.room.control.entity.vo.DeviceOptionVo;

/**
 * <p>
 * 反控记录 服务类
 * </p>
 *
 * @author zhmy
 * @since 2023-05-31
 */
public interface DeviceOptionService extends IService<DeviceOption> {

    // 条件查询分页方法
    Page<DeviceOptionVo> pageQuery(DeviceOptionQuery deviceOptionQuery);

    boolean controlDevice(DeviceOptionControl deviceOption, String operatorCode);

    void onMqttMessage(String topic, String payload);
}
