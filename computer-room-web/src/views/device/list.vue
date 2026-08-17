<template>
  <div class="app-container">

    <!--查询表单-->
    <el-form :inline="true" class="demo-form-inline">
      <el-form-item>
        <el-select v-model="deviceQuery.source" clearable placeholder="在线状态">
          <el-option :value="0" label="在线"/>
          <el-option :value="1" label="离线"/>
        </el-select>
      </el-form-item>
      <el-form-item>
        <el-input v-model="deviceQuery.deviceKey" placeholder="设备唯一标识" clearable />
      </el-form-item>
      <el-form-item>
        <el-input v-model="deviceQuery.deviceName" placeholder="设备名称" clearable />
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

      <el-table-column prop="productKey" label="产品标识" width="100" />
      <el-table-column prop="deviceKey" label="设备唯一标识" width="150" />
      <el-table-column prop="deviceName" label="设备名称" width="150" />

      <el-table-column label="在线状态" width="80">
        <template slot-scope="scope">
          <span 
            :class="['status-dot', scope.row.onlineStatus === 1 ? 'online' : 'offline']">
          </span>
          {{ scope.row.onlineStatus===1?'在线':'离线' }}
        </template>
      </el-table-column>

      <el-table-column prop="lastOnlineTime" label="最后上线时间" width="160"/>
      <el-table-column prop="lastOfflineTime" label="最后离线时间" width="160"/>
      <el-table-column prop="lastActiveTime" label="最后活跃时间" width="160"/>
      <el-table-column label="操作" width="200" align="center">
        <template slot-scope="scope">
          <router-link :to="'/device/edit/'+scope.row.id">
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
// 引入调用device.js文件
import device from '@/api/device'

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
            deviceQuery: {}, // 条件封装对象
            deviceOptions: []      // 设备下拉选项列表
        }
    },
    created() { // 页面渲染之前执行，一般调用methods定义的方法
        // 调用
        this.getList()
    },
    methods: { // 创建具体的方法，调用device.js定义的方法
        // 环境数据的方法
        getList(page = 1) {
            this.page = page
            const params = {
                currentPage: this.page,
                pageSize: this.limit,
                ...this.deviceQuery
            }
            device.getDeviceListPage(params)
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
            this.deviceQuery = {} // 表单输入框数据清空
            this.getList() // 刷新页面重新查询
        },
        // 根据id删除
        removeDataById(id) {
            // alert(id)
            this.$confirm('此操作将永久删除该环境记录, 是否继续?', '提示', {
                confirmButtonText: '确定',
                cancelButtonText: '取消',
                type: 'warning'
            }).then(() => { // 点击确定，执行then方法
                device.deleteDeviceById(id)
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
        }
    }
}
</script>

<style scoped>
.status-dot {
  display: inline-block;
  width: 8px;
  height: 8px;
  border-radius: 50%;
  margin-right: 6px;
  vertical-align: middle;
}
.online {
  background-color: #67c23a;  /* 绿色 */
}
.offline {
  background-color: #f56c6c;  /* 红色 */
}
</style>