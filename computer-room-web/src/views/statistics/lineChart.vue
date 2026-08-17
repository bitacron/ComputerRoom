<template>
  <div class="app-container">
    <!-- 查询表单 -->
    <el-form :inline="true" class="demo-form-inline">
      <el-form-item label="开始时间">
        <el-date-picker
          v-model="environmentQuery.begin"
          type="datetime"
          placeholder="选择开始时间"
          value-format="yyyy-MM-dd HH:mm:ss"
          default-time="00:00:00"
        />
      </el-form-item>
      <el-form-item label="截止时间">
        <el-date-picker
          v-model="environmentQuery.end"
          type="datetime"
          placeholder="选择截止时间"
          value-format="yyyy-MM-dd HH:mm:ss"
          default-time="00:00:00"
        />
      </el-form-item>
      <!-- 快捷时间选择下拉框 -->
      <el-form-item label="快捷选择">
        <el-select
          v-model="quickSelect"
          placeholder="请选择"
          clearable
          style="width: 140px"
          @change="handleQuickSelect"
        >
          <el-option
            v-for="item in quickOptions"
            :key="item.value"
            :label="item.label"
            :value="item.value"
          />
        </el-select>
      </el-form-item>
      <!-- 新增：设备标识下拉框 -->
      <el-form-item label="设备标识">
        <el-select
          v-model="environmentQuery.deviceKey"
          clearable
          placeholder="请选择设备"
          filterable
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
      <el-form-item>
        <el-button type="primary" @click="handleQuery">查询</el-button>
        <el-button @click="resetQuery">重置</el-button>
      </el-form-item>
    </el-form>

    <!-- 两个独立图表容器 -->
    <div class="chart-wrapper">
      <div class="chart-container">
        <div ref="tempHumChart" class="chart" style="height:350px;"/>
      </div>
      <div class="chart-container" style="margin-top:30px;">
        <div ref="gasChart" class="chart" style="height:350px;"/>
      </div>
    </div>
  </div>
</template>

<script>
import * as echarts from 'echarts'
import request from '@/utils/request'
import deviceApi from '@/api/device'   // 新增设备接口导入

