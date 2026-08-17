<template>
  <view class="history-container">
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
        <!-- 可选搜索框（如需搜索可取消注释） -->
        <!-- <view class="search-box">
          <input type="text" v-model="searchKeyword" placeholder="输入设备ID或名称搜索" @confirm="searchDevices" @input="onSearchInput" />
          <button @click="searchDevices" size="mini" type="primary">搜索</button>
        </view> -->
      </view>
    </view>

    <!-- 顶部传感器 Tab -->
    <scroll-view scroll-x class="sensor-tabs" :scroll-with-animation="true">
      <view class="tab-list">
        <view
          v-for="item in sensorTypes"
          :key="item.value"
          class="tab-item"
          :class="{ active: currentSensor === item.value }"
          @click="switchSensor(item.value)"
        >
          {{ item.label+"("+item.unit+")" }}
        </view>
      </view>
    </scroll-view>

    <!-- 时间范围选择 -->
    <view class="time-range">
      <view class="range-buttons">
        <view
          v-for="range in timeRanges"
          :key="range.value"
          class="range-btn"
          :class="{ active: currentRange === range.value }"
          @click="switchRange(range.value)"
        >
          {{ range.label }}
        </view>
        <view class="range-btn custom" @click="showCustomDate = true">
          自定义
        </view>
      </view>
      <view class="custom-date-range" v-if="showCustomDate">
        <view class="date-item">
          <text>开始时间：</text>
          <picker mode="date" :value="customStartDate" @change="onStartDateChange">
            <view class="picker">{{ customStartDate || '请选择' }}</view>
          </picker>
        </view>
        <view class="date-item">
          <text>结束时间：</text>
          <picker mode="date" :value="customEndDate" @change="onEndDateChange">
            <view class="picker">{{ customEndDate || '请选择' }}</view>
          </picker>
        </view>
        <view class="date-buttons">
          <button class="confirm-btn" @click="confirmCustomDate">确定</button>
          <button class="cancel-btn" @click="showCustomDate = false">取消</button>
        </view>
      </view>
      <view class="selected-date" v-if="customStartTime">
        {{ formatDate(customStartTime) }} 至 {{ formatDate(customEndTime) }}
      </view>
    </view>

    <!-- 图表区域 -->
    <view class="chart-card">
      <view class="chart-header">
        <view class="chart-title">{{ currentSensorLabel }}变化趋势</view>
        <view class="chart-type-switch">
          <view
            :class="['switch-btn', { active: chartType === 'line' }]"
            @click="switchChartType('line')"
          >折线图</view>
          <view
            :class="['switch-btn', { active: chartType === 'column' }]"
            @click="switchChartType('column')"
          >柱状图</view>
        </view>
      </view>
      <view class="chart-container">
        <qiun-data-charts
          :key="chartType"
          :type="chartType"
          :opts="chartOpts"
          :chartData="chartData"
          :canvas2d="true"
          tooltipFormat="tooltipDemo1"
        />
      </view>
    </view>

    <!-- 数据列表 -->
    <view class="list-card">
      <view class="list-header">
        <text class="list-title">历史数据明细</text>
        <text class="list-tip">共 {{ historyList.length }} 条</text>
      </view>
      <scroll-view scroll-y class="data-list" @scrolltolower="loadMore" :enable-back-to-top="true">
        <view v-if="historyList.length === 0 && !loading" class="empty-list">
          <text>暂无数据</text>
        </view>
        <view
          v-for="(item, index) in displayList"
          :key="item.id"
          class="list-item"
          @click="showDetail(item)"
        >
          <text class="item-time">{{ formatTime(item.gmtMeasurement) }}</text>
          <text class="item-value">{{ getValueBySensor(item) }} {{ currentUnit }}</text>
        </view>
        <view v-if="hasMore && !loading" class="load-more" @click="loadMore">
          加载更多...
        </view>
        <view v-if="!hasMore && historyList.length > 0" class="no-more">
          没有更多了
        </view>
      </scroll-view>
    </view>
  </view>
</template>

