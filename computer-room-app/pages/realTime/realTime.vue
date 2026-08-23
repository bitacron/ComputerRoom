<template>
  <view class="container">
    <!-- 设备选择卡片 -->
    <view class="card device-card">
      <view class="card-header">
        <text>设备选择</text>
      </view>
      <view class="card-body">
        <picker
          mode="selector"
          :range="deviceOptions"
          :range-key="'label'"
          :value="selectedIndex"
          @change="onDevicePickerChange"
          :disabled="deviceLoading"
        >
          <view class="picker-display">
            <text>{{ selectedDeviceKey ? (selectedDeviceLabel || '请选择设备') : '请选择设备' }}</text>
            <text class="arrow">▼</text>
          </view>
        </picker>
      </view>
    </view>

    <!-- 空设备提示 -->
    <view v-if="deviceOptions.length === 0" class="card empty-card">
      <view class="card-body empty-device-wrapper">
        <text class="empty-icon">⚠️</text>
        <text class="empty-text">暂无可用设备，请先添加设备</text>
        <button class="btn btn-primary refresh-list-btn" @click="fetchDeviceList">刷新列表</button>
      </view>
    </view>

    <!-- 实时数据卡片（仅当有设备时显示） -->
    <view v-else class="card">
      <view class="card-header">
        <text>实时环境监测</text>
        <text class="refresh-btn" @click="fetchData" :class="{loading: loading}">⟳ 刷新</text>
      </view>
      <view class="card-body">
        <view v-if="!selectedDeviceKey" class="loading-mask">
          <text>请先选择设备</text>
        </view>
        <view v-else-if="loading && !environmentData" class="loading-mask">
          <text>加载中...</text>
        </view>
        <view v-else-if="error" class="error-mask">
          <text>{{ error }}</text>
          <text class="refresh-btn" @click="fetchData">重试</text>
        </view>
        <view v-else-if="!environmentData" class="loading-mask">
          <text>暂无数据</text>
        </view>
        <view v-else>

          <!-- 第一行：温度、湿度 -->
          <view class="data-row">
            <view class="info-item">
              <text class="label">温度：</text>
              <text class="value">{{ environmentData.temperature }} °C</text>
            </view>
            <view class="info-item">
              <text class="label">湿度：</text>
              <text class="value">{{ environmentData.humidity }} %</text>
            </view>
          </view>
          <!-- 第二行：气体浓度 + 状态 -->
          <view class="data-row">
            <view class="info-item">
              <text class="label">气体浓度：</text>
              <text class="value">{{ environmentData.gasPpm }} ppm</text>
            </view>
            <view class="info-item">
              <text class="label">气体状态：</text>
              <text :class="['tag', environmentData.gasStatus === 1 ? 'tag-danger' : 'tag-success']">
                {{ environmentData.gasStatus === 1 ? '异常' : '正常' }}
              </text>
            </view>
          </view>
          <!-- 第三行：光照状态 + 百分比 -->
          <view class="data-row">
            <view class="info-item">
              <text class="label">光照状态：</text>
              <text :class="['tag', environmentData.lightStatus === 1 ? 'tag-warning' : 'tag-info']">
                {{ environmentData.lightStatus === 1 ? '开启' : '关闭' }}
              </text>
            </view>
            <view class="info-item">
              <text class="label">光照百分比：</text>
              <text class="value">{{ environmentData.lightPercentage }} %</text>
            </view>
          </view>
          <!-- 第四行：火焰状态 + 百分比 -->
          <view class="data-row">
            <view class="info-item">
              <text class="label">火焰状态：</text>
              <text :class="['tag', environmentData.flameStatus === 1 ? 'tag-danger' : 'tag-success']">
                {{ environmentData.flameStatus === 1 ? '检测到火焰' : '无火焰' }}
              </text>
            </view>
            <view class="info-item">
              <text class="label">火焰百分比：</text>
              <text class="value">{{ environmentData.flamePercentage }} %</text>
            </view>
          </view>
          <!-- 第五行：告警状态 -->
          <view class="data-row">
            <view class="info-item">
              <text class="label">告警状态：</text>
              <text :class="['tag', environmentData.alarmStatus === 1 ? 'tag-danger' : 'tag-success']">
                {{ environmentData.alarmStatus === 1 ? '告警中' : '正常' }}
              </text>
            </view>
          </view>
          <!-- 第六行：散热器、LED -->
          <view class="data-row">
            <view class="info-item">
              <text class="label">散热器：</text>
              <text :class="['tag', environmentData.fanStatus === 1 ? 'tag-primary' : 'tag-info']">
                {{ environmentData.fanStatus === 1 ? '开启' : '关闭' }}
              </text>
            </view>
            <view class="info-item">
              <text class="label">LED：</text>
              <text :class="['tag', environmentData.ledStatus === 1 ? 'tag-warning' : 'tag-info']">
                {{ environmentData.ledStatus === 1 ? '开启' : '关闭' }}
              </text>
            </view>
          </view>
          <!-- 第七行：测量时间 -->
          <view class="data-row">
            <view class="info-item full-width">
              <text class="label">最新测量时间：</text>
              <text class="value">{{ environmentData.gmtMeasurement }}</text>
            </view>
          </view>
        </view>
      </view>
    </view>

    <!-- 设备反控卡片（暂不对小程序做按钮级权限，所有登录用户可用） -->
    <view v-if="deviceOptions.length > 0" class="card">
      <view class="card-header">
        <text>设备反控</text>
      </view>
      <view class="card-body">
        <!-- 散热器控制 -->
        <view class="control-row">
          <view class="control-icon">
            <image 
              :src="fanIcon" 
              class="fan-icon-img" 
              :class="environmentData && environmentData.fanStatus === 1 ? 'fan-animate' : 'fan-static'"
              mode="aspectFit"
            />
          </view>
          <view class="control-info">
            <view class="control-label">散热器</view>
            <view class="control-status">当前状态: {{ environmentData && environmentData.fanStatus === 1 ? '开启' : '关闭' }}</view>
            <view class="button-group">
              <button 
                class="btn btn-primary" 
                :disabled="fanLoading || (pendingCommand && pendingCommand.deviceType === 'fan') || (environmentData && environmentData.fanStatus === 1)"
                @click="controlRadiator('on')"
              >打开</button>
              <button 
                class="btn btn-danger" 
                :disabled="fanLoading || (pendingCommand && pendingCommand.deviceType === 'fan') || (environmentData && environmentData.fanStatus === 0)"
                @click="controlRadiator('off')"
              >关闭</button>
            </view>
          </view>
        </view>
        <!-- LED 控制 -->
        <view class="control-row">
          <view class="control-icon">
            <image 
              v-if="environmentData && environmentData.ledStatus === 1"
              :src="ledOnIcon" 
              class="led-icon-img led-on-glow"
              mode="aspectFit"
            />
            <image 
              v-else
              :src="ledOffIcon" 
              class="led-icon-img"
              mode="aspectFit"
            />
          </view>
          <view class="control-info">
            <view class="control-label">LED灯</view>
            <view class="control-status">当前状态: {{ environmentData && environmentData.ledStatus === 1 ? '开启' : '关闭' }}</view>
            <view class="button-group">
              <button 
                class="btn btn-primary" 
                :disabled="ledLoading || (pendingCommand && pendingCommand.deviceType === 'led') || (environmentData && environmentData.ledStatus === 1)"
                @click="controlLed('on')"
              >打开</button>
              <button 
                class="btn btn-danger" 
                :disabled="ledLoading || (pendingCommand && pendingCommand.deviceType === 'led') || (environmentData && environmentData.ledStatus === 0)"
                @click="controlLed('off')"
              >关闭</button>
            </view>
          </view>
        </view>
        <!-- 等待指令提示 -->
        <view v-if="pendingCommand" class="pending-tip">
          <text>⏳ 等待设备响应 {{ pendingCommand.deviceType === 'fan' ? '散热器' : 'LED' }} {{ pendingCommand.action === 'on' ? '开启' : '关闭' }} 指令...</text>
        </view>
      </view>
    </view>
  </view>
