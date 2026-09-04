/*
 Navicat Premium Data Transfer

 Source Server         : localhost
 Source Server Type    : MySQL
 Source Server Version : 50725
 Source Host           : localhost:3306
 Source Schema         : computer_room

 Target Server Type    : MySQL
 Target Server Version : 50725
 File Encoding         : 65001

 Date: 04/04/2026 00:47:09
*/

SET NAMES utf8mb4;
SET FOREIGN_KEY_CHECKS = 0;

-- ----------------------------
-- Table structure for account
-- ----------------------------
DROP TABLE IF EXISTS `account`;
CREATE TABLE `account`  (
  `id` bigint(19) NOT NULL AUTO_INCREMENT COMMENT '用户ID',
  `username` varchar(50) CHARACTER SET utf8mb4 COLLATE utf8mb4_general_ci NOT NULL COMMENT '用户名',
  `password` varchar(255) CHARACTER SET utf8mb4 COLLATE utf8mb4_general_ci NOT NULL COMMENT '密码',
  `nickname` varchar(50) CHARACTER SET utf8mb4 COLLATE utf8mb4_general_ci NULL DEFAULT NULL COMMENT '昵称',
  `avatar` varchar(500) CHARACTER SET utf8mb4 COLLATE utf8mb4_general_ci NULL DEFAULT NULL COMMENT '头像',
  `role` varchar(20) CHARACTER SET utf8mb4 COLLATE utf8mb4_general_ci NULL DEFAULT NULL COMMENT '角色',
  `phone` varchar(64) CHARACTER SET utf8mb4 COLLATE utf8mb4_general_ci NULL DEFAULT NULL COMMENT '手机号',
  `email` varchar(64) CHARACTER SET utf8mb4 COLLATE utf8mb4_general_ci NULL DEFAULT NULL COMMENT '邮箱',
  `gender` tinyint(4) NULL DEFAULT NULL COMMENT '性别（0-女；1-男）',
  `gmt_created` datetime NULL DEFAULT NULL COMMENT '创建日期',
  `gmt_modified` datetime NULL DEFAULT NULL COMMENT '修改时间',
  PRIMARY KEY (`id`) USING BTREE,
  UNIQUE INDEX `uk_username`(`username`) USING BTREE COMMENT '用户名唯一索引',
  INDEX `idx_username`(`username`) USING BTREE,
  INDEX `idx_gmt_created`(`gmt_created`) USING BTREE,
  INDEX `idx_gmt_modified`(`gmt_modified`) USING BTREE
) ENGINE = InnoDB AUTO_INCREMENT = 8 CHARACTER SET = utf8mb4 COLLATE = utf8mb4_general_ci COMMENT = '用户信息表' ROW_FORMAT = Dynamic;

-- ----------------------------
-- Table structure for access_permission
-- ----------------------------
DROP TABLE IF EXISTS `access_permission`;
CREATE TABLE `access_permission`  (
  `id` bigint NOT NULL COMMENT '编号',
  `pid` bigint NOT NULL DEFAULT 0 COMMENT '所属上级',
  `name` varchar(20) CHARACTER SET utf8mb4 COLLATE utf8mb4_general_ci NOT NULL DEFAULT '' COMMENT '名称',
  `type` tinyint(3) NOT NULL DEFAULT 0 COMMENT '类型(1:菜单,2:按钮)',
  `permission_value` varchar(50) CHARACTER SET utf8mb4 COLLATE utf8mb4_general_ci NULL DEFAULT NULL COMMENT '权限值',
  `path` varchar(100) CHARACTER SET utf8mb4 COLLATE utf8mb4_general_ci NULL DEFAULT NULL COMMENT '访问路径',
  `component` varchar(100) CHARACTER SET utf8mb4 COLLATE utf8mb4_general_ci NULL DEFAULT NULL COMMENT '组件路径',
  `icon` varchar(50) CHARACTER SET utf8mb4 COLLATE utf8mb4_general_ci NULL DEFAULT NULL COMMENT '图标',
  `status` tinyint(4) NULL DEFAULT NULL COMMENT '状态(0:禁止,1:正常)',
  `sort` int NOT NULL DEFAULT 0 COMMENT '同级排序，越小越靠前',
  `is_deleted` tinyint(1) UNSIGNED NOT NULL DEFAULT 0 COMMENT '逻辑删除 1（true）已删除， 0（false）未删除',
  `gmt_create` datetime NULL DEFAULT NULL COMMENT '创建时间',
  `gmt_modified` datetime NULL DEFAULT NULL COMMENT '更新时间',
  PRIMARY KEY (`id`) USING BTREE,
  INDEX `idx_pid`(`pid`) USING BTREE
) ENGINE = InnoDB CHARACTER SET = utf8mb4 COLLATE = utf8mb4_general_ci COMMENT = '权限' ROW_FORMAT = DYNAMIC;

