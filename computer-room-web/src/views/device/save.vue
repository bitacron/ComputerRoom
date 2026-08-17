<template>
  <div class="app-container">
    <el-form label-width="120px">

      <el-form-item label="产品标识">
        <el-input v-model="device.productKey" :disabled="!!device.id"/>
      </el-form-item>
      <el-form-item label="设备唯一标识">
        <el-input v-model="device.deviceKey" :disabled="!!device.id"/>
      </el-form-item>
      <el-form-item label="设备名称">
        <el-input v-model="device.deviceName"/>
      </el-form-item>

      <el-form-item>
        <el-button :disabled="saveBtnDisabled" type="primary" @click="saveOrUpdate">保存</el-button>
      </el-form-item>
    </el-form>
  </div>
</template>
<script>
// 引入调用device.js文件
import deviceApi from '@/api/device'

export default {
    // 写核心代码部分
    data() { // 定义变量和初始值
        return {
            device: {
                deviceKey: '',
                deviceName: '',
                productKey: '',
                gasStatus: '',
                onlineStatus: 0
            },
            BASE_API: process.env.BASE_API, // 获取dev.en.js的地址
            imagecropperShow: false, // 上传组件是否显示
            imagecropperKey: 0, // 上传组件key值
            saveBtnDisabled: false, // 保存按钮是否禁用
            deviceOptions: []      // 设备下拉选项列表
        }
    },
    watch: { // 监听
        $route(to, from) { // 路由变化方式，路由发生变化，方法就会执行
            // console.log('watch $route')
            this.init()
        }
    },
    created() { // 页面渲染之前执行，一般调用methods定义的方法
        this.init()
    },
    methods: {
        init() {
            // 判断路径是否有Id值。如果有，说明是修改方法，获取信息；如果没有则是添加方法，不用获取信息
            if (this.$route.params && this.$route.params.id) {
                // 获取路径id值
                const id = this.$route.params.id
                // 调用根据id查询的方法
                this.getInfo(id)
            }
        },
        getInfo(id) {
            deviceApi.getDeviceInfo(id)
                .then(response => {
                    this.device = response.data
                })
        },
        saveOrUpdate() {
            if (!this.device.id) {
                // 无id，添加
                this.saveDevice()
            } else {
                // 有id，修改
                this.updateDevice()
            }
        },
        // 修改环境数据信息
        updateDevice() {
            deviceApi.updateDeviceInfo(this.device)
                .then(response => { // 修改成功
                    // 提示修改信息
                    this.$message({
                        type: 'success',
                        message: '修改成功!'
                    })
                    // 回到列表页面 路由跳转（即重定向）
                    this.$router.push({ path: '/device/table' })
                })
        },
        // 保存
        saveDevice() {
            deviceApi.addDevice(this.device)
                .then(response => { // 添加成功
                    // 提示添加信息
                    this.$message({
                        type: 'success',
                        message: '添加成功!'
                    })
                    // 回到列表页面 路由跳转（即重定向）
                    this.$router.push({ path: '/device/table' })
                })
        }
    }
}
</script>