</template>

<script>
import request from '@/utils/request.js';
import { getWsUrl } from '@/utils/config.js';

export default {
  data() {
    return {
      // 图标路径
      fanIcon: '/static/icons/fan.png',
      ledOnIcon: '/static/icons/led-on.png',
      ledOffIcon: '/static/icons/led-off.png',
      // 设备相关
      deviceOptions: [],        // [{ value: 'deviceKey', label: 'deviceName - deviceKey' }]
      selectedDeviceKey: null,  // 当前选中的设备Key
      selectedIndex: -1,        // picker 选中索引
      selectedDeviceLabel: '',  // 当前显示的标签
      deviceLoading: false,     // 设备列表加载中

      environmentData: null,
      loading: false,
      error: null,

      // WebSocket 相关
      socketTask: null,
      connecting: false,
      reconnectTimer: null,
      reconnectAttempts: 0,
      stompFrameBuffer: '',

      // 反控等待确认
      pendingCommand: null,
      commandTimeout: null,
      fanLoading: false,
      ledLoading: false,
      pollingTimer: null
    };
  },
  onShow() {
    // 页面显示：加载设备列表
    this.init();
  },
  onHide() {
    this.disconnectWebSocket();
    this.stopPolling();
    if (this.reconnectTimer) clearTimeout(this.reconnectTimer);
    if (this.commandTimeout) clearTimeout(this.commandTimeout);
  },
  onUnload() {
    this.disconnectWebSocket();
    if (this.reconnectTimer) clearTimeout(this.reconnectTimer);
    if (this.commandTimeout) clearTimeout(this.commandTimeout);
  },
  methods: {
    // ==================== 初始化 ====================
    async init() {
      await this.fetchDeviceList();        // 获取设备列表
      await this.loadDefaultDevice();      // 获取默认设备
    },

    // ========== 设备列表相关 ==========
    async fetchDeviceList(keyword = '') {
      this.deviceLoading = true;
      try {
        const res = await request({
          url: '/service/device/getDeviceSelect',
          method: 'GET',
          data: keyword ? { keyword } : {}
        });
        if (res.code === 20000) {
          this.deviceOptions = (res.data || []).map(item => ({
            value: item.deviceKey,
            label: `${item.deviceName || '未命名'} (${item.deviceKey})`
          }));
        } else {
          uni.showToast({ title: res.msg || '获取设备列表失败', icon: 'none' });
        }
      } catch (err) {
        console.error('获取设备列表失败', err);
        uni.showToast({ title: '网络异常，获取设备列表失败', icon: 'none' });
      } finally {
        this.deviceLoading = false;
      }
    },

    // 从后端获取默认设备
    async loadDefaultDevice() {
      if (this.deviceOptions.length === 0) {
        this.selectedDeviceKey = null;
        this.selectedIndex = -1;
        this.selectedDeviceLabel = '';
        return;
      }

      try {
        const res = await request({
          url: '/service/device/default/getDefaultDevice',
          method: 'GET'
        });
        
        // 判断返回是否成功且有设备Key
        if (res.code === 20000 && res.data && res.data.deviceKey) {
          const defaultDeviceKey = res.data.deviceKey;
          // 检查该设备是否存在于当前设备列表中
          if (this.deviceOptions.some(device => device.value === defaultDeviceKey)) {
            this.selectedDeviceKey = defaultDeviceKey;
            this.selectedIndex = this.deviceOptions.findIndex(d => d.value === defaultDeviceKey);
            this.selectedDeviceLabel = this.deviceOptions[this.selectedIndex]?.label || '';
            this.onDeviceChange(this.selectedDeviceKey);
            return;
          }
        }
      } catch (err) {
        console.error('获取默认设备失败', err);
      }
      
      // 如果默认设备获取失败或设备不存在，使用设备列表第一个
      if (this.deviceOptions.length > 0) {
        const firstDevice = this.deviceOptions[0].value;
        this.selectedDeviceKey = firstDevice;
        this.selectedIndex = 0;
        this.selectedDeviceLabel = this.deviceOptions[0].label;
        this.onDeviceChange(this.selectedDeviceKey);
        // 异步保存为默认设备（首次使用）
        this.setDefaultDevice(firstDevice);
      } else {
        this.selectedDeviceKey = null;
        this.selectedIndex = -1;
        this.selectedDeviceLabel = '';
      }
    },

    // 保存默认设备到后端
    async setDefaultDevice(deviceKey) {
      if (!deviceKey) return;
      try {
        await request({
          url: '/service/device/default/setDefaultDevice',
          method: 'POST',
          data: { deviceKey }
        });
      } catch (err) {
        console.error('保存默认设备失败', err);
        // 静默失败，不影响主要功能
      }
    },

    // picker 选择变化
    onDevicePickerChange(e) {
      const idx = e.detail.value;
      if (idx >= 0 && idx < this.deviceOptions.length) {
        const selected = this.deviceOptions[idx];
        this.selectedDeviceKey = selected.value;
        this.selectedIndex = idx;
        this.selectedDeviceLabel = selected.label;
        this.onDeviceChange(this.selectedDeviceKey);
      } else {
        // 清空选择
        this.selectedDeviceKey = null;
        this.selectedIndex = -1;
        this.selectedDeviceLabel = '';
        this.onDeviceChange(null);
      }
    },

    // 切换设备时的核心逻辑
    onDeviceChange(deviceKey) {
      if (!deviceKey) {
        // 清空所有数据，断开连接
        this.environmentData = null;
        this.error = null;
        this.disconnectWebSocket();
        this.stopPolling();
        return;
      }
      
      // 保存为默认设备（后端同步）
      this.setDefaultDevice(deviceKey);

      // 重置状态
      this.environmentData = null;
      this.error = null;

      // 断开旧连接
      this.disconnectWebSocket();

      // 获取新设备数据（HTTP）
      this.fetchData();

      // 建立新 WebSocket 连接（订阅该设备主题）
      this.connectWebSocket();
    },

    // ========== 数据获取 ==========
    async fetchData() {
      if (!this.selectedDeviceKey) return;
      this.loading = true;
      this.error = null;
      try {
        const res = await request({
          url: '/service/environment/getLastEnvironment',
          method: 'GET',
          data: { deviceKey: this.selectedDeviceKey }
        });
        if (res && res.data) {
          this.applyEnvironmentPush(res.data);
        } else {
          // 接口返回成功但无数据
          this.environmentData = null;
          this.error = null;
        }
      } catch (err) {
        console.error('获取实时数据失败', err);
        this.error = err.msg || '获取数据失败，请检查网络';
        this.environmentData = null;
      } finally {
        this.loading = false;
      }
    },

    // ========== WebSocket 连接与订阅 ==========
    connectWebSocket() {
      if (!this.selectedDeviceKey) return;
      if (this.socketTask && this.socketTask.readyState === 1) {
        console.log('WebSocket 已连接，无需重复连接');
        return;
      }
      if (this.connecting) return;

      this.connecting = true;
      this.error = null;

      this.socketTask = uni.connectSocket({
        url: getWsUrl(),
        success: () => console.log('WebSocket 连接创建成功'),
        fail: (err) => {
          console.error('WebSocket 连接创建失败', err);
          this.handleConnectionError();
        }
      });

      this.socketTask.onOpen(() => {
        console.log('WebSocket 连接已打开');
        this.connecting = false;
        this.reconnectAttempts = 0;
        if (this.socketTask) {
          this.sendStompConnect();
        }
        this.stopPolling();
      });

      this.socketTask.onMessage((res) => {
        this.handleStompMessage(res.data);
      });

      this.socketTask.onError((err) => {
        console.error('WebSocket 错误', err);
        this.handleConnectionError();
        this.startPolling();
      });

      this.socketTask.onClose(() => {
        console.log('WebSocket 连接已关闭');
        this.socketTask = null;
        this.connecting = false;
        this.startPolling();
        if (this.reconnectTimer === null) {
          this.reconnectWebSocket();
        }
      });
    },

    sendStompConnect() {
      if (!this.socketTask) return;
      const connectFrame = 'CONNECT\naccept-version:1.1,1.0\nheart-beat:10000,10000\n\n\0';
      this.socketTask.send({ data: connectFrame, fail: (err) => console.error('发送 CONNECT 失败', err) });
    },

    sendStompSubscribe() {
      if (!this.socketTask) return;
      const envTopic = `/topic/environment/${this.selectedDeviceKey}`;
      const envFrame = `SUBSCRIBE\nid:sub-0\ndestination:${envTopic}\nack:auto\n\n\0`;
      this.socketTask.send({ data: envFrame, fail: (err) => console.error('发送 SUBSCRIBE 失败', err) });
      const cmdTopic = `/topic/command/${this.selectedDeviceKey}`;
      const cmdFrame = `SUBSCRIBE\nid:sub-1\ndestination:${cmdTopic}\nack:auto\n\n\0`;
      this.socketTask.send({ data: cmdFrame, fail: (err) => console.error('发送指令订阅失败', err) });
    },

    handleStompMessage(data) {
      this.stompFrameBuffer += data;
      let nullIndex;
      while ((nullIndex = this.stompFrameBuffer.indexOf('\0')) !== -1) {
        const frameStr = this.stompFrameBuffer.substring(0, nullIndex);
        this.stompFrameBuffer = this.stompFrameBuffer.substring(nullIndex + 1);
        if (frameStr.trim().length === 0) continue;

        const lines = frameStr.split('\n');
        const command = lines[0];

        if (command === 'CONNECTED') {
          console.log('STOMP 连接成功，发送订阅');
          this.sendStompSubscribe();
        } else if (command === 'MESSAGE') {
          const bodyStartIndex = frameStr.indexOf('\n\n');
          if (bodyStartIndex !== -1) {
            let body = frameStr.substring(bodyStartIndex + 2);
            try {
              const messageData = JSON.parse(body);
              if (messageData.cmdId && messageData.status != null) {
                this.handleCommandResult(messageData);
              } else {
                this.applyEnvironmentPush(messageData);
                if (this.error) this.error = null;
              }
            } catch (e) {
              console.error('解析推送 JSON 失败', e, body);
            }
          }
        } else if (command === 'ERROR') {
          console.error('STOMP 错误帧:', frameStr);
          this.error = '实时数据连接出错，将尝试重连';
          this.disconnectWebSocket();
          this.reconnectWebSocket();
        }
      }
    },

    applyEnvironmentPush(data) {
      if (!data) return;
      if (this.pendingCommand) {
        const expected = this.pendingCommand.action === 'on' ? 1 : 0;
        const actual = this.pendingCommand.deviceType === 'fan' ? data.fanStatus : data.ledStatus;
        if (actual === expected) {
          this.environmentData = data;
          this.handleCommandResult({
            cmdId: this.pendingCommand.cmdId,
            status: 2,
            fan: data.fanStatus,
            led: data.ledStatus
          });
          return;
        }
        if (this.environmentData) {
          if (this.pendingCommand.deviceType === 'fan') {
            data.fanStatus = this.environmentData.fanStatus;
          } else if (this.pendingCommand.deviceType === 'led') {
            data.ledStatus = this.environmentData.ledStatus;
          }
        }
      }
      this.environmentData = data;
    },

    handleCommandResult(cmd) {
      if (!cmd || !this.pendingCommand || cmd.cmdId !== this.pendingCommand.cmdId) {
        return;
      }
      const { deviceType, action } = this.pendingCommand;
      const deviceName = deviceType === 'fan' ? '散热器' : 'LED';
      if (this.commandTimeout) {
        clearTimeout(this.commandTimeout);
        this.commandTimeout = null;
      }
      if (cmd.status === 2) {
        if (this.environmentData) {
          if (cmd.fan !== undefined && cmd.fan !== null) this.environmentData.fanStatus = cmd.fan;
          if (cmd.led !== undefined && cmd.led !== null) this.environmentData.ledStatus = cmd.led;
        }
        uni.showToast({ title: `${deviceName}${action === 'on' ? '开启' : '关闭'}成功`, icon: 'success' });
      } else if (cmd.status === 3) {
        uni.showToast({ title: `${deviceName}执行失败`, icon: 'none' });
      } else if (cmd.status === 4) {
        uni.showToast({ title: '设备未响应', icon: 'none' });
      }
      this.pendingCommand = null;
      this.fanLoading = false;
      this.ledLoading = false;
    },

    handleConnectionError() {
      this.error = '实时数据连接失败，正在尝试重连...';
      this.connecting = false;
      if (this.socketTask) {
        try { this.socketTask.close({}); } catch (e) {}
        this.socketTask = null;
      }
      this.reconnectWebSocket();
    },

    reconnectWebSocket() {
      if (this.reconnectTimer) return;
      const delay = Math.min(1000 * Math.pow(2, this.reconnectAttempts), 30000);
      console.log(`将在 ${delay}ms 后尝试重连 WebSocket`);
      this.reconnectTimer = setTimeout(() => {
        this.reconnectAttempts++;
        this.connectWebSocket();
        this.reconnectTimer = null;
      }, delay);
    },

    disconnectWebSocket() {
      if (this.socketTask) {
        try {
          const disconnectFrame = 'DISCONNECT\n\n\0';
          this.socketTask.send({ data: disconnectFrame, fail: () => {} });
          this.socketTask.close({});
        } catch (e) {}
        this.socketTask = null;
      }
      this.connecting = false;
      this.stompFrameBuffer = '';
      if (this.reconnectTimer) clearTimeout(this.reconnectTimer);
    },

    startPolling() {
      if (this.pollingTimer) return;
      console.log('启用轮询降级');
      this.pollingTimer = setInterval(() => {
        if (!this.loading && this.selectedDeviceKey) {
          this.fetchData();
        }
      }, 5000);
    },

    stopPolling() {
      if (this.pollingTimer) {
        clearInterval(this.pollingTimer);
        this.pollingTimer = null;
        console.log('停止轮询');
      }
    },

    // ========== 设备反控 ==========
    async controlRadiator(action) {
      if (!this.selectedDeviceKey) {
        uni.showToast({ title: '请先选择设备', icon: 'none' });
        return;
      }
      if (this.pendingCommand) {
        uni.showToast({ title: '请等待上一次指令执行完成', icon: 'none' });
        return;
      }
      this.fanLoading = true;
      try {
        const res = await request({
          url: '/service/deviceOption/control',
          method: 'POST',
          data: {
            deviceType: 'fan',
            deviceKey: this.selectedDeviceKey,
            command: action
          },
          timeout: 3000
        });
        if (res.code === 20000 && res.data && res.data.cmdId) {
          this.pendingCommand = { deviceType: 'fan', action, cmdId: res.data.cmdId };
          uni.showToast({ title: '指令已发送，等待设备响应...', icon: 'none', duration: 2000 });
          this.commandTimeout = setTimeout(() => {
            if (this.pendingCommand && this.pendingCommand.cmdId === res.data.cmdId) {
              uni.showToast({ title: '设备未响应', icon: 'none' });
              this.pendingCommand = null;
              this.fanLoading = false;
              this.commandTimeout = null;
            }
          }, 8000);
        } else {
          uni.showToast({ title: res.msg || '操作失败', icon: 'none' });
          this.fanLoading = false;
        }
      } catch (err) {
        uni.showToast({ title: err.msg || '控制超时或失败', icon: 'none' });
        this.fanLoading = false;
      }
    },

    async controlLed(action) {
      if (!this.selectedDeviceKey) {
        uni.showToast({ title: '请先选择设备', icon: 'none' });
        return;
      }
      if (this.pendingCommand) {
        uni.showToast({ title: '请等待上一次指令执行完成', icon: 'none' });
        return;
      }
      this.ledLoading = true;
      try {
        const res = await request({
          url: '/service/deviceOption/control',
          method: 'POST',
          data: {
            deviceType: 'led',
            deviceKey: this.selectedDeviceKey,
            command: action
          },
          timeout: 3000
        });
        if (res.code === 20000 && res.data && res.data.cmdId) {
          this.pendingCommand = { deviceType: 'led', action, cmdId: res.data.cmdId };
          uni.showToast({ title: '指令已发送，等待设备响应...', icon: 'none', duration: 2000 });
          this.commandTimeout = setTimeout(() => {
            if (this.pendingCommand && this.pendingCommand.cmdId === res.data.cmdId) {
              uni.showToast({ title: '设备未响应', icon: 'none' });
              this.pendingCommand = null;
              this.ledLoading = false;
              this.commandTimeout = null;
            }
          }, 8000);
        } else {
          uni.showToast({ title: res.msg || '操作失败', icon: 'none' });
          this.ledLoading = false;
        }
      } catch (err) {
        uni.showToast({ title: err.msg || '控制超时或失败', icon: 'none' });
        this.ledLoading = false;
      }
    }
  }
};
</script>