-- ----------------------------
-- Table structure for access_role
-- ----------------------------
DROP TABLE IF EXISTS `access_role`;
CREATE TABLE `access_role`  (
  `id` bigint NOT NULL COMMENT '角色id',
  `role_name` varchar(20) CHARACTER SET utf8 COLLATE utf8_general_ci NOT NULL DEFAULT '' COMMENT '角色名称',
  `role_code` varchar(20) CHARACTER SET utf8 COLLATE utf8_general_ci NULL DEFAULT NULL COMMENT '角色编码',
  `remark` varchar(255) CHARACTER SET utf8 COLLATE utf8_general_ci NULL DEFAULT NULL COMMENT '备注',
  `is_deleted` tinyint(1) UNSIGNED NOT NULL DEFAULT 0 COMMENT '逻辑删除 1（true）已删除， 0（false）未删除',
  `gmt_create` datetime NOT NULL COMMENT '创建时间',
  `gmt_modified` datetime NOT NULL COMMENT '更新时间',
  PRIMARY KEY (`id`) USING BTREE
) ENGINE = InnoDB CHARACTER SET = utf8 COLLATE = utf8_general_ci ROW_FORMAT = DYNAMIC;

-- ----------------------------
-- Table structure for access_role_permission
-- ----------------------------
DROP TABLE IF EXISTS `access_role_permission`;
CREATE TABLE `access_role_permission`  (
  `id` bigint NOT NULL,
  `role_id` bigint NOT NULL DEFAULT 0,
  `permission_id` bigint NOT NULL DEFAULT 0,
  `is_deleted` tinyint(1) UNSIGNED NOT NULL DEFAULT 0 COMMENT '逻辑删除 1（true）已删除， 0（false）未删除',
  `gmt_create` datetime NULL DEFAULT NULL COMMENT '创建时间',
  `gmt_modified` datetime NULL DEFAULT NULL COMMENT '更新时间',
  PRIMARY KEY (`id`) USING BTREE,
  INDEX `idx_role_id`(`role_id`) USING BTREE,
  INDEX `idx_permission_id`(`permission_id`) USING BTREE
) ENGINE = InnoDB CHARACTER SET = utf8 COLLATE = utf8_general_ci COMMENT = '角色权限' ROW_FORMAT = DYNAMIC;

-- ----------------------------
-- Table structure for access_user
-- ----------------------------
DROP TABLE IF EXISTS `access_user`;
CREATE TABLE `access_user`  (
  `id` bigint NOT NULL COMMENT '用户ID',
  `username` varchar(20) CHARACTER SET utf8mb4 COLLATE utf8mb4_general_ci NOT NULL DEFAULT '' COMMENT '用户名',
  `password` varchar(32) CHARACTER SET utf8mb4 COLLATE utf8mb4_general_ci NOT NULL DEFAULT '' COMMENT '密码（MD5）',
  `nick_name` varchar(50) CHARACTER SET utf8mb4 COLLATE utf8mb4_general_ci NULL DEFAULT NULL COMMENT '昵称',
  `phone` varchar(64) CHARACTER SET utf8mb4 COLLATE utf8mb4_general_ci NULL DEFAULT NULL COMMENT '手机号',
  `email` varchar(64) CHARACTER SET utf8mb4 COLLATE utf8mb4_general_ci NULL DEFAULT NULL COMMENT '邮箱',
  `avatar` varchar(500) CHARACTER SET utf8mb4 COLLATE utf8mb4_general_ci NULL DEFAULT NULL COMMENT '头像',
  `gender` tinyint(4) NULL DEFAULT NULL COMMENT '性别（0-女；1-男）',
  `is_deleted` tinyint(1) UNSIGNED NOT NULL DEFAULT 0 COMMENT '逻辑删除 1（true）已删除， 0（false）未删除',
  `gmt_create` datetime NOT NULL COMMENT '创建时间',
  `gmt_modified` datetime NOT NULL COMMENT '更新时间',
  PRIMARY KEY (`id`) USING BTREE,
  UNIQUE INDEX `uk_username`(`username`) USING BTREE
) ENGINE = InnoDB CHARACTER SET = utf8mb4 COLLATE = utf8mb4_general_ci COMMENT = '用户表' ROW_FORMAT = DYNAMIC;

