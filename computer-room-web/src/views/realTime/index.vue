<template>
  <div class="real-time-container">
    <!-- 设备选择卡片 -->
    <el-card class="device-selector-card" shadow="never">
      <el-form :inline="true" label-width="80px">
        <el-form-item label="设备标识">
          <el-select
            v-model="selectedDeviceKey"
            clearable
            placeholder="请选择设备"
            filterable
            remote
            reserve-keyword
            :remote-method="searchDevices"
            :loading="deviceLoading"
            :disabled="deviceOptions.length === 0"
            @change="onDeviceChange"
          >
            <el-option
              v-for="item in deviceOptions"
              :key="item.value"
              :label="item.label"
              :value="item.value"
            />
          </el-select>
        </el-form-item>
      </el-form>
    </el-card>

    <!-- 空设备提示 -->
    <el-card v-if="deviceOptions.length === 0" class="data-card" shadow="hover">
      <div class="empty-device-wrapper">
        <i class="el-icon-warning-outline" style="font-size: 48px; color: #E6A23C;" />
        <p style="margin-top: 16px; color: #909399;">暂无可用设备，请先添加设备</p>
        <el-button type="primary" size="small" @click="fetchDeviceList">刷新列表</el-button>
      </div>
    </el-card>

    <!-- 实时数据卡片 -->
    <el-card v-else class="data-card" shadow="hover">
      <div slot="header" class="clearfix">
        <span>实时环境监测</span>
        <el-button
          :loading="loading"
          style="float: right; padding: 3px 0"
          type="text"
          @click="fetchData"
        >
          刷新
        </el-button>
      </div>

      <!-- 未选择设备 -->
      <div v-if="!selectedDeviceKey" class="loading-wrapper">
        请先选择设备
      </div>

      <!-- 加载中 -->
      <div v-else-if="loading && !environmentData" class="loading-wrapper">
        <i class="el-icon-loading"/> 加载中...
      </div>

      <!-- 错误提示 -->
      <div v-else-if="error" class="error-wrapper">
        <el-alert :title="error" :closable="false" type="error" show-icon/>
      </div>

      <!-- 暂无数据 -->
      <div v-else-if="!environmentData" class="loading-wrapper">
        暂无数据
      </div>

      <!-- 数据显示 -->
      <div v-else class="data-content">
        <!-- 第1行：温度、湿度 -->
        <div class="data-row">
          <div class="info-item">
            <span class="label">温度：</span>
            <span class="value">{{ environmentData.temperature }} °C</span>
          </div>
          <div class="info-item">
            <span class="label">湿度：</span>
            <span class="value">{{ environmentData.humidity }} %</span>
          </div>
        </div>

        <!-- 第2行：气体浓度、气体状态 -->
        <div class="data-row">
          <div class="info-item">
            <span class="label">气体浓度：</span>
            <span class="value">{{ environmentData.gasPpm }} ppm</span>
          </div>
          <div class="info-item">
            <span class="label">气体状态：</span>
            <el-tag :type="environmentData.gasStatus === 1 ? 'danger' : 'success'" size="small">
              {{ environmentData.gasStatus === 1 ? '异常' : '正常' }}
            </el-tag>
          </div>
        </div>

        <!-- 第3行：光照状态、光照百分比 -->
        <div class="data-row">
          <div class="info-item">
            <span class="label">光照状态：</span>
            <el-tag :type="environmentData.lightStatus === 1 ? 'warning' : 'info'" size="small">
              {{ environmentData.lightStatus === 1 ? '开启' : '关闭' }}
            </el-tag>
          </div>
          <div class="info-item">
            <span class="label">光照百分比：</span>
            <span class="value">{{ environmentData.lightPercentage }} %</span>
          </div>
        </div>

        <!-- 第4行：火焰状态、火焰百分比 -->
        <div class="data-row">
          <div class="info-item">
            <span class="label">火焰状态：</span>
            <el-tag :type="environmentData.flameStatus === 1 ? 'danger' : 'success'" size="small">
              {{ environmentData.flameStatus === 1 ? '检测到火焰' : '无火焰' }}
            </el-tag>
          </div>
          <div class="info-item">
            <span class="label">火焰百分比：</span>
            <span class="value">{{ environmentData.flamePercentage }} %</span>
          </div>
        </div>

        <!-- 第5行：告警状态 -->
        <div class="data-row">
          <div class="info-item">
            <span class="label">告警状态：</span>
            <el-tag :type="environmentData.alarmStatus === 1 ? 'danger' : 'success'" size="small">
              {{ environmentData.alarmStatus === 1 ? '告警中' : '正常' }}
            </el-tag>
          </div>
        </div>

        <!-- 第6行：散热器、LED -->
        <div class="data-row">
          <div class="info-item">
            <span class="label">散热器：</span>
            <el-tag :type="environmentData.fanStatus === 1 ? 'primary' : 'info'" size="small">
              {{ environmentData.fanStatus === 1 ? '开启' : '关闭' }}
            </el-tag>
          </div>
          <div class="info-item">
            <span class="label">LED：</span>
            <el-tag :type="environmentData.ledStatus === 1 ? 'primary' : 'info'" size="small">
              {{ environmentData.ledStatus === 1 ? '开启' : '关闭' }}
            </el-tag>
          </div>
        </div>

        <!-- 第7行：测量时间 -->
        <div class="data-row timestamp">
          <div class="info-item full-width">
            <span class="label">最新测量时间：</span>
            <span class="value">{{ environmentData.gmtMeasurement }}</span>
          </div>
        </div>
      </div>
    </el-card>

    <!-- 反控卡片 -->
    <el-card v-if="deviceOptions.length > 0" class="control-card" shadow="hover">
      <div slot="header">
        <span>设备反控</span>
      </div>
      <el-row :gutter="20">
        <!-- 散热器控制 -->
        <el-col :span="12">
          <div class="control-item">
            <div class="control-icon">
              <svg-icon :class="[environmentData && environmentData.fanStatus === 1 ? 'fan-on' : 'fan-off']" icon-class="fan" />
            </div>
            <div class="control-info">
              <div class="control-label">散热器</div>
              <div class="control-status">当前状态: {{ fanStatusText }}</div>
              <div class="control-buttons">
                <el-button
                  :loading="fanLoading"
                  :disabled="isActuatorPending('fan') || (environmentData && environmentData.fanStatus === 1)"
                  size="small"
                  type="primary"
                  @click="controlRadiator('on')"
                >
                  打开
                </el-button>
                <el-button
                  :loading="fanLoading"
                  :disabled="isActuatorPending('fan') || (environmentData && environmentData.fanStatus === 0)"
                  size="small"
                  type="danger"
                  @click="controlRadiator('off')"
                >
                  关闭
                </el-button>
              </div>
            </div>
          </div>
        </el-col>
        <!-- LED 控制 -->
        <el-col :span="12">
          <div class="control-item">
            <div class="control-icon">
              <svg-icon v-if="environmentData && environmentData.ledStatus === 1" icon-class="led-on" class="led-on" />
              <svg-icon v-else icon-class="led-off" />
            </div>
            <div class="control-info">
              <div class="control-label">LED灯</div>
              <div class="control-status">当前状态: {{ ledStatusText }}</div>
              <div class="control-buttons">
                <el-button
                  :loading="ledLoading"
                  :disabled="isActuatorPending('led') || (environmentData && environmentData.ledStatus === 1)"
                  size="small"
                  type="primary"
                  @click="controlLed('on')"
                >
                  打开
                </el-button>
                <el-button
                  :loading="ledLoading"
                  :disabled="isActuatorPending('led') || (environmentData && environmentData.ledStatus === 0)"
                  size="small"
                  type="danger"
                  @click="controlLed('off')"
                >
                  关闭
                </el-button>
              </div>
            </div>
          </div>
        </el-col>
      </el-row>
      <div v-if="pending" class="pending-tip">
        等待设备响应 {{ pending.act === 'fan' ? '散热器' : 'LED' }} {{ pending.val === 1 ? '开启' : '关闭' }}...
      </div>
    </el-card>
  </div>
