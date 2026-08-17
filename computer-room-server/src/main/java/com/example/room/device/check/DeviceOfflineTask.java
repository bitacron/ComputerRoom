package com.example.room.device.check;

import com.baomidou.mybatisplus.core.conditions.update.LambdaUpdateWrapper;
import com.example.room.device.entity.Device;
import com.example.room.device.mapper.DeviceMapper;
import lombok.extern.slf4j.Slf4j;
import org.springframework.scheduling.annotation.Scheduled;
import org.springframework.stereotype.Component;

import javax.annotation.Resource;
import java.util.Date;

@Slf4j
@Component  // 必须添加组件注解，让 Spring 管理
public class DeviceOfflineTask {

    @Resource
    private DeviceMapper deviceMapper;

    /**
     * 定时任务：每1分钟执行一次，将3分钟未活跃的设备标记为离线
     */
    @Scheduled(cron = "0 * * * * ?")
    public void checkOfflineDevices() {
        // 计算3分钟前的时间
        Date timeout = new Date(System.currentTimeMillis() - 3 * 60 * 1000);

        // 更新所有 last_active_time < timeout 且 status=1 的设备为离线
        int updated = deviceMapper.update(null,
                new LambdaUpdateWrapper<Device>()
                        .set(Device::getOnlineStatus, 0)
                        .set(Device::getLastOfflineTime, new Date())
                        .lt(Device::getLastActiveTime, timeout)
                        .eq(Device::getOnlineStatus, 1)
        );

        if (updated > 0) {
            log.info("标记了 {} 台设备为离线", updated);
        }
    }
}
