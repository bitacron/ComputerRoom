# 机房监控系统（CRMS）

本项目是一套面向中小型机房 / 数据中心的物联网监控系统，采用「端—边—云—端」的典型 IoT 架构，由 **固件（firmware）**、**服务端（server）**、**Web 后台（web）**、**移动 APP（app）** 四个模块协同组成，实现机房环境数据的实时采集、上传、存储、可视化与远程控制。

## 四大模块

### 1. firmware — 设备固件层

目录：`computer-room-firmware`

运行于 **STM32F10x** 单片机上的嵌入式程序，负责采集温湿度、烟雾、光照、火焰等传感器数据，通过 **ESP8266-01** Wi-Fi 模组接入网络。使用 cJSON / core_json 组包，经 MQTT 将数据上报至 Broker，并接收下发指令（散热器继电器、LED 等），是整个系统的数据源头与执行末端。

主要感知与执行：DHT11 温湿度、MQ2 烟雾、LDR 光敏、火焰传感器、蜂鸣器告警、继电器反控散热器、LED、OLED 显示。

### 2. server — 后端服务层

目录：`computer-room-server`

基于 **Spring Boot 2.7 + MyBatis-Plus + MySQL** 构建的业务服务。集成 Spring Security + JWT 完成鉴权、Redis 缓存会话与热点权限、spring-integration-mqtt 订阅设备消息、WebSocket 向前端实时推送。按环境监控、设备管理、远程控制、MQTT、权限（Access）等模块划分，对外提供 REST API 与 Knife4j 接口文档。

### 3. web — 管理后台

目录：`computer-room-web`

基于 vue-admin-template + Vue 2 + Element UI 的 PC 端管理界面。面向运维人员，提供登录鉴权、动态路由、实时环境看板、历史环境数据、设备列表、反控指令、MQTT 收发记录、数据统计与 RBAC 权限管理，是机房统一监控与运营管理的主入口。

### 4. app — 移动前台

目录：`computer-room-app`

基于 **uni-app**（Vue 2）跨端框架开发的移动端应用。提供登录、实时监测、历史数据、个人中心与远程操作，便于管理人员随时随地查看机房状态。

## 数据链路

```
传感器 → STM32(固件) → ESP8266 → MQTT/EMQX → 服务端 → WebSocket/REST → Web 后台 / 移动 APP
                                              ↑←←←← 控制指令下发 ←←←←←←
```

## 仓库结构

```
ComputerRoomMonitoringSystem/
├── computer-room-firmware/   # STM32 固件
├── computer-room-server/     # Spring Boot 后端
├── computer-room-web/        # Vue 管理后台
├── computer-room-app/        # uni-app 移动端
└── computer_room.sql         # 全库建表与示例数据
```

## 运行环境与版本

以下版本来自各工程内部配置（`pom.xml`、`computer_room.sql`、`package.json`、`application.yaml`、`manifest.json` 等）。未在仓库中锁定小版本的组件，给出建议区间。

| 组件 | 版本 | 说明 |
|------|------|------|
| JDK | **8**（1.8） | `computer-room-server/pom.xml` 中 `java.version` |
| Spring Boot | **2.7.18** | 最后一个完整支持 JDK 8 的 2.x 系列 |
| MyBatis-Plus | 3.5.0 | 数据访问 |
| MySQL | **5.7.25** | SQL 导出源库版本；驱动 `mysql-connector-java 5.1.47` |
| Redis | **6.x**（兼容 5.x / 7.x） | 仓库未锁定小版本；默认 `localhost:6379`，库 0 |
| EMQX | **4.4.x / 5.x** | MQTT Broker，仓库未锁定小版本；默认 `tcp://localhost:1883` |
| Node.js | **14.x** | Vue CLI 5 要求 `^12 \|\| >=14`；前端锁定 `sass@1.32.13`，建议 14 LTS |
| npm | **6.x+** | 与 Node 14 配套 |
| Vue | **2.6.14** | `computer-room-web` |
| Vue CLI | **5.0** | `@vue/cli-service ~5.0.0` |
| Element UI | **2.12.0** | Web 后台 UI |
| uni-app | Vue 2 / compilerVersion 3 | `computer-room-app/manifest.json` |
| 单片机 | STM32F10x + ESP8266-01 | Keil 工程 |

本地默认连接（见 `computer-room-server/src/main/resources/application.yaml`）：

- 后端端口：`8080`
- 数据库：`computer_room`（脚本：根目录 `computer_room.sql` 或 `computer-room-server/computer_room.sql`、`access.sql`）
- MQTT：默认关闭（`mqtt.enabled: false`），启用后连本机 `1883`
- Web 开发代理：`computer-room-web/.env.development` 中 `BASE_API=http://localhost:8080`

## 快速启动（开发）

1. 安装并启动 **MySQL 5.7**、**Redis**；如需联调设备，再启动 **EMQX**（1883）。
2. 导入 SQL，创建库 `computer_room`。
3. 使用 **JDK 8** 启动 `computer-room-server`（Maven / IDE）。
4. 使用 **Node 14**：`cd computer-room-web && npm install && npm run serve`。
5. 用 HBuilderX 打开 `computer-room-app`，运行到微信开发者工具或 App。
6. 固件用 Keil 打开 `computer-room-firmware`，编译烧录 STM32。