<style scoped>
/* 全局变量 */
:root {
  --primary-color: #409eff;
  --success-color: #67c23a;
  --warning-color: #e6a23c;
  --danger-color: #f56c6c;
  --info-color: #909399;
  --card-bg: #ffffff;
  --body-bg: #f5f7fa;
  --border-radius-lg: 24rpx;
  --border-radius-md: 16rpx;
  --box-shadow: 0 8rpx 32rpx rgba(0, 0, 0, 0.08);
}

.container {
  padding: 20rpx;
  background-color: var(--body-bg);
  min-height: 100vh;
  box-sizing: border-box;
}

/* 卡片通用样式 */
.card {
  background-color: var(--card-bg);
  border-radius: var(--border-radius-lg);
  margin-bottom: 24rpx;
  box-shadow: var(--box-shadow);
  overflow: hidden;
  transition: all 0.3s ease;
}

.card-header {
  padding: 24rpx 32rpx;
  border-bottom: 1rpx solid #f0f0f0;
  font-size: 32rpx;
  font-weight: 600;
  color: #303133;
  display: flex;
  justify-content: space-between;
  align-items: center;
}

.card-body {
  padding: 24rpx 32rpx 32rpx;
}

/* 设备选择器 */
.device-card {
  margin-bottom: 20rpx;
}