-- ----------------------------
-- Table structure for access_user_role
-- ----------------------------
DROP TABLE IF EXISTS `access_user_role`;
CREATE TABLE `access_user_role`  (
  `id` bigint NOT NULL COMMENT '主键id',
  `role_id` bigint NOT NULL DEFAULT 0 COMMENT '角色id',
  `user_id` bigint NOT NULL DEFAULT 0 COMMENT '用户id',
  `is_deleted` tinyint(1) UNSIGNED NOT NULL DEFAULT 0 COMMENT '逻辑删除 1（true）已删除， 0（false）未删除',
  `gmt_create` datetime NOT NULL COMMENT '创建时间',
  `gmt_modified` datetime NOT NULL COMMENT '更新时间',
  PRIMARY KEY (`id`) USING BTREE,
  INDEX `idx_role_id`(`role_id`) USING BTREE,
  INDEX `idx_user_id`(`user_id`) USING BTREE
) ENGINE = InnoDB CHARACTER SET = utf8 COLLATE = utf8_general_ci ROW_FORMAT = DYNAMIC;

-- ----------------------------
-- Table structure for command
-- ----------------------------
DROP TABLE IF EXISTS `command`;
CREATE TABLE `command`  (
  `id` bigint(19) NOT NULL COMMENT '主键ID',
  `cmd_id` char(50) CHARACTER SET utf8mb4 COLLATE utf8mb4_general_ci NOT NULL COMMENT '指令ID',
  `device_key` varchar(32) CHARACTER SET utf8mb4 COLLATE utf8mb4_general_ci NULL DEFAULT NULL,
  `device_type` varchar(10) CHARACTER SET utf8mb4 COLLATE utf8mb4_general_ci NOT NULL COMMENT '控制类型：1-散热',
  `command` varchar(10) CHARACTER SET utf8mb4 COLLATE utf8mb4_general_ci NOT NULL COMMENT '命令：1-ON 2-OFF',
  `status` tinyint(4) NULL DEFAULT NULL COMMENT '状态\r\n',
  `gmt_create` datetime NOT NULL COMMENT '创建时间',
  `is_deleted` tinyint(1) NULL DEFAULT 0 COMMENT '逻辑删除',
  PRIMARY KEY (`id`) USING BTREE
) ENGINE = InnoDB CHARACTER SET = utf8mb4 COLLATE = utf8mb4_general_ci COMMENT = '设备反控记录' ROW_FORMAT = Dynamic;

