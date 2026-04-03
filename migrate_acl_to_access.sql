-- ============================================================
-- 迁移脚本：acl_* → access_*（去尚硅谷化）
-- 适用：已有 computer_room 库中仍使用 acl_* 表名的环境
-- 执行前请备份数据库
-- ============================================================

SET NAMES utf8mb4;

-- 1. 重命名表（注意依赖顺序：先关联表再主表亦可，RENAME 可批量原子执行）
RENAME TABLE
  `acl_role_permission` TO `access_role_permission`,
  `acl_user_role` TO `access_user_role`,
  `acl_permission` TO `access_permission`,
  `acl_role` TO `access_role`,
  `acl_user` TO `access_user`;

-- 2. 更新权限菜单中的路由 / 组件 / 图标 / 权限码
-- 2.1 顶级「权限管理」菜单：path、icon
UPDATE `access_permission`
SET `path` = '/access',
    `icon` = 'access'
WHERE `path` = '/acl'
   OR `icon` = 'acl';

-- 2.2 组件路径：/acl/... → /access/...
UPDATE `access_permission`
SET `component` = REPLACE(`component`, '/acl/', '/access/')
WHERE `component` LIKE '/acl/%';

-- 2.3 访问路径中若仍含 /acl（兜底）
UPDATE `access_permission`
SET `path` = REPLACE(`path`, '/acl', '/access')
WHERE `path` LIKE '%/acl%';

-- 2.4 角色权限按钮权限码：role.acl → role.access
UPDATE `access_permission`
SET `permission_value` = 'role.access'
WHERE `permission_value` = 'role.acl';

-- 完成。部署新代码后建议清理 Redis 中用户权限缓存（key 为用户名），或重新登录。