.picker-display {
  display: flex;
  justify-content: space-between;
  align-items: center;
  background-color: #f5f5f5;
  padding: 20rpx;
  border-radius: 12rpx;
  font-size: 28rpx;
}

.arrow {
  font-size: 24rpx;
  color: #999;
}

/* 刷新按钮 */
.refresh-btn {
  font-size: 28rpx;
  color: var(--primary-color);
  display: flex;
  align-items: center;
  gap: 8rpx;
  background: none;
  border: none;
  padding: 8rpx 16rpx;
  border-radius: 40rpx;
  transition: all 0.2s;
}

.refresh-btn:active {
  background-color: rgba(64, 158, 255, 0.1);
  transform: scale(0.96);
}

.refresh-btn.loading {
  opacity: 0.6;
  pointer-events: none;
}

/* 空设备提示 */
.empty-card {
  margin-top: 20rpx;
}

.empty-device-wrapper {
  display: flex;
  flex-direction: column;
  align-items: center;
  justify-content: center;
  padding: 60rpx 0;
}

.empty-icon {
  font-size: 80rpx;
  margin-bottom: 20rpx;
  color: #e6a23c;
}

.empty-text {
  font-size: 28rpx;
  color: #909399;
  margin-bottom: 30rpx;
}

.refresh-list-btn {
  width: 200rpx;
  margin-top: 20rpx;
}

