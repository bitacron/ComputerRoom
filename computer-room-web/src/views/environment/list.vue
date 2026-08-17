<template>
  <div class="app-container">

    <!--查询表单-->
    <el-form :inline="true" class="demo-form-inline">
      <el-form-item>
        <el-select v-model="environmentQuery.source" clearable placeholder="数据来源">
          <el-option :value="1" label="硬件上报"/>
          <el-option :value="2" label="手动记录"/>
        </el-select>
      </el-form-item>
      <el-form-item label="添加时间">
        <el-date-picker
          v-model="environmentQuery.begin"
          type="datetime"
          placeholder="选择开始时间"
          value-format="yyyy-MM-dd HH:mm:ss"
          default-time="00:00:00"
        />
      </el-form-item>
      <!-- 新增：设备标识下拉框 -->
      <el-form-item label="设备标识">
        <el-select
          v-model="environmentQuery.deviceKey"
          clearable
          placeholder="请选择设备"
          filterable  
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
        <el-date-picker
          v-model="environmentQuery.end"
          type="datetime"
          placeholder="选择截止时间"
          value-format="yyyy-MM-dd HH:mm:ss"
          default-time="00:00:00"
        />
      </el-form-item>
      <el-button type="primary" icon="el-icon-search" @click="getList()">查询</el-button>
      <el-button type="default" @click="resetData()">清空</el-button>
      <el-button type="success" icon="el-icon-download" @click="exportData()">导出Excel</el-button>
    </el-form>
    <!-- 表格 -->
    <el-table
      v-loading="listLoading"
      :data="list"
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
      <el-table-column label="数据来源" width="80">
        <template slot-scope="scope">
          {{ scope.row.source===1?'硬件上报':'手动记录' }}
        </template>
      </el-table-column>
      <el-table-column prop="deviceKey" label="上报设备" width="100" />
      <el-table-column prop="temperature" label="温度" width="50" />
      <el-table-column prop="humidity" label="湿度" width="50" />
      <el-table-column prop="gasPpm" label="烟雾浓度" width="80" />
      <el-table-column label="烟雾状态" width="80">
        <template slot-scope="scope">
          {{ scope.row.gasStatus===1?'有烟雾':'无烟雾' }}
        </template>
      </el-table-column>
      <el-table-column prop="lightPercentage" label="光照亮度" width="80" />
      <el-table-column label="光照状态" width="80">
        <template slot-scope="scope">
          {{ scope.row.lightStatus===1?'无光':'有光' }}
        </template>
      </el-table-column>
      <el-table-column prop="flamePercentage" label="明火概率" width="80" />
      <el-table-column label="火焰状态" width="80">
        <template slot-scope="scope">
          {{ scope.row.flameStatus===1?'有明火':'无明火' }}
        </template>
      </el-table-column>
      <el-table-column label="蜂鸣器报警状态" width="80">
        <template slot-scope="scope">
          {{ scope.row.alarmStatus===1?'报警':'未报警' }}
        </template>
      </el-table-column>
      <el-table-column label="散热设备开关" width="80">
        <template slot-scope="scope">
          {{ scope.row.fanStatus===1?'开启':'关闭' }}
        </template>
      </el-table-column>
      <el-table-column label="led开关" width="80">
        <template slot-scope="scope">
          {{ scope.row.ledStatus===1?'开启':'关闭' }}
        </template>
      </el-table-column>
      <el-table-column prop="gmtMeasurement" label="检测时间" width="160"/>
      <el-table-column prop="gmtCreate" label="添加时间" width="160"/>
      <el-table-column label="操作" width="200" align="center">
        <template slot-scope="scope">
          <router-link :to="'/environment/edit/'+scope.row.id">
            <el-button type="primary" size="mini" icon="el-icon-edit">修改</el-button>
          </router-link>
          <el-button
            type="danger"
            size="mini"
            icon="el-icon-delete"
            @click="removeDataById(scope.row.id)">删除</el-button>
        </template>
      </el-table-column>
    </el-table>
    <!-- 分页 -->
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
// 引入调用environment.js文件
import environment from '@/api/environment'
import deviceApi from '@/api/device'

