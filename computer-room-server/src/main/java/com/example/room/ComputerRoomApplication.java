package com.example.room;

import org.mybatis.spring.annotation.MapperScan;
import org.springframework.boot.SpringApplication;
import org.springframework.boot.autoconfigure.SpringBootApplication;
import org.springframework.scheduling.annotation.EnableScheduling;

@SpringBootApplication
@EnableScheduling
@MapperScan("com.example.room.**.mapper")
public class ComputerRoomApplication {
    public static void main(String[] args) {
        SpringApplication.run(ComputerRoomApplication.class, args);
    }
}