-- ----------------------------
-- Table structure for device
-- ----------------------------
DROP TABLE IF EXISTS `device`;
CREATE TABLE `device`  (
  `id` bigint(20) UNSIGNED NOT NULL AUTO_INCREMENT COMMENT '主键ID',
  `product_key` varchar(32) CHARACTER SET utf8mb4 COLLATE utf8mb4_unicode_ci NOT NULL COMMENT '产品标识（冗余字段，方便查询）',
  `device_key` varchar(64) CHARACTER SET utf8mb4 COLLATE utf8mb4_unicode_ci NOT NULL COMMENT '设备唯一标识（用户自定义或系统生成）',
  `device_name` varchar(128) CHARACTER SET utf8mb4 COLLATE utf8mb4_unicode_ci NULL DEFAULT NULL COMMENT '设备名称（用户自定义）',
  `online_status` tinyint(4) NULL DEFAULT 0 COMMENT '在线状态: 0-离线, 1-在线（实时状态）',
  `last_online_time` datetime NULL DEFAULT NULL COMMENT '最后上线时间',
  `last_offline_time` datetime NULL DEFAULT NULL COMMENT '最后离线时间',
  `last_active_time` datetime NULL DEFAULT NULL COMMENT '最后活跃时间（上报数据时间）',
  `temperature` decimal(5, 2) NULL DEFAULT NULL COMMENT '当前温度',
  `humidity` decimal(5, 2) NULL DEFAULT NULL COMMENT '当前湿度',
  `gas_ppm` decimal(5, 2) NULL DEFAULT NULL COMMENT '当前烟雾浓度',
  `gas_status` tinyint(4) NULL DEFAULT NULL COMMENT '当前烟雾状态（1：有烟雾；0：无烟雾）',
  `light_status` tinyint(4) NULL DEFAULT NULL COMMENT '当前光照状态（1：无光；0：有光）',
  `flame_status` tinyint(4) NULL DEFAULT NULL COMMENT '当前火焰状态（1：有火；0：无火）',
  `light_percentage` decimal(5, 2) NULL DEFAULT NULL COMMENT '当前光照强度（0-100）',
  `flame_percentage` decimal(5, 2) NULL DEFAULT NULL COMMENT '当前火焰百分比（0-100）',
  `alarm_status` tinyint(4) NULL DEFAULT NULL COMMENT '当前蜂鸣器报警（1：报警；0：未报警）',
  `fan_status` tinyint(4) NULL DEFAULT NULL COMMENT '当前散热设备开关（1：开启；0：关闭）',
  `led_status` tinyint(4) NULL DEFAULT NULL COMMENT '当前 LED 开关（1：开启；0：关闭）',
  `last_report_time` datetime NULL DEFAULT NULL COMMENT '影子对应的最近一次测量时间',
  `last_actuator_time` datetime NULL DEFAULT NULL COMMENT '最近一次执行器（风扇/LED）变更时间',
  PRIMARY KEY (`id`) USING BTREE,
  INDEX `idx_device_key`(`device_key`) USING BTREE
) ENGINE = InnoDB AUTO_INCREMENT = 4 CHARACTER SET = utf8mb4 COLLATE = utf8mb4_unicode_ci COMMENT = '设备表' ROW_FORMAT = Dynamic;

-- ----------------------------
-- Table structure for device_option
-- ----------------------------
DROP TABLE IF EXISTS `device_option`;
CREATE TABLE `device_option`  (
  `id` bigint(19) NOT NULL COMMENT '反控ID',
  `device_key` varchar(32) CHARACTER SET utf8mb4 COLLATE utf8mb4_general_ci NOT NULL COMMENT '设备ID',
  `action` varchar(20) CHARACTER SET utf8mb4 COLLATE utf8mb4_general_ci NULL DEFAULT NULL COMMENT '操作',
  `operator_code` varchar(20) CHARACTER SET utf8mb4 COLLATE utf8mb4_general_ci NULL DEFAULT NULL COMMENT '操作人',
  `gmt_create` datetime NOT NULL COMMENT '创建时间',
  `is_deleted` tinyint(1) NULL DEFAULT 0 COMMENT '逻辑删除',
  PRIMARY KEY (`id`) USING BTREE
) ENGINE = InnoDB CHARACTER SET = utf8mb4 COLLATE = utf8mb4_general_ci COMMENT = '设备反控记录' ROW_FORMAT = Dynamic;

-- ----------------------------
-- Table structure for environment
-- ----------------------------
DROP TABLE IF EXISTS `environment`;
CREATE TABLE `environment`  (
  `id` bigint(19) NOT NULL COMMENT '环境数据ID',
  `source` tinyint(4) NULL DEFAULT 1 COMMENT '数据来源（1：硬件上报；0：手动记录）',
  `device_key` varchar(32) CHARACTER SET utf8mb4 COLLATE utf8mb4_general_ci NULL DEFAULT NULL COMMENT '上报设备',
  `temperature` decimal(5, 2) NULL DEFAULT NULL COMMENT '温度',
  `humidity` decimal(5, 2) NULL DEFAULT NULL COMMENT '湿度',
  `gas_ppm` decimal(5, 2) NULL DEFAULT NULL COMMENT '烟雾浓度',
  `gas_status` tinyint(4) NULL DEFAULT NULL COMMENT 'MQ2烟雾AO状态（1：有烟雾；0：无烟雾）',
  `light_status` tinyint(4) NULL DEFAULT NULL COMMENT '光敏电阻数值（1：无光；0：有光）',
  `flame_status` tinyint(4) NULL DEFAULT NULL COMMENT '火焰传感器AO状态（1：有火；0：无火）',
  `light_percentage` decimal(5, 2) NULL DEFAULT NULL COMMENT '光照强度（0-100）',
  `flame_percentage` decimal(5, 2) NULL DEFAULT NULL COMMENT '附近有火焰的百分比（0-100）',
  `alarm_status` tinyint(4) NULL DEFAULT NULL COMMENT '蜂鸣器报警状态（1：报警；0：未报警）',
  `fan_status` tinyint(4) NULL DEFAULT NULL COMMENT '散热设备开关（1：开启；0：关闭）',
  `led_status` tinyint(4) NULL DEFAULT NULL COMMENT 'led开关（1：开启；0：关闭）',
  `gmt_measurement` datetime NULL DEFAULT NULL COMMENT '测量时间',
  `gmt_create` datetime NOT NULL COMMENT '创建时间',
  `is_deleted` tinyint(1) UNSIGNED NOT NULL DEFAULT 0 COMMENT '逻辑删除 1（true）已删除， 0（false）未删除',
  PRIMARY KEY (`id`) USING BTREE
) ENGINE = InnoDB CHARACTER SET = utf8mb4 COLLATE = utf8mb4_general_ci COMMENT = '环境监测数据' ROW_FORMAT = DYNAMIC;