/* 数据行：使用 Grid 布局，响应式 */
.data-row {
  display: grid;
  grid-template-columns: repeat(2, 1fr);
  gap: 24rpx;
  margin-bottom: 24rpx;
}

@media (max-width: 500rpx) {
  .data-row {
    grid-template-columns: 1fr;
  }
}

.info-item {
  display: flex;
  align-items: center;
  background-color: #fafbfc;
  border-radius: var(--border-radius-md);
  padding: 20rpx 24rpx;
  transition: all 0.2s;
}

.info-item .label {
  width: 120rpx;
  font-size: 28rpx;
  color: #606266;
  font-weight: 500;
}

.info-item .value {
  flex: 1;
  font-size: 30rpx;
  font-weight: 600;
  color: #303133;
}

.full-width {
  grid-column: 1 / -1;
}

/* 标签样式 */
.tag {
  display: inline-block;
  padding: 6rpx 20rpx;
  font-size: 24rpx;
  border-radius: 32rpx;
  background-color: #f4f4f5;
  color: #909399;
  font-weight: 500;
  text-align: center;
  min-width: 100rpx;
}

.tag-danger {
  background-color: #fef0f0;
  color: var(--danger-color);
}

.tag-success {
  background-color: #f0f9eb;
  color: var(--success-color);
}