</template>

<script>
import request from '@/utils/request'
import SockJS from 'sockjs-client'
import Stomp from 'stompjs'
import deviceApi from '@/api/device'
import deviceOption from '@/api/deviceOption'

export default {
  name: 'RealTimeIndex',
  data() {
    return {
      // 设备相关
      deviceOptions: [],        // 设备下拉选项列表 [{value, label}]
      selectedDeviceKey: null,  // 当前选中的设备Key
      deviceLoading: false,     // 设备搜索加载状态

      environmentData: null,
      loading: false,
      error: null,
      stompClient: null,
      reconnectTimer: null,
      reconnectAttempts: 0,
      fanLoading: false,
      ledLoading: false,
      pollingTimer: null,
      pending: null
    }
  },
  computed: {
    fanStatusText() {
      return this.environmentData && this.environmentData.fanStatus === 1 ? '开启' : '关闭'
    },
    ledStatusText() {
      return this.environmentData && this.environmentData.ledStatus === 1 ? '开启' : '关闭'
    }
  },
  mounted() {
    this.init()  // 初始化：获取设备列表和默认设备
  },
  beforeDestroy() {
    this.disconnectWebSocket()
    this.clearPending()
    if (this.reconnectTimer) clearTimeout(this.reconnectTimer)
    if (this.pollingTimer) clearInterval(this.pollingTimer)
  },
  methods: {
    // ==================== 初始化 ====================
    async init() {
      await this.fetchDeviceList()        // 获取设备列表
      await this.loadDefaultDevice()      // 获取默认设备
    },

    // 获取设备列表（供下拉框使用）
    async fetchDeviceList(keyword = '') {
      this.deviceLoading = true
      try {
        const response = await deviceApi.getDeviceSelect({ keyword })
        this.deviceOptions = response.data.map(item => ({
          value: item.deviceKey,
          label: item.deviceName
        }))
      } catch (err) {
        console.error('获取设备列表失败', err)
        this.$message.error('设备列表加载失败')
        this.deviceOptions = []
      } finally {
        this.deviceLoading = false
      }
    },

    // 从后端获取默认设备
    async loadDefaultDevice() {
      if (this.deviceOptions.length === 0) {
        this.selectedDeviceKey = null
        return
      }

      try {
        const res = await request({
          url: '/service/device/default/getDefaultDevice',
          method: 'get'
        })

        // 判断返回是否成功且有设备Key
        if (res.code === 20000 && res.data && res.data.deviceKey) {
          const defaultDeviceKey = res.data.deviceKey
          // 检查该设备是否存在于当前设备列表中
          if (this.deviceOptions.some(device => device.value === defaultDeviceKey)) {
            this.selectedDeviceKey = defaultDeviceKey
            this.onDeviceChange(this.selectedDeviceKey)
            return
          }
        }
      } catch (err) {
        console.error('获取默认设备失败', err)
        // 接口调用失败，继续使用降级方案
      }

      // 如果默认设备获取失败或设备不存在，使用设备列表第一个
      if (this.deviceOptions.length > 0) {
        const firstDevice = this.deviceOptions[0].value
        this.selectedDeviceKey = firstDevice
        this.onDeviceChange(this.selectedDeviceKey)
        // 异步保存为默认设备（首次使用）
        this.setDefaultDevice(firstDevice)
      } else {
        this.selectedDeviceKey = null
      }
    },

    // 保存默认设备到后端
    async setDefaultDevice(deviceKey) {
      if (!deviceKey) return
      try {
        await request({
          url: '/service/device/default/setDefaultDevice',
          method: 'post',
          data: { deviceKey }
        })
      } catch (err) {
        console.error('保存默认设备失败', err)
        // 静默失败，不影响主要功能
      }
    },

    // 远程搜索（防抖）
    searchDevices(query) {
      if (query) {
        this.fetchDeviceList(query)
      } else {
        this.fetchDeviceList()
      }
    },

    // ==================== 设备切换核心逻辑 ====================
    onDeviceChange(deviceKey) {
      if (!deviceKey) {
        // 清空选择
        this.environmentData = null
        this.error = null
        this.disconnectWebSocket()
        this.stopPolling()
        return
      }

      // 保存为默认设备（后端同步）
      this.setDefaultDevice(deviceKey)

      // 重置状态
      this.environmentData = null
      this.error = null

      // 断开旧的WebSocket连接，重新建立
      this.disconnectWebSocket()

      // 获取新设备数据（HTTP首次）
      this.fetchData()

      // 建立新的WebSocket连接（订阅该设备主题）
      this.connectWebSocket()
    },

    // ==================== 数据获取 ====================
    async fetchData() {
      if (!this.selectedDeviceKey) return
      this.loading = true
      this.error = null
      try {
        const res = await request({
          url: '/service/environment/getLastEnvironment',
          method: 'get',
          params: { deviceKey: this.selectedDeviceKey }
        })
        if (res.code === 20000 && res.data) {
          this.applyEnvironmentPush(res.data)
        } else {
          // 接口返回成功但无数据
          this.environmentData = null
        }
      } catch (err) {
        console.error('获取实时数据失败', err)
        this.error = err.message || '获取数据失败，请检查网络'
        this.environmentData = null
      } finally {
        this.loading = false
      }
    },

    // ==================== WebSocket 连接 ====================
    connectWebSocket() {
      if (!this.selectedDeviceKey) return

      const socket = new SockJS(`${process.env.BASE_API}/ws/environment`)
      this.stompClient = Stomp.over(socket)
      this.stompClient.debug = (msg) => console.log(msg)

      this.stompClient.connect({}, (frame) => {
        console.log('STOMP 连接成功', frame)
        this.stopPolling()
        this.reconnectAttempts = 0
        if (this.reconnectTimer) clearTimeout(this.reconnectTimer)

        // 订阅当前设备的主题
        const envTopic = `/topic/environment/${this.selectedDeviceKey}`
        this.stompClient.subscribe(envTopic, (message) => {
          try {
            const data = JSON.parse(message.body)
            this.applyEnvironmentPush(data)
            if (this.error) this.error = null
          } catch (e) {
            console.error('解析消息失败', e)
          }
        })
        const cmdTopic = `/topic/command/${this.selectedDeviceKey}`
        this.stompClient.subscribe(cmdTopic, (message) => {
          try {
            this.handleCommandResult(JSON.parse(message.body))
          } catch (e) {
            console.error('解析指令结果失败', e)
          }
        })
      }, (error) => {
        console.error('STOMP 连接失败', error)
        this.error = '实时数据连接失败，启动轮询，尝试重连...'
        this.startPolling()
        this.reconnectWebSocket()
      })
    },

    disconnectWebSocket() {
      if (this.stompClient && this.stompClient.connected) {
        this.stompClient.disconnect()
      }
      this.stompClient = null
    },

    reconnectWebSocket() {
      if (this.reconnectTimer) return
      const delay = Math.min(1000 * Math.pow(2, this.reconnectAttempts), 30000)
      this.reconnectTimer = setTimeout(() => {
        this.reconnectAttempts++
        this.connectWebSocket()
        this.reconnectTimer = null
      }, delay)
    },

    startPolling() {
      if (this.pollingTimer) return
      this.pollingTimer = setInterval(() => {
        if (this.selectedDeviceKey) {
          this.fetchData()
        }
      }, 5000)
    },

    stopPolling() {
      if (this.pollingTimer) {
        clearInterval(this.pollingTimer)
        this.pollingTimer = null
      }
    },

    // ==================== 设备反控 ====================
    isActuatorPending(act) {
      return !!(this.pending && this.pending.act === act)
    },
    applyEnvironmentPush(data) {
      if (!data) return
      if (this.pending) {
        const expected = this.pending.val
        const actual = this.pending.act === 'fan' ? data.fanStatus : data.ledStatus
        if (actual === expected) {
          this.environmentData = data
          this.handleCommandResult({
            cmdId: this.pending.cmdId,
            status: 2,
            fan: data.fanStatus,
            led: data.ledStatus
          })
          return
        }
        if (this.environmentData) {
          if (this.pending.act === 'fan') {
            data.fanStatus = this.environmentData.fanStatus
          } else if (this.pending.act === 'led') {
            data.ledStatus = this.environmentData.ledStatus
          }
        }
      }
      this.environmentData = data
    },
    handleCommandResult(cmd) {
      if (!cmd || !this.pending || cmd.cmdId !== this.pending.cmdId) {
        return
      }
      const name = this.pending.act === 'fan' ? '散热器' : 'LED'
      const actionText = this.pending.val === 1 ? '开启' : '关闭'
      if (cmd.status === 2) {
        if (this.environmentData) {
          if (cmd.fan !== undefined && cmd.fan !== null) this.environmentData.fanStatus = cmd.fan
          if (cmd.led !== undefined && cmd.led !== null) this.environmentData.ledStatus = cmd.led
        }
        this.$message.success(`${name}${actionText}成功`)
      } else if (cmd.status === 3) {
        this.$message.error(`${name}执行失败`)
      } else if (cmd.status === 4) {
        this.$message.error('设备未响应')
      }
      this.clearPending()
    },
    clearPending() {
      if (this.pending && this.pending.timer) {
        clearTimeout(this.pending.timer)
      }
      this.pending = null
      this.fanLoading = false
      this.ledLoading = false
    },
    startPending(act, val, cmdId) {
      if (this.pending && this.pending.timer) {
        clearTimeout(this.pending.timer)
      }
      const timer = setTimeout(() => {
        if (this.pending && this.pending.cmdId === cmdId) {
          this.$message.error('设备未响应')
          this.clearPending()
        }
      }, 8000)
      this.pending = { act, val, cmdId, timer }
    },
    async sendControl(act, action) {
      if (!this.selectedDeviceKey) {
        this.$message.warning('请先选择设备')
        return
      }
      if (this.pending) {
        this.$message.warning('请等待上一次指令执行完成')
        return
      }
      const val = action === 'on' ? 1 : 0
      if (act === 'fan') this.fanLoading = true
      else this.ledLoading = true
      try {
        const res = await deviceOption.controlDevice({
          deviceType: act,
          deviceKey: this.selectedDeviceKey,
          command: action
        })
        if (res.code === 20000 && res.data && res.data.cmdId) {
          this.startPending(act, val, res.data.cmdId)
          this.$message.info('指令已发送，等待设备响应')
        } else {
          this.$message.error(res.msg || '操作失败')
          this.clearPending()
        }
      } catch (err) {
        this.$message.error((err && err.msg) || '操作失败，请重试')
        this.clearPending()
      }
    },
    async controlRadiator(action) {
      await this.sendControl('fan', action)
    },
    async controlLed(action) {
      await this.sendControl('led', action)
    }
  }
}
</script>