-- ----------------------------
-- Table structure for mqtt_receive_cmd_resp
-- ----------------------------
DROP TABLE IF EXISTS `mqtt_receive_cmd_resp`;
CREATE TABLE `mqtt_receive_cmd_resp`  (
  `id` bigint(19) NOT NULL COMMENT 'ID',
  `topic` varchar(255) CHARACTER SET utf8mb4 COLLATE utf8mb4_unicode_ci NULL DEFAULT NULL COMMENT 'topic主题',
  `payload` text CHARACTER SET utf8mb4 COLLATE utf8mb4_unicode_ci NULL COMMENT 'payload有效载荷',
  `device_key` varchar(32) CHARACTER SET utf8mb4 COLLATE utf8mb4_unicode_ci NULL DEFAULT NULL COMMENT '设备id',
  `receive_time` datetime NULL DEFAULT NULL COMMENT '接收时间',
  `is_deleted` tinyint(1) UNSIGNED ZEROFILL NULL DEFAULT 0 COMMENT '逻辑删除 1（true）已删除， 0（false）未删除',
  PRIMARY KEY (`id`) USING BTREE,
  INDEX `idx_receive_time`(`receive_time`) USING BTREE,
  INDEX `idx_device_id`(`device_key`) USING BTREE
) ENGINE = InnoDB CHARACTER SET = utf8mb4 COLLATE = utf8mb4_unicode_ci COMMENT = 'MQTT接收数据-指令回复表' ROW_FORMAT = Dynamic;

-- ----------------------------
-- Table structure for mqtt_receive_report
-- ----------------------------
DROP TABLE IF EXISTS `mqtt_receive_report`;
CREATE TABLE `mqtt_receive_report`  (
  `id` bigint(19) NOT NULL COMMENT 'ID',
  `topic` varchar(255) CHARACTER SET utf8mb4 COLLATE utf8mb4_unicode_ci NULL DEFAULT NULL COMMENT 'topic主题',
  `payload` text CHARACTER SET utf8mb4 COLLATE utf8mb4_unicode_ci NULL COMMENT 'payload有效载荷',
  `device_key` varchar(32) CHARACTER SET utf8mb4 COLLATE utf8mb4_unicode_ci NULL DEFAULT NULL COMMENT '设备id',
  `receive_time` datetime NULL DEFAULT NULL COMMENT '接收时间',
  `is_deleted` tinyint(1) UNSIGNED ZEROFILL NULL DEFAULT 0 COMMENT '逻辑删除 1（true）已删除， 0（false）未删除',
  PRIMARY KEY (`id`) USING BTREE,
  INDEX `idx_receive_time`(`receive_time`) USING BTREE,
  INDEX `idx_device_id`(`device_key`) USING BTREE
) ENGINE = InnoDB CHARACTER SET = utf8mb4 COLLATE = utf8mb4_unicode_ci COMMENT = 'MQTT接收数据-上报表' ROW_FORMAT = Dynamic;

