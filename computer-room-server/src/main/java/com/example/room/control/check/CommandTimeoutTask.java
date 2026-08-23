package com.example.room.control.check;

import com.baomidou.mybatisplus.core.conditions.query.LambdaQueryWrapper;
import com.example.room.control.entity.Command;
import com.example.room.control.service.CommandService;
import com.example.room.control.service.DeviceOptionService;
import lombok.extern.slf4j.Slf4j;
import org.springframework.scheduling.annotation.Scheduled;
import org.springframework.stereotype.Component;

import javax.annotation.Resource;
import java.util.Date;
import java.util.List;

@Slf4j
@Component
public class CommandTimeoutTask {

    private static final long COMMAND_TIMEOUT_MS = 8_000L;

    @Resource
    private CommandService commandService;

    @Resource
    private DeviceOptionService deviceOptionService;

    @Scheduled(fixedDelay = 2000)
    public void timeoutPendingCommands() {
        Date now = new Date();
        Date deadline = new Date(now.getTime() - COMMAND_TIMEOUT_MS);
        List<Command> pending = commandService.list(
                new LambdaQueryWrapper<Command>()
                        .eq(Command::getStatus, 1)
                        .le(Command::getGmtCreate, deadline)
        );
        if (pending.isEmpty()) {
            return;
        }
        int pushed = 0;
        for (Command command : pending) {
            boolean recent = command.getGmtCreate() != null
                    && now.getTime() - command.getGmtCreate().getTime() < 30_000L;
            if (recent) {
                deviceOptionService.markCommandTimeout(command);
                pushed++;
            } else {
                command.setStatus(4);
                commandService.updateById(command);
            }
        }
        log.info("收口 {} 条超时指令，其中推送前端 {} 条", pending.size(), pushed);
    }
}
