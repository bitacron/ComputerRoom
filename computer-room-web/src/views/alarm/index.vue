<template>
  <div class="app-container">
    <el-row :gutter="16" class="summary-row">
      <el-col :span="8">
        <el-card shadow="hover" class="summary-card" @click.native="filterByStatus(0)">
          <div class="summary-label">报警中</div>
          <div class="summary-value danger">{{ summary.alarming }}</div>
          <div class="summary-sub">其中紧急 {{ summary.urgent }} 条</div>
        </el-card>
      </el-col>
      <el-col :span="8">
        <el-card shadow="hover" class="summary-card">
          <div class="summary-label">今日报警</div>
          <div class="summary-value warning">{{ summary.today }}</div>
        </el-card>
      </el-col>
      <el-col :span="8">
        <el-card shadow="hover" class="summary-card" @click.native="filterByStatus(2)">
          <div class="summary-label">已处理</div>
          <div class="summary-value success">{{ summary.handled }}</div>
        </el-card>
      </el-col>
    </el-row>

    <el-form :inline="true" class="demo-form-inline">
      <el-form-item label="设备标识">
        <el-select
          v-model="alarmQuery.deviceKey"
          clearable
          filterable
          placeholder="请选择设备"
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
        <el-select v-model="alarmQuery.alarmType" clearable placeholder="报警类型">
          <el-option value="TEMP" label="温度"/>
          <el-option value="HUMI" label="湿度"/>
          <el-option value="GAS" label="烟雾"/>
          <el-option value="FLAME" label="火焰"/>
        </el-select>
      </el-form-item>
      <el-form-item>
        <el-select v-model="alarmQuery.alarmLevel" clearable placeholder="等级">
          <el-option :value="1" label="预警"/>
          <el-option :value="2" label="紧急"/>
        </el-select>
      </el-form-item>
      <el-form-item>
        <el-select v-model="alarmQuery.status" clearable placeholder="处理状态">
          <el-option :value="0" label="报警中"/>
          <el-option :value="1" label="已恢复"/>
          <el-option :value="2" label="已处理"/>
        </el-select>
      </el-form-item>
      <el-form-item label="报警时间">
        <el-date-picker
          v-model="alarmQuery.begin"
          type="datetime"
          placeholder="开始时间"
          value-format="yyyy-MM-dd HH:mm:ss"
          default-time="00:00:00"
        />
      </el-form-item>
      <el-form-item>
        <el-date-picker
          v-model="alarmQuery.end"
          type="datetime"
          placeholder="截止时间"
          value-format="yyyy-MM-dd HH:mm:ss"
          default-time="00:00:00"
        />
      </el-form-item>
      <el-button type="primary" icon="el-icon-search" @click="getList()">查询</el-button>
      <el-button type="default" @click="resetData()">清空</el-button>
    </el-form>

    <el-table
      v-loading="listLoading"
      :data="list"
      :row-class-name="tableRowClassName"
      element-loading-text="数据加载中"
      border
      fit
      highlight-current-row>
      <el-table-column
        label="序号"
        width="70"
        align="center">
        <template slot-scope="scope">
          {{ (page - 1) * limit + scope.$index + 1 }}
        </template>
      </el-table-column>
      <el-table-column prop="deviceKey" label="设备" width="140"/>
      <el-table-column label="报警类型" width="90" align="center">
        <template slot-scope="scope">
          {{ typeText(scope.row.alarmType) }}
        </template>
      </el-table-column>
      <el-table-column label="等级" width="80" align="center">
        <template slot-scope="scope">
          <el-tag :type="levelTag(scope.row.alarmLevel)" size="small">
            {{ levelText(scope.row.alarmLevel) }}
          </el-tag>
        </template>
      </el-table-column>
      <el-table-column prop="content" label="报警内容" min-width="220"/>
      <el-table-column prop="gmtCreate" label="开始时间" width="170"/>
      <el-table-column prop="recoverTime" label="恢复时间" width="170"/>
      <el-table-column label="状态" width="90" align="center">
        <template slot-scope="scope">
          <el-tag :type="statusTag(scope.row.status)" size="small">
            {{ statusText(scope.row.status) }}
          </el-tag>
        </template>
      </el-table-column>
      <el-table-column prop="handler" label="处理人" width="100"/>
      <el-table-column label="操作" width="180" align="center">
        <template slot-scope="scope">
          <el-button
            v-if="scope.row.status === 0 && hasPerm('alarm.handle')"
            type="warning"
            size="mini"
            @click="handleAlarm(scope.row.id)">处理</el-button>
          <el-button
            v-if="hasPerm('alarm.remove')"
            type="danger"
            size="mini"
            icon="el-icon-delete"
            @click="removeDataById(scope.row.id)">删除</el-button>
        </template>
      </el-table-column>
    </el-table>

    <el-pagination
      :current-page="page"
      :page-size="limit"
      :total="total"
      style="padding: 30px 0; text-align: center;"
      layout="total, prev, pager, next, jumper"
      @current-change="getList"
    />
  </div>