export default {
    // 写核心代码部分
    // data:{
    // },
    data() { // 定义变量和初始值
        return {
            listLoading: false, // 是否显示loading信息
            list: null, // 查询之后接口返回集合
            page: 1, // 当前页
            limit: 10, // 每页记录数
            total: 0, // 总记录数
            environmentQuery: {}, // 条件封装对象
            deviceOptions: []      // 设备下拉选项列表
        }
    },
    created() { // 页面渲染之前执行，一般调用methods定义的方法
        // 调用
        this.getList()
        this.fetchDeviceList()   // 加载设备下拉选项
    },
    methods: { // 创建具体的方法，调用environment.js定义的方法
        // 环境数据的方法
        getList(page = 1) {
            this.page = page
            const params = {
                currentPage: this.page,
                pageSize: this.limit,
                ...this.environmentQuery
            }
            environment.getEnvironmentListPage(params)
                .then(response => { // 请求成功
                    // response接口返回的数据
                    // console.log(response)
                    this.list = response.data.records
                    this.total = response.data.total
                    console.log(this.list)
                    console.log(this.total)
                })
                .catch(error => { // 请求失败
                    console.log(error)
                })
        },
        resetData() { // 清除
            this.environmentQuery = {} // 表单输入框数据清空
            this.getList() // 刷新页面重新查询
        },
        // 导出Excel方法
        exportData() {
            const params = { ...this.environmentQuery }
            environment.exportEnvironmentList(params)
                .then(response => {
                    // 检查响应头，如果后端返回了错误文本
                    const contentType = response.headers['content-type']
                    if (contentType && contentType.includes('text/plain')) {
                        // 读取错误文本并提示
                        const reader = new FileReader()
                        reader.onload = () => {
                            this.$message.error(reader.result)
                        }
                        reader.readAsText(response.data)
                        return
                    }
                    // 正常导出
                    const blob = new Blob([response.data], { type: contentType })
                    const link = document.createElement('a')
                    const url = URL.createObjectURL(blob)
                    link.href = url
                    link.download = '环境监测数据.xlsx' // 文件名
                    link.click()
                    URL.revokeObjectURL(url)
                })
                .catch(error => {
                    console.error('导出失败', error)
                    this.$message.error('导出失败，请重试')
                })
        },
        // 根据id删除
        removeDataById(id) {
            // alert(id)
            this.$confirm('此操作将永久删除该环境记录, 是否继续?', '提示', {
                confirmButtonText: '确定',
                cancelButtonText: '取消',
                type: 'warning'
            }).then(() => { // 点击确定，执行then方法
                environment.deleteEnvironmentById(id)
                    .then(response => { // 删除成功
                        // 提示信息
                        this.$message({
                            type: 'success',
                            message: '删除成功!'
                        })
                        // 回到列表页面
                        this.getList()
                    })
            })// 点击确定，执行catch方法
            .catch((response) => {
                if (response === 'cancel') {
                    this.$message({
                        type: 'info',
                        message: '已取消删除'
                    })
                } else {
                    this.$message({
                    type: 'error',
                    message: '删除失败'
                    })
                }
            })
        },
      async fetchDeviceList() {
        try {
          // 调用接口，返回设备列表
          const response = await deviceApi.getDeviceSelect() // 根据实际接口路径调整
          this.deviceOptions = response.data.map(item => ({
            value: item.deviceKey,
            label: item.deviceKey  // 也可以显示更多信息，如 item.name
          }))
        } catch (error) {
          console.error('获取设备列表失败', error)
          this.$message.error('设备列表加载失败')
        }
      }
    }
}
</script>