<style scoped>
.real-time-container {
  padding: 20px;
  display: flex;
  flex-direction: column;
  gap: 20px;
}

.device-selector-card,
.data-card,
.control-card {
  max-width: 900px;
  margin: 0 auto;
  width: 100%;
}

.pending-tip {
  margin-top: 12px;
  color: #E6A23C;
  font-size: 13px;
}

.loading-wrapper,
.error-wrapper,
.empty-device-wrapper {
  text-align: center;
  padding: 30px 0;
  color: #999;
}

.empty-device-wrapper {
  display: flex;
  flex-direction: column;
  align-items: center;
  justify-content: center;
}

.data-content {
  display: flex;
  flex-direction: column;
  gap: 20px;
}

.data-row {
  display: flex;
  flex-wrap: wrap;
  gap: 20px;
}

.data-row .info-item {
  flex: 1 1 0;
  min-width: 200px;
  display: flex;
  align-items: center;
  font-size: 16px;
  padding: 8px 0;
}

.info-item .label {
  font-weight: bold;
  color: #666;
  min-width: 100px;
  text-align: right;
  margin-right: 8px;
  white-space: nowrap;
}

.info-item .value {
  color: #333;
  text-align: left;
  flex: 1;
}

.info-item .el-tag {
  margin-left: 0;
}

.info-item.full-width {
  flex: 1;
}

.timestamp {
  border-top: 1px dashed #eee;
  padding-top: 15px;
  margin-top: 5px;
}
.timestamp .info-item .label {
  min-width: 120px;
}

.control-item {
  display: flex;
  align-items: center;
  padding: 15px;
  border: 1px solid #ebeef5;
  border-radius: 4px;
  transition: all 0.3s;
}
.control-item:hover {
  box-shadow: 0 2px 12px 0 rgba(0,0,0,0.1);
}
.control-icon {
  font-size: 48px;
  margin-right: 20px;
  width: 60px;
  text-align: center;
}
.control-icon i {
  transition: color 0.3s;
}

/* 风扇样式 */
.fan-svg {
  width: 60px;
  height: 60px;
  transition: filter 0.3s;
  transform-origin: center center;
}
.fan-on {
  animation: spin 2s linear infinite;
  filter: none;
}
.fan-off {
  animation: none;
  filter: grayscale(100%) brightness(0.7);
}
@keyframes spin {
  from { transform: rotate(0deg); }
  to { transform: rotate(360deg); }
}

/* LED样式 */
.led-svg {
  width: 60px;
  height: 60px;
  transition: filter 0.3s;
}
.led-on {
  filter: drop-shadow(0 0 6px #FFE564);
}

.control-info {
  flex: 1;
}
.control-label {
  font-size: 18px;
  font-weight: bold;
  margin-bottom: 5px;
}
.control-status {
  font-size: 14px;
  color: #666;
  margin-bottom: 10px;
}
.control-buttons {
  display: flex;
  gap: 10px;
}
</style>
