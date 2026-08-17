<template>
  <div class="app-container">
    <!--查询表单-->
    <el-form :inline="true" class="demo-form-inline">
      <el-form-item label="设备标识">
        <el-select
          v-model="sendQuery.deviceKey"
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
      <el-form-item label="添加时间">
        <el-date-picker
          v-model="sendQuery.begin"
          type="datetime"
          placeholder="选择开始时间"
          value-format="yyyy-MM-dd HH:mm:ss"
          default-time="00:00:00"
        />
      </el-form-item>

      <el-form-item>
        <el-date-picker
          v-model="sendQuery.end"
          type="datetime"
          placeholder="选择截止时间"
          value-format="yyyy-MM-dd HH:mm:ss"
          default-time="00:00:00"
        />
      </el-form-item>
      <el-button type="primary" icon="el-icon-search" @click="getList()">查询</el-button>
      <el-button type="default" @click="resetData()">清空</el-button>
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
      <el-table-column prop="topic" label="topic主题" width="280" />
      <el-table-column prop="payload" label="payload有效载荷" width="580" />
      <el-table-column prop="deviceKey" label="设备标识" width="100" />
      <el-table-column prop="sendTime" label="接收时间" width="160"/>
      <el-table-column label="操作" width="200" align="center">
        <template slot-scope="scope">
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
// 引入调用mqtt.js文件
import mqtt from '@/api/mqtt'
import deviceApi from '@/api/device'

export default {
    // 写核心代码部分
    // data:{
    // },
    data() { // 定义变量和初始值
        return {
            deviceOptions: [],      // 设备下拉选项列表
            listLoading: false, // 是否显示loading信息
            list: null, // 查询之后接口返回集合
            page: 1, // 当前页
            limit: 10, // 每页记录数
            total: 0, // 总记录数
            sendQuery: {} // 条件封装对象
        }
    },
    created() { // 页面渲染之前执行，一般调用methods定义的方法
        // 调用
        this.getList()
        this.fetchDeviceList()   // 加载设备下拉选项
    },
    methods: { // 创建具体的方法，调用mqtt.js定义的方法
        // mqtt列表的方法
        getList(page = 1) {
            this.page = page
            const params = {
                currentPage: this.page,
                pageSize: this.limit,
                ...this.sendQuery
            }
            mqtt.getSendListPage(params)
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
            this.sendQuery = {} // 表单输入框数据清空
            this.getList() // 刷新页面重新查询
        },
        // 根据id删除
        removeDataById(id) {
            // alert(id)
            this.$confirm('此操作将永久删除该记录, 是否继续?', '提示', {
                confirmButtonText: '确定',
                cancelButtonText: '取消',
                type: 'warning'
            }).then(() => { // 点击确定，执行then方法
                mqtt.deleteSendById(id)
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
