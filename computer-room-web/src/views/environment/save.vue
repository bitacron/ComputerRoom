<template>
  <div class="app-container">
    <el-form label-width="120px">

            <!-- 新增：设备标识下拉框 -->
      <el-form-item label="设备标识">
        <el-select
          v-model="environment.deviceKey"
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
      <el-form-item label="温度">
        <el-input v-model="environment.temperature"/>
      </el-form-item>
      <el-form-item label="湿度">
        <el-input v-model="environment.humidity"/>
      </el-form-item>
      <el-form-item label="烟雾浓度">
        <el-input v-model="environment.gasPpm"/>
      </el-form-item>
      <el-form-item label="烟雾状态">
        <el-select v-model="environment.gasStatus" clearable placeholder="请选择">
          <el-option :value="1" label="有烟雾"/>
          <el-option :value="0" label="无烟雾"/>
        </el-select>
      </el-form-item>
      <el-form-item label="光照亮度">
        <el-input v-model="environment.lightPercentage"/>
      </el-form-item>
      <el-form-item label="光照状态">
        <el-select v-model="environment.lightStatus" clearable placeholder="请选择">
          <el-option :value="1" label="无光"/>
          <el-option :value="0" label="有光"/>
        </el-select>
      </el-form-item>
      <el-form-item label="明火概率">
        <el-input v-model="environment.flamePercentage"/>
      </el-form-item>
      <el-form-item label="火焰状态">
        <el-select v-model="environment.flameStatus" clearable placeholder="请选择">
          <el-option :value="1" label="有明火"/>
          <el-option :value="0" label="无明火"/>
        </el-select>
      </el-form-item>
      <el-form-item label="蜂鸣器报警状态">
        <el-select v-model="environment.alarmStatus" clearable placeholder="请选择">
          <!--数据类型一定要和取出的json中的一致，否则没法回填.因此，这里value使用动态绑定的值，保证其数据类型是number-->
          <el-option :value="1" label="报警"/>
          <el-option :value="0" label="未报警"/>
        </el-select>
      </el-form-item>
      <el-form-item label="散热设备开关">
        <el-select v-model="environment.fanStatus" clearable placeholder="请选择">
          <el-option :value="1" label="开启"/>
          <el-option :value="0" label="关闭"/>
        </el-select>
      </el-form-item>
      <el-form-item label="led灯开关">
        <el-select v-model="environment.ledStatus" clearable placeholder="请选择">
          <el-option :value="1" label="开启"/>
          <el-option :value="0" label="关闭"/>
        </el-select>
      </el-form-item>
      <el-form-item label="检测时间">
        <el-date-picker
          v-model="environment.gmtMeasurement"
          type="datetime"
          placeholder="选择开始时间"
          value-format="yyyy-MM-dd HH:mm:ss"
          default-time="00:00:00"
        />
      </el-form-item>
      <el-form-item>
        <el-button :disabled="saveBtnDisabled" type="primary" @click="saveOrUpdate">保存</el-button>
      </el-form-item>
    </el-form>
  </div>
</template>
<script>
// 引入调用environment.js文件
import environmentApi from '@/api/environment'
import deviceApi from '@/api/device'

export default {
    // 写核心代码部分
    data() { // 定义变量和初始值
        return {
            environment: {
                temperature: '',
                humidity: '',
                gasPpm: '',
                gasStatus: '',
                lightStatus: '',
                flameStatus: '',
                lightPercentage: '',
                flamePercentage: '',
                alarmStatus: '',
                fan: '',
                ledStatus: '',
                source: 2
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
        this.fetchDeviceList()   // 加载设备下拉选项
    },
    methods: { // 创建具体的方法，调用teacher.js定义的方法
        close() { // 关闭上传弹窗的方法
            this.imagecropperShow = false // 上传弹框的值等于false，就关闭弹窗了
            this.imagecropperKey = this.imagecropperKey + 1 // 上传组件初始化
        },
        cropSuccess(data) { // 上传成功之后的方法
            // console.log(data)
            // 关弹窗
            this.imagecropperShow = false
            // 上传之后的接口返回图片的地址
            this.teacher.avatar = data.url
            this.imagecropperKey = this.imagecropperKey + 1 // 上传组件初始化
        },
        init() {
            // 判断路径是否有Id值。如果有，说明是修改方法，获取信息；如果没有则是添加方法，不用获取信息
            if (this.$route.params && this.$route.params.id) {
                // 获取路径id值
                const id = this.$route.params.id
                // 调用根据id查询的方法
                this.getInfo(id)
            } else { // 路径没有id值，做添加
                // 清空表单
                // this.teacher = { avatar: 'https://online-education-chn.oss-cn-beijing.aliyuncs.com/avatar/default.jpg',}
            }
        },
        getInfo(id) {
            environmentApi.getEnvironmentInfo(id)
                .then(response => {
                    this.environment = response.data
                })
        },
        saveOrUpdate() { // 修改和添加共同使用这个方法
            // 判断是修改还是添加
            // 根据teacher对象是否有id进行判断，没有id就是添加，有id就是修改
            if (!this.environment.id) {
                // 无id，添加
                this.saveEnvironment()
            } else {
                // 有id，修改
                this.updateEnvironment()
            }
        },
        // 修改环境数据信息
        updateEnvironment() {
            environmentApi.updateEnvironmentInfo(this.environment)
                .then(response => { // 修改成功
                    // 提示修改信息
                    this.$message({
                        type: 'success',
                        message: '修改成功!'
                    })
                    // 回到列表页面 路由跳转（即重定向）
                    this.$router.push({ path: '/environment/table' })
                })
        },
        // 保存
        saveEnvironment() {
            environmentApi.addEnvironment(this.environment)
                .then(response => { // 添加成功
                    // 提示添加信息
                    this.$message({
                        type: 'success',
                        message: '添加成功!'
                    })
                    // 回到列表页面 路由跳转（即重定向）
                    this.$router.push({ path: '/environment/table' })
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