</template>

<script>
import alarmApi from '@/api/alarm'
import deviceApi from '@/api/device'

export default {
  name: 'AlarmIndex',
  data() {
    return {
      listLoading: false,
      list: [],
      page: 1,
      limit: 10,
      total: 0,
      alarmQuery: {},
      deviceOptions: [],
      summary: {
        alarming: 0,
        urgent: 0,
        today: 0,
        handled: 0
      },
      refreshTimer: null
    }
  },
  created() {
    this.fetchDeviceList()
    this.fetchSummary()
    this.getList()
  },
  mounted() {
    this.refreshTimer = setInterval(() => {
      this.fetchSummary()
      this.getList(this.page)
    }, 15000)
  },
  beforeDestroy() {
    if (this.refreshTimer) {
      clearInterval(this.refreshTimer)
    }
  },
  methods: {
    typeText(type) {
      const map = { TEMP: '温度', HUMI: '湿度', GAS: '烟雾', FLAME: '火焰' }
      return map[type] || type
    },
    levelText(level) {
      return level === 2 ? '紧急' : '预警'
    },
    levelTag(level) {
      return level === 2 ? 'danger' : 'warning'
    },
    statusText(status) {
      const map = { 0: '报警中', 1: '已恢复', 2: '已处理' }
      return map[status] || '未知'
    },
    statusTag(status) {
      const map = { 0: 'danger', 1: 'info', 2: 'success' }
      return map[status] || 'info'
    },
    tableRowClassName({ row }) {
      if (row.status !== 0) {
        return ''
      }
      return row.alarmLevel === 2 ? 'alarm-row-urgent' : 'alarm-row-warn'
    },
    filterByStatus(status) {
      this.alarmQuery = { ...this.alarmQuery, status }
      this.getList()
    },
    getList(page = 1) {
      this.page = page
      this.listLoading = true
      const params = {
        currentPage: this.page,
        pageSize: this.limit,
        ...this.alarmQuery
      }
      alarmApi.getAlarmListPage(params)
        .then(response => {
          this.list = response.data.records
          this.total = response.data.total
        })
        .catch(error => {
          console.log(error)
        })
        .finally(() => {
          this.listLoading = false
        })
    },
    fetchSummary() {
      alarmApi.getAlarmSummary()
        .then(response => {
          this.summary = Object.assign({
            alarming: 0,
            urgent: 0,
            today: 0,
            handled: 0
          }, response.data)
        })
        .catch(error => {
          console.log(error)
        })
    },
    resetData() {
      this.alarmQuery = {}
      this.getList()
      this.fetchSummary()
    },
    handleAlarm(id) {
      this.$confirm('确认将该报警标记为已处理？处理后 5 分钟内同类型不会重复告警。', '提示', {
        confirmButtonText: '确定',
        cancelButtonText: '取消',
        type: 'warning'
      }).then(() => {
        return alarmApi.handleAlarm(id)
      }).then(() => {
        this.$message.success('处理成功')
        this.getList(this.page)
        this.fetchSummary()
      }).catch((response) => {
        if (response === 'cancel') {
          this.$message({ type: 'info', message: '已取消' })
        }
      })
    },
    removeDataById(id) {
      this.$confirm('此操作将删除该报警记录, 是否继续?', '提示', {
        confirmButtonText: '确定',
        cancelButtonText: '取消',
        type: 'warning'
      }).then(() => {
        return alarmApi.deleteAlarmById(id)
      }).then(() => {
        this.$message.success('删除成功')
        this.getList()
        this.fetchSummary()
      }).catch((response) => {
        if (response === 'cancel') {
          this.$message({ type: 'info', message: '已取消删除' })
        }
      })
    },
    async fetchDeviceList() {
      try {
        const response = await deviceApi.getDeviceSelect()
        this.deviceOptions = response.data.map(item => ({
          value: item.deviceKey,
          label: item.deviceName || item.deviceKey
        }))
      } catch (error) {
        console.error('获取设备列表失败', error)
      }
    }
  }
}
</script>

<style scoped>
.summary-row {
  margin-bottom: 16px;
}
.summary-card {
  cursor: pointer;
  text-align: center;
}
.summary-label {
  color: #909399;
  font-size: 14px;
  margin-bottom: 8px;
}
.summary-value {
  font-size: 28px;
  font-weight: 600;
  line-height: 1.2;
}
.summary-value.danger {
  color: #F56C6C;
}
.summary-value.warning {
  color: #E6A23C;
}
.summary-value.success {
  color: #67C23A;
}
.summary-sub {
  margin-top: 6px;
  font-size: 12px;
  color: #F56C6C;
}
</style>

<style>
.el-table .alarm-row-urgent {
  background: #fef0f0;
}
.el-table .alarm-row-warn {
  background: #fdf6ec;
}
</style>