.tag-warning {
  background-color: #fdf6ec;
  color: var(--warning-color);
}

.tag-primary {
  background-color: #ecf5ff;
  color: var(--primary-color);
}

.tag-info {
  background-color: #f4f4f5;
  color: var(--info-color);
}

/* 控制项布局 */
.control-row {
  display: flex;
  align-items: center;
  padding: 24rpx 0;
  border-bottom: 1rpx solid #f0f0f0;
}

.control-row:last-child {
  border-bottom: none;
}

.control-icon {
  width: 100rpx;
  text-align: center;
  font-size: 60rpx;
  margin-right: 24rpx;
}

.control-info {
  flex: 1;
}

.control-label {
  font-size: 32rpx;
  font-weight: 600;
  color: #303133;
  margin-bottom: 8rpx;
}

.control-status {
  font-size: 26rpx;
  color: #909399;
  margin-bottom: 20rpx;
}

/* 按钮组 */
.button-group {
  display: flex;
  gap: 20rpx;
}

.btn {
  flex: 1;
  padding: 16rpx 0;
  border-radius: 80rpx;
  font-size: 28rpx;
  font-weight: 500;
  text-align: center;
  background-color: #ffffff;
  border: 1px solid #dcdfe6;
  color: #606266;
  transition: all 0.2s;
}