export default {
  name: 'LineChart',
  data() {
    return {
      tempHumChart: null,
      gasChart: null,
      chartData: {
        times: [],
        temperature: [],
        humidity: [],
        gasPpm: []
      },
      environmentQuery: {
        begin: '',
        end: '',
        deviceKey: ''    // 新增设备标识字段
      },
      // 快捷选项数据
      quickOptions: [
        { label: '最近一年', value: '1year' },
        { label: '最近六个月', value: '6month' },
        { label: '最近三个月', value: '3month' },
        { label: '最近一个月', value: '1month' },
        { label: '最近半个月', value: '15day' },
        { label: '最近7天', value: '7day' },
        { label: '最近三天', value: '3day' },
        { label: '最近24小时', value: '24hour' },
        { label: '最近12小时', value: '12hour' }
      ],
      quickSelect: '', // 当前选中的快捷选项值
      deviceOptions: [],   // 设备下拉选项列表
      // 标志位，防止初始化时设备切换触发重复查询
      isInitializing: false
    }
  },
  mounted() {
    this.initCharts()
    // 先加载设备列表，内部完成默认设备设置和首次查询
    this.init()
    window.addEventListener('resize', this.resizeHandler)
  },
  beforeDestroy() {
    window.removeEventListener('resize', this.resizeHandler)
    if (this.tempHumChart) this.tempHumChart.dispose()
    if (this.gasChart) this.gasChart.dispose()
  },
  methods: {
    // ==================== 初始化 ====================
    async init() {
      await this.fetchDeviceList()        // 获取设备列表
      await this.loadDefaultDevice()      // 获取默认设备
    },

    // 初始化图表（折线图，带 dataZoom）
    initCharts() {
      this.tempHumChart = echarts.init(this.$refs.tempHumChart)
      this.tempHumChart.setOption({
        title: { text: '温度与湿度变化', left: 'center' },
        tooltip: { trigger: 'axis' },
        legend: { data: ['温度(℃)', '湿度(%)'], top: 30 },
        xAxis: { type: 'category', data: [] },
        yAxis: { type: 'value', name: '数值' },
        series: [
          { name: '温度(℃)', type: 'line', data: [] },
          { name: '湿度(%)', type: 'line', data: [] }
        ],
        dataZoom: [
          { type: 'slider', start: 0, end: 100 },
          { type: 'inside', start: 0, end: 100 }
        ]
      })

      this.gasChart = echarts.init(this.$refs.gasChart)
      this.gasChart.setOption({
        title: { text: '烟雾浓度变化', left: 'center' },
        tooltip: { trigger: 'axis' },
        xAxis: { type: 'category', data: [] },
        yAxis: { type: 'value', name: '浓度(ppm)' },
        series: [
          { name: '烟雾浓度(ppm)', type: 'line', data: [] }
        ],
        dataZoom: [
          { type: 'slider', start: 0, end: 100 },
          { type: 'inside', start: 0, end: 100 }
        ]
      })
    },

    // 获取设备列表
    async fetchDeviceList() {
      try {
        const response = await deviceApi.getDeviceSelect()
        this.deviceOptions = response.data.map(item => ({
          value: item.deviceKey,
          label: item.deviceName || item.deviceKey  // 优先显示设备名称
        }))

        if (this.deviceOptions.length === 0) {
          console.warn('设备列表为空')
          this.$message.warning('暂无可用设备')
        }
      } catch (error) {
        console.error('获取设备列表失败', error)
        this.$message.error('设备列表加载失败')
        this.deviceOptions = []
      }
    },

    // 从后端获取默认设备
    async loadDefaultDevice() {
      if (this.deviceOptions.length === 0) {
        this.environmentQuery.deviceKey = ''
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
            // 开启初始化标志，避免触发查询（因为查询会在后面手动调用）
            this.isInitializing = true
            this.environmentQuery.deviceKey = defaultDeviceKey
            this.isInitializing = false
            // 加载默认设备的数据
            this.fetchData()
            return
          } else {
            // 默认设备已被删除，提示用户
            this.$message.warning('您上次选择的设备已不可用，已为您切换到最新设备')
          }
        }
      } catch (err) {
        console.error('获取默认设备失败', err)
        // 接口调用失败，继续使用降级方案
      }
      
      // 如果默认设备获取失败或设备不存在，使用设备列表第一个
      if (this.deviceOptions.length > 0) {
        const firstDevice = this.deviceOptions[0].value
        this.isInitializing = true
        this.environmentQuery.deviceKey = firstDevice
        this.isInitializing = false
        this.fetchData()
        // 异步保存为默认设备（首次使用）
        this.setDefaultDevice(firstDevice)
      } else {
        this.environmentQuery.deviceKey = ''
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

    // 获取环境数据
    fetchData() {
      // 如果未选择设备且设备列表有值，尝试使用第一个设备（兜底）
      if (!this.environmentQuery.deviceKey && this.deviceOptions.length > 0) {
        this.environmentQuery.deviceKey = this.deviceOptions[0].value
      }
      if (!this.environmentQuery.deviceKey) {
        this.$message.warning('请先选择设备')
        return
      }

      request({
        url: '/service/environment/statistics',
        method: 'post',
        data: this.environmentQuery
      }).then(response => {
        if (response.code === 20000) {
          const records = response.data || []
          const dataArray = Array.isArray(records) ? records : []
          this.processData(dataArray)
          this.updateCharts()
        } else {
          this.$message.error('数据获取失败')
        }
      }).catch(err => {
        console.error(err)
        this.$message.error('请求异常')
      })
    },

    // 处理原始数据
    processData(records) {
      const times = []
      const temperature = []
      const humidity = []
      const gasPpm = []
      // 按时间正序展示（原数据可能是倒序，这里先反转）
      records.slice().reverse().forEach(item => {
        const time = item.gmtMeasurement
        times.push(time)
        temperature.push(item.temperature)
        humidity.push(item.humidity)
        gasPpm.push(item.gasPpm)
      })
      this.chartData = { times, temperature, humidity, gasPpm }
    },

    // 更新图表
    updateCharts() {
      this.tempHumChart.setOption({
        xAxis: { data: this.chartData.times },
        series: [
          { data: this.chartData.temperature },
          { data: this.chartData.humidity }
        ]
      })
      this.gasChart.setOption({
        xAxis: { data: this.chartData.times },
        series: [
          { data: this.chartData.gasPpm }
        ]
      })
    },

    // 设备切换事件（自动查询并保存默认设备）
    onDeviceChange() {
      // 初始化过程中不重复查询
      if (this.isInitializing) return
      
      const deviceKey = this.environmentQuery.deviceKey
      if (deviceKey) {
        // 保存为默认设备（后端同步）
        this.setDefaultDevice(deviceKey)
      }
      
      // 切换设备后自动刷新数据
      this.fetchData()
    },

    // 处理快捷选择变化
    handleQuickSelect(value) {
      if (!value) return
      const now = new Date()
      const end = this.formatDate(now)
      const beginDate = new Date()

      switch (value) {
        case '1year':
          beginDate.setFullYear(beginDate.getFullYear() - 1)
          break
        case '6month':
          beginDate.setMonth(beginDate.getMonth() - 6)
          break
        case '3month':
          beginDate.setMonth(beginDate.getMonth() - 3)
          break
        case '1month':
          beginDate.setMonth(beginDate.getMonth() - 1)
          break
        case '15day':
          beginDate.setDate(beginDate.getDate() - 15)
          break
        case '7day':
          beginDate.setDate(beginDate.getDate() - 7)
          break
        case '3day':
          beginDate.setDate(beginDate.getDate() - 3)
          break
        case '24hour':
          beginDate.setHours(beginDate.getHours() - 24)
          break
        case '12hour':
          beginDate.setHours(beginDate.getHours() - 12)
          break
        default:
          return
      }

      this.environmentQuery.begin = this.formatDate(beginDate)
      this.environmentQuery.end = end
      // 自动触发查询
      this.fetchData()
    },

    // 辅助方法：将 Date 对象格式化为 yyyy-MM-dd HH:mm:ss
    formatDate(date) {
      const year = date.getFullYear()
      const month = (date.getMonth() + 1).toString().padStart(2, '0')
      const day = date.getDate().toString().padStart(2, '0')
      const hours = date.getHours().toString().padStart(2, '0')
      const minutes = date.getMinutes().toString().padStart(2, '0')
      const seconds = date.getSeconds().toString().padStart(2, '0')
      return `${year}-${month}-${day} ${hours}:${minutes}:${seconds}`
    },

    // 查询按钮点击
    handleQuery() {
      this.fetchData()
    },

    // 重置查询条件
    resetQuery() {
      // 清空时间范围和快捷选择
      this.environmentQuery.begin = ''
      this.environmentQuery.end = ''
      this.quickSelect = ''

      // 重置设备为默认设备（从后端获取）
      if (this.deviceOptions.length > 0) {
        // 开启初始化标志，避免设备change触发重复查询
        this.isInitializing = true
        // 重新加载默认设备（会触发查询）
        this.loadDefaultDevice()
        this.isInitializing = false
      } else {
        // 没有设备，仅清空时间
        this.fetchData()
      }
    },

    // 窗口大小自适应
    resizeHandler() {
      if (this.tempHumChart) this.tempHumChart.resize()
      if (this.gasChart) this.gasChart.resize()
    }
  }
}
</script>

<style scoped>
.chart-wrapper {
  margin-top: 20px;
}
.chart-container {
  background: #fff;
  padding: 20px;
  border-radius: 4px;
  box-shadow: 0 2px 12px 0 rgba(0,0,0,0.1);
}
.el-form-item {
  margin-bottom: 0;
}
</style>