<script>
import request from '@/utils/request.js';
import QiunDataCharts from '@/uni_modules/qiun-data-charts/components/qiun-data-charts/qiun-data-charts.vue'
import QiunLoading from '@/uni_modules/qiun-data-charts/components/qiun-loading/qiun-loading.vue'

export default {
  components: { QiunDataCharts, QiunLoading },
  data() {
    return {
      // 设备相关
      deviceOptions: [],        // [{ value: 'deviceKey', label: 'deviceName - deviceKey' }]
      selectedDeviceKey: null,  // 当前选中的设备Key
      selectedIndex: -1,        // picker 选中索引
      selectedDeviceLabel: '',  // 当前显示的标签
      deviceLoading: false,     // 设备列表加载中
      defaultDeviceKey: null,   // 用户设置的默认设备Key
      searchKeyword: '',        // 搜索关键词（可选）
      searchTimer: null,        // 搜索防抖定时器

      sensorTypes: [
        { label: '温度', value: 'temperature', unit: '°C' },
        { label: '湿度', value: 'humidity', unit: '%' },
        { label: '气体浓度', value: 'gasPpm', unit: 'ppm' },
        { label: '光照强度', value: 'lightPercentage', unit: '%' },
        { label: '明火概率', value: 'flamePercentage', unit: '%' }
      ],
      currentSensor: 'temperature',
      timeRanges: [
        { label: '全部', value: 'all' },
        { label: '今日', value: 'today' },
        { label: '本周', value: 'week' },
        { label: '本月', value: 'month' }
      ],
      currentRange: 'all',
      customStartTime: null,
      customEndTime: null,
      chartType: 'line',
      chartData: {},
      chartOpts: {
        color: ["#409eff"],
        padding: [15, 15, 0, 5],
        enableScroll: false,
        dataLabel: false,
        dataPointShape: false,
        legend: {
          show: false,
          position: 'top',
          align: 'center',
          lineHeight: 20,
          fontSize: 12,
          fontColor: '#666',
          borderWidth: 0,
          borderColor: '#ccc',
          padding: 10,
          margin: 10,
          backgroundColor: 'rgba(255,255,255,0.9)',
        },
        xAxis: {
          disableGrid: true,
          rotateLabel: true,
          labelCount: 5,
          itemCount: 300,
          fontSize: 10
        },
        yAxis: {
          data: [{ min: 0 }]
        },
        extra: {
          line: {
            type: 'straight'
          },
          column: {
            type: 'group',
            width: 2
          }
        }
      },
      historyList: [],
      displayList: [],
      loading: false,
      pageSize: 20,
      currentPage: 1,
      showCustomDate: false,
      customStartDate: '',
      customEndDate: ''
    };
  },
  computed: {
    currentSensorLabel() {
      const found = this.sensorTypes.find(s => s.value === this.currentSensor);
      return found ? found.label : '';
    },
    currentUnit() {
      const found = this.sensorTypes.find(s => s.value === this.currentSensor);
      return found ? found.unit : '';
    },
    hasMore() {
      return this.displayList.length < this.historyList.length;
    }
  },
  watch: {
    currentSensor() {
      this.resetAndFetch();
    },
    currentRange() {
      this.resetAndFetch();
    },
    customStartTime() {
      this.resetAndFetch();
    }
  },
  // async mounted() {
  //   // 页面挂载时：先获取默认设备，再加载设备列表
  //   await this.fetchDefaultDevice();
  //   this.fetchDeviceList();
  // },
  onLoad() {
	  // 页面首次加载时初始化
	  this.initData();
  },
  onShow() {
    // 每次显示时，如果已有设备但设备列表可能变化，可重新加载（可选）
    // 为避免频繁刷新，可保留此逻辑，但注意防抖  // 每次显示页面时都重新加载（包括tab切换）
	this.initData();
    // this.fetchDeviceList();
  },
  onHide() {
    if (this.searchTimer) clearTimeout(this.searchTimer);
  },
  methods: {
	async initData() {
	  await this.fetchDefaultDevice();
	  this.fetchDeviceList();
	},
    // ========== 默认设备相关 ==========
    // 获取用户默认设备
    async fetchDefaultDevice() {
      try {
        const res = await request({
          url: '/service/device/default/getDefaultDevice',
          method: 'GET'
        });
        if (res.code === 20000 && res.data && res.data.deviceKey) {
          this.defaultDeviceKey = res.data.deviceKey;
        } else {
          this.defaultDeviceKey = null;
        }
      } catch (err) {
        console.error('获取默认设备失败', err);
        this.defaultDeviceKey = null;
      }
    },

    // 设置用户默认设备
    async setDefaultDevice(deviceKey) {
      if (!deviceKey) return;
      try {
        await request({
          url: '/service/device/default/setDefaultDevice',
          method: 'POST',
          data: { deviceKey: deviceKey }
        });
        // 设置成功，更新本地缓存的默认设备
        this.defaultDeviceKey = deviceKey;
      } catch (err) {
        console.error('设置默认设备失败', err);
        // 设置失败不影响主流程，静默处理或提示
      }
    },

    // ========== 设备列表 ==========
    async fetchDeviceList(keyword = '') {
      this.deviceLoading = true;
      try {
        const res = await request({
          url: '/service/device/getDeviceSelect',
          method: 'get',
          params: { keyword }
        });
        // 兼容多种返回结构
        let deviceArray = [];
        if (res.code === 20000 && Array.isArray(res.data)) {
          deviceArray = res.data;
        } else if (Array.isArray(res)) {
          deviceArray = res;
        } else if (res.data && Array.isArray(res.data.list)) {
          deviceArray = res.data.list;
        }
        this.deviceOptions = deviceArray.map(item => ({
          value: item.deviceKey,
          label: `${item.deviceName || '未命名'} (${item.deviceKey})`
        }));

        // 优先选中默认设备，如果没有默认设备则选中第一个
        if (this.deviceOptions.length > 0) {
          let targetDevice = null;
          let targetIndex = -1;

          // 1. 尝试查找默认设备
          if (this.defaultDeviceKey) {
            const defaultIndex = this.deviceOptions.findIndex(
              item => item.value === this.defaultDeviceKey
            );
            if (defaultIndex !== -1) {
              targetIndex = defaultIndex;
              targetDevice = this.deviceOptions[defaultIndex];
            }
          }

          // 2. 没有默认设备或默认设备不在列表中，选第一个
          if (!targetDevice) {
            targetIndex = 0;
            targetDevice = this.deviceOptions[0];
            // 同时更新默认设备
            this.setDefaultDevice(targetDevice.value);
          }

          this.selectedDeviceKey = targetDevice.value;
          this.selectedIndex = targetIndex;
          this.selectedDeviceLabel = targetDevice.label;
          this.onDeviceChange(this.selectedDeviceKey);
        } else {
          // 没有设备时清空
          this.selectedDeviceKey = null;
          this.selectedIndex = -1;
          this.selectedDeviceLabel = '';
          this.historyList = [];
          this.displayList = [];
          this.chartData = {};
        }
      } catch (err) {
        console.error('获取设备列表失败', err);
        uni.showToast({ title: '设备列表加载失败', icon: 'none' });
      } finally {
        this.deviceLoading = false;
      }
    },

    // 搜索设备（可选）
    searchDevices() {
      this.fetchDeviceList(this.searchKeyword);
    },
    onSearchInput(e) {
      clearTimeout(this.searchTimer);
      this.searchTimer = setTimeout(() => {
        this.searchDevices();
      }, 300);
    },

    // picker 选择变化
    onDevicePickerChange(e) {
      const idx = e.detail.value;
      if (idx >= 0 && idx < this.deviceOptions.length) {
        const selected = this.deviceOptions[idx];
        this.selectedDeviceKey = selected.value;
        this.selectedIndex = idx;
        this.selectedDeviceLabel = selected.label;
        this.onDeviceChange(selected.value);
        // 用户手动选择设备后，更新默认设备
        this.setDefaultDevice(selected.value);
      } else {
        this.selectedDeviceKey = null;
        this.selectedIndex = -1;
        this.selectedDeviceLabel = '';
        this.onDeviceChange(null);
      }
    },

    // 设备切换时重置数据并重新加载历史
    onDeviceChange(deviceKey) {
      if (!deviceKey) {
        this.historyList = [];
        this.displayList = [];
        this.chartData = {};
        return;
      }
      this.resetAndFetch(); // 重新获取历史数据
    },

    // ========== 原有历史数据逻辑 ==========
    async fetchHistory() {
      if (!this.selectedDeviceKey) {
        // 没有选中设备时不清空，等待设备选中
        return;
      }
      this.loading = true;
      try {
        const params = {
          begin: this.getStartTime(),
          end: this.getEndTime(),
          deviceKey: this.selectedDeviceKey   // 关键：添加设备筛选参数
        };
        const res = await request({
          url: '/service/environment/statistics',
          method: 'POST',
          data: params
        });
        if (res.code === 20000 && res.data) {
          this.historyList = res.data || [];
          this.currentPage = 1;
          this.updateDisplayList();
          this.updateChartData();
        } else {
          uni.showToast({ title: res.msg || '获取数据失败', icon: 'none' });
        }
      } catch (err) {
        uni.showToast({ title: err.msg || '网络异常，请重试', icon: 'none' });
      } finally {
        this.loading = false;
      }
    },

    // 更新图表数据
    updateChartData() {
      if (this.historyList.length === 0) {
        this.chartData = {};
        return;
      }
      const sortedData = [...this.historyList].sort((a, b) => 
        new Date(a.gmtMeasurement) - new Date(b.gmtMeasurement)
      );
      const maxPoints = 300;
      const step = Math.ceil(sortedData.length / maxPoints);
      const sampledData = sortedData.filter((_, index) => index % step === 0).slice(0, maxPoints);

      const categories = sampledData.map(item => this.formatTimeShort(item.gmtMeasurement));
      const seriesData = sampledData.map(item => {
        const val = item[this.currentSensor];
        return val !== undefined ? val : 0;
      });

      this.chartData = {
        categories: categories,
        series: [{
          name: this.currentSensorLabel + "(" + this.currentUnit + ")",
          data: seriesData
        }]
      };
    },

    updateDisplayList() {
      const start = 0;
      const end = this.currentPage * this.pageSize;
      this.displayList = this.historyList.slice(start, end);
    },

    loadMore() {
      if (this.hasMore && !this.loading) {
        this.currentPage++;
        this.updateDisplayList();
      }
    },

    resetAndFetch() {
      // 重置分页，重新获取数据
      this.currentPage = 1;
      this.fetchHistory();
    },

    switchSensor(sensor) {
      this.currentSensor = sensor;
    },

    switchRange(range) {
      this.currentRange = range;
      this.customStartTime = null;
      this.customEndTime = null;
      this.showCustomDate = false;
      this.customStartDate = '';
      this.customEndDate = '';
    },

    getStartTime() {
      if (this.customStartTime) {
        return this.formatToBackendDatetime(this.customStartTime);
      }
      switch (this.currentRange) {
        case 'all':
          return '';
        case 'today':
          const todayStart = new Date();
          todayStart.setHours(0, 0, 0, 0);
          return this.formatToBackendDatetime(todayStart);
        case 'week':
          const weekStart = new Date();
          weekStart.setDate(weekStart.getDate() - 7);
          weekStart.setHours(0, 0, 0, 0);
          return this.formatToBackendDatetime(weekStart);
        case 'month':
          const monthStart = new Date();
          monthStart.setMonth(monthStart.getMonth() - 1);
          monthStart.setHours(0, 0, 0, 0);
          return this.formatToBackendDatetime(monthStart);
        default:
          return '';
      }
    },

    getEndTime() {
      if (this.customEndTime) {
        return this.formatToBackendDatetime(this.customEndTime);
      }
      switch (this.currentRange) {
        case 'all':
          return '';
        case 'today':
        case 'week':
        case 'month':
          const now = new Date();
          return this.formatToBackendDatetime(now);
        default:
          return '';
      }
    },

    formatToBackendDatetime(date) {
      let d = date;
      if (typeof date === 'string') {
        d = new Date(date);
      }
      if (!(d instanceof Date) || isNaN(d.getTime())) {
        return '';
      }
      const year = d.getFullYear();
      const month = String(d.getMonth() + 1).padStart(2, '0');
      const day = String(d.getDate()).padStart(2, '0');
      const hours = String(d.getHours()).padStart(2, '0');
      const minutes = String(d.getMinutes()).padStart(2, '0');
      const seconds = String(d.getSeconds()).padStart(2, '0');
      return `${year}-${month}-${day} ${hours}:${minutes}:${seconds}`;
    },

    formatDate(date) {
      if (!date) return '';
      const d = new Date(date);
      return `${d.getFullYear()}-${d.getMonth() + 1}-${d.getDate()}`;
    },

    formatTime(dateStr) {
      if (!dateStr) return '';
      const date = new Date(dateStr);
      const year = date.getFullYear();
      const month = date.getMonth() + 1;
      const day = date.getDate();
      const hour = date.getHours();
      const minute = date.getMinutes();
      const second = date.getSeconds();
      return `${year}-${month}-${day} ${hour}:${minute.toString().padStart(2, '0')}:${second.toString().padStart(2, '0')}`;
    },

    formatTimeShort(dateStr) {
      if (!dateStr) return '';
      const date = new Date(dateStr);
      const year = date.getFullYear();
      const month = date.getMonth() + 1;
      const day = date.getDate();
      const hour = date.getHours();
      const minute = date.getMinutes();
      const second = date.getSeconds();
      return `${month}-${day} ${hour}:${minute.toString().padStart(2, '0')}:${second.toString().padStart(2, '0')}`;
    },

    getValueBySensor(item) {
      const val = item[this.currentSensor];
      return val !== undefined ? val : '--';
    },

    showDetail(item) {
      uni.showModal({
        title: '详细信息',
        content: `设备：${this.selectedDeviceLabel}\n时间：${item.gmtMeasurement}\n${this.currentSensorLabel}：${this.getValueBySensor(item)} ${this.currentUnit}`,
        showCancel: false
      });
    },

    onStartDateChange(e) {
      this.customStartDate = e.detail.value;
    },

    onEndDateChange(e) {
      this.customEndDate = e.detail.value;
    },

    confirmCustomDate() {
      if (!this.customStartDate || !this.customEndDate) {
        uni.showToast({ title: '请选择完整的日期范围', icon: 'none' });
        return;
      }
      const start = new Date(this.customStartDate);
      start.setHours(0, 0, 0, 0);
      const end = new Date(this.customEndDate);
      end.setHours(23, 59, 59, 999);
      this.customStartTime = start;
      this.customEndTime = end;
      this.showCustomDate = false;
    },

    switchChartType(type) {
      this.chartType = type;
    }
  }
};
</script>