.btn-primary {
  background: linear-gradient(135deg, #409eff, #66b1ff);
  border: none;
  color: white;
  box-shadow: 0 4rpx 12rpx rgba(64, 158, 255, 0.3);
}

.btn-danger {
  background: linear-gradient(135deg, #f56c6c, #f78989);
  border: none;
  color: white;
  box-shadow: 0 4rpx 12rpx rgba(245, 108, 108, 0.3);
}

.btn:active {
  transform: scale(0.96);
  opacity: 0.9;
}

button[disabled] {
  opacity: 0.5;
  transform: none;
  pointer-events: none;
}

/* 风扇图片样式 */
.fan-icon-img {
  width: 60rpx;
  height: 60rpx;
  display: inline-block;
}

.fan-animate {
  animation: spin 1s linear infinite;
}

.fan-static {
  opacity: 0.5;
  filter: grayscale(100%);
}

/* LED图片样式 */
.led-icon-img {
  width: 60rpx;
  height: 60rpx;
  display: inline-block;
}

.led-on-glow {
  filter: drop-shadow(0 0 8rpx #ffb347);
}

@keyframes spin {
  from { transform: rotate(0deg); }
  to { transform: rotate(360deg); }
}

/* 加载与错误状态 */
.loading-mask, .error-mask {
  text-align: center;
  padding: 80rpx 32rpx;
  color: var(--info-color);
  font-size: 28rpx;
  display: flex;
  flex-direction: column;
  align-items: center;
  gap: 20rpx;
}

.error-mask {
  color: var(--danger-color);
}

.error-mask .refresh-btn {
  background-color: var(--danger-color);
  color: white;
  padding: 12rpx 32rpx;
  border-radius: 80rpx;
  margin-top: 16rpx;
}

/* 等待指令提示 */
.pending-tip {
  margin-top: 20rpx;
  padding: 16rpx;
  background-color: #fdf6ec;
  border-radius: var(--border-radius-md);
  text-align: center;
  font-size: 26rpx;
  color: var(--warning-color);
}
</style>