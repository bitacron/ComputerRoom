-- 漏水字段增量脚本（已有库执行本文件即可，不要重复执行）
-- 库名：computer_room

ALTER TABLE `device`
  ADD COLUMN `leak_status` tinyint(4) NULL DEFAULT NULL COMMENT '当前漏水状态（1：有积水；0：无积水）' AFTER `flame_percentage`,
  ADD COLUMN `leak_percentage` decimal(5, 2) NULL DEFAULT NULL COMMENT '当前积水程度百分比（0-100）' AFTER `leak_status`;

ALTER TABLE `environment`
  ADD COLUMN `leak_status` tinyint(4) NULL DEFAULT NULL COMMENT '漏水传感器状态（1：有积水；0：无积水）' AFTER `flame_percentage`,
  ADD COLUMN `leak_percentage` decimal(5, 2) NULL DEFAULT NULL COMMENT '积水程度百分比（0-100）' AFTER `leak_status`;

ALTER TABLE `alarm_record`
  MODIFY COLUMN `alarm_type` varchar(16) CHARACTER SET utf8mb4 COLLATE utf8mb4_general_ci NOT NULL COMMENT '报警类型：TEMP/HUMI/GAS/FLAME/LEAK';