<style scoped>
.history-container {
  background-color: #f5f7fa;
  min-height: 100vh;
  padding-bottom: 30rpx;
}
/* 设备选择卡片样式 */
.device-card {
  background-color: #fff;
  border-radius: 24rpx;
  margin: 20rpx;
  margin-bottom: 0;
  overflow: hidden;
}
.device-card .card-header {
  padding: 24rpx 32rpx;
  border-bottom: 1rpx solid #f0f0f0;
  font-size: 32rpx;
  font-weight: 600;
  color: #303133;
}
.device-card .card-body {
  padding: 20rpx 32rpx 32rpx;
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
.search-box {
  display: flex;
  margin-top: 20rpx;
  gap: 20rpx;
}
.search-box input {
  flex: 1;
  background-color: #f5f5f5;
  padding: 16rpx 24rpx;
  border-radius: 48rpx;
  font-size: 28rpx;
}
.search-box button {
  width: auto;
  padding: 0 24rpx;
  font-size: 28rpx;
}

.sensor-tabs {
  background-color: #fff;
  white-space: nowrap;
  border-bottom: 1px solid #f0f0f0;
  padding: 0 20rpx;
  margin-top: 20rpx;
}
.tab-list {
  display: inline-flex;
}
.tab-item {
  display: inline-block;
  padding: 24rpx 32rpx;
  font-size: 28rpx;
  color: #606266;
  position: relative;
}
.tab-item.active {
  color: #409eff;
  font-weight: 500;
}
.tab-item.active::after {
  content: '';
  position: absolute;
  bottom: 0;
  left: 50%;
  transform: translateX(-50%);
  width: 60rpx;
  height: 4rpx;
  background-color: #409eff;
  border-radius: 2rpx;
}
.time-range {
  background-color: #fff;
  padding: 20rpx;
  margin-top: 12rpx;
}
.range-buttons {
  display: flex;
  flex-wrap: wrap;
  gap: 20rpx;
}
.range-btn {
  padding: 12rpx 24rpx;
  border-radius: 60rpx;
  background-color: #f4f4f5;
  font-size: 26rpx;
  color: #606266;
}
.range-btn.active {
  background-color: #409eff;
  color: #fff;
}
.range-btn.custom {
  background-color: #fff;
  border: 1px solid #dcdfe6;
}
.selected-date {
  margin-top: 16rpx;
  font-size: 24rpx;
  color: #909399;
  text-align: center;
}
.custom-date-range {
  margin-top: 20rpx;
  padding: 20rpx;
  background-color: #f9f9f9;
  border-radius: 16rpx;
}
.date-item {
  display: flex;
  align-items: center;
  margin-bottom: 20rpx;
}
.date-item text {
  width: 140rpx;
  font-size: 28rpx;
  color: #606266;
}
.picker {
  flex: 1;
  padding: 16rpx 20rpx;
  background-color: #fff;
  border-radius: 12rpx;
  border: 1px solid #e4e7ed;
  font-size: 28rpx;
}
.date-buttons {
  display: flex;
  gap: 20rpx;
  margin-top: 20rpx;
}
.confirm-btn, .cancel-btn {
  flex: 1;
  padding: 16rpx;
  border-radius: 60rpx;
  font-size: 28rpx;
  text-align: center;
}
.confirm-btn {
  background-color: #409eff;
  color: #fff;
}
.cancel-btn {
  background-color: #fff;
  border: 1px solid #dcdfe6;
  color: #606266;
}
.chart-card {
  background-color: #fff;
  border-radius: 24rpx;
  margin: 20rpx;
  padding: 20rpx;
  box-shadow: 0 4rpx 12rpx rgba(0,0,0,0.05);
}
.chart-header {
  display: flex;
  justify-content: space-between;
  align-items: center;
  margin-bottom: 20rpx;
}
.chart-title {
  font-size: 30rpx;
  font-weight: 600;
  color: #303133;
}
.chart-type-switch {
  display: flex;
  gap: 20rpx;
}
.switch-btn {
  padding: 6rpx 20rpx;
  border-radius: 40rpx;
  background-color: #f4f4f5;
  font-size: 24rpx;
  color: #606266;
}
.switch-btn.active {
  background-color: #409eff;
  color: #fff;
}
.chart-container {
  width: 100%;
  height: 400rpx;
}
.list-card {
  background-color: #fff;
  border-radius: 24rpx;
  margin: 0 20rpx;
  overflow: hidden;
}
.list-header {
  display: flex;
  justify-content: space-between;
  padding: 24rpx;
  border-bottom: 1px solid #f0f0f0;
}
.list-title {
  font-size: 30rpx;
  font-weight: 600;
}
.list-tip {
  font-size: 24rpx;
  color: #909399;
}
.data-list {
  max-height: 600rpx;
}
.list-item {
  display: flex;
  justify-content: space-between;
  padding: 24rpx;
  border-bottom: 1px solid #f5f5f5;
}
.item-time {
  font-size: 26rpx;
  color: #606266;
}
.item-value {
  font-size: 28rpx;
  font-weight: 500;
  color: #409eff;
}
.empty-list, .load-more, .no-more {
  text-align: center;
  padding: 40rpx;
  color: #909399;
  font-size: 26rpx;
}
</style>