-- ----------------------------
-- Table structure for mqtt_send_cmd
-- ----------------------------
DROP TABLE IF EXISTS `mqtt_send_cmd`;
CREATE TABLE `mqtt_send_cmd`  (
  `id` bigint(19) NOT NULL COMMENT 'ID',
  `topic` varchar(255) CHARACTER SET utf8mb4 COLLATE utf8mb4_unicode_ci NULL DEFAULT NULL COMMENT 'topic主题',
  `payload` text CHARACTER SET utf8mb4 COLLATE utf8mb4_unicode_ci NULL COMMENT 'payload有效载荷',
  `device_key` varchar(32) CHARACTER SET utf8mb4 COLLATE utf8mb4_unicode_ci NULL DEFAULT NULL COMMENT '设备id',
  `send_time` datetime NULL DEFAULT NULL COMMENT '发送时间',
  `is_deleted` tinyint(1) UNSIGNED ZEROFILL NULL DEFAULT 0 COMMENT '逻辑删除 1（true）已删除， 0（false）未删除',
  PRIMARY KEY (`id`) USING BTREE,
  INDEX `idx_receive_time`(`send_time`) USING BTREE,
  INDEX `idx_device_id`(`device_key`) USING BTREE
) ENGINE = InnoDB CHARACTER SET = utf8mb4 COLLATE = utf8mb4_unicode_ci COMMENT = 'MQTT发送记录-指令表' ROW_FORMAT = Dynamic;

-- ----------------------------
-- Table structure for user_device_preference
-- ----------------------------
DROP TABLE IF EXISTS `user_device_preference`;
CREATE TABLE `user_device_preference`  (
  `id` bigint(20) UNSIGNED NOT NULL AUTO_INCREMENT COMMENT '主键ID',
  `user_code` varchar(32) CHARACTER SET utf8mb4 COLLATE utf8mb4_unicode_ci NOT NULL COMMENT '用户编码',
  `device_key` varchar(64) CHARACTER SET utf8mb4 COLLATE utf8mb4_unicode_ci NOT NULL COMMENT '设备唯一标识（用户自定义或系统生成）',
  PRIMARY KEY (`id`) USING BTREE
) ENGINE = InnoDB AUTO_INCREMENT = 7 CHARACTER SET = utf8mb4 COLLATE = utf8mb4_unicode_ci COMMENT = '设备表' ROW_FORMAT = Dynamic;

-- ----------------------------
-- Table structure for alarm_record
-- ----------------------------
DROP TABLE IF EXISTS `alarm_record`;
CREATE TABLE `alarm_record`  (
  `id` bigint(19) NOT NULL COMMENT '报警记录ID',
  `device_key` varchar(32) CHARACTER SET utf8mb4 COLLATE utf8mb4_general_ci NULL DEFAULT NULL COMMENT '设备标识',
  `alarm_type` varchar(16) CHARACTER SET utf8mb4 COLLATE utf8mb4_general_ci NOT NULL COMMENT '报警类型：TEMP/HUMI/GAS/FLAME',
  `alarm_level` tinyint(4) NOT NULL DEFAULT 1 COMMENT '等级：1预警 2紧急',
  `content` varchar(255) CHARACTER SET utf8mb4 COLLATE utf8mb4_general_ci NULL DEFAULT NULL COMMENT '报警内容',
  `status` tinyint(4) NOT NULL DEFAULT 0 COMMENT '状态：0报警中 1已恢复 2已处理',
  `handler` varchar(50) CHARACTER SET utf8mb4 COLLATE utf8mb4_general_ci NULL DEFAULT NULL COMMENT '处理人',
  `handle_time` datetime NULL DEFAULT NULL COMMENT '处理时间',
  `recover_time` datetime NULL DEFAULT NULL COMMENT '恢复时间',
  `is_deleted` tinyint(1) UNSIGNED NOT NULL DEFAULT 0 COMMENT '逻辑删除 1（true）已删除， 0（false）未删除',
  `gmt_create` datetime NOT NULL COMMENT '报警开始时间',
  PRIMARY KEY (`id`) USING BTREE,
  INDEX `idx_device_key`(`device_key`) USING BTREE,
  INDEX `idx_status`(`status`) USING BTREE,
  INDEX `idx_gmt_create`(`gmt_create`) USING BTREE
) ENGINE = InnoDB CHARACTER SET = utf8mb4 COLLATE = utf8mb4_general_ci COMMENT = '报警记录' ROW_FORMAT = DYNAMIC;

SET FOREIGN_KEY_CHECKS = 1;
