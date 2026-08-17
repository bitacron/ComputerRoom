<template>
  <div class="app-container">
    <el-form ref="user" :model="user" :rules="validateRules" label-width="120px">
      <el-form-item label="用户名" prop="username">
        <el-input v-model="user.username"/>
      </el-form-item>
      <el-form-item label="用户昵称">
        <el-input v-model="user.nickName"/>
      </el-form-item>
     
      <el-form-item v-if="!user.id" label="用户密码" prop="password">
        <el-input v-model="user.password"/>
      </el-form-item>

      <el-form-item label="手机号">
        <el-input v-model="user.phone"/>
      </el-form-item>

      <el-form-item label="邮箱">
        <el-input v-model="user.email"/>
      </el-form-item>

      <el-form-item label="性别">
        <el-select v-model="user.gender" clearable placeholder="请选择">
          <el-option :value="1" label="男"/>
          <el-option :value="0" label="女"/>
        </el-select>
      </el-form-item>
      <!-- 头像选择区域 -->
      <el-form-item label="头像">
        <div style="display: flex; align-items: center;">
          <el-image
            :src="user.avatar || defaultAvatar"
            style="width: 80px; height: 80px; border-radius: 50%; margin-right: 20px;"
            fit="cover"
          >
            <div slot="error" style="width: 80px; height: 80px; background: #f5f7fa; border-radius: 50%; display: flex; align-items: center; justify-content: center; color: #999;">
              无图
            </div>
          </el-image>
          <el-button type="primary" plain @click="openAvatarDialog">编辑头像</el-button>
        </div>
      </el-form-item>
      <el-form-item>
        <el-button :disabled="saveBtnDisabled" type="primary" @click="saveOrUpdate">保存</el-button>
      </el-form-item>
    </el-form>

        <!-- 头像选择弹窗 -->
    <el-dialog :visible.sync="avatarDialogVisible" title="选择头像" width="400px" append-to-body>
      <div class="avatar-list">
        <div
          v-for="(url, index) in avatarOptions"
          :key="index"
          class="avatar-item"
          @click="selectAvatar(url)"
        >
          <el-image :src="url" fit="cover" style="width: 80px; height: 80px; border-radius: 50%;">
            <div slot="error" style="width: 80px; height: 80px; background: #f5f7fa; border-radius: 50%; display: flex; align-items: center; justify-content: center; color: #999;">
              加载失败
            </div>
          </el-image>
        </div>
      </div>
    </el-dialog>
  </div>
</template>

<script>

import userApi from '@/api/access/user'

const defaultForm = {
  username: '',
  nickName: '',
  password: '',
  phone: '',
  email: '',
  gender: ''
}

const validatePass = (rule, value, callback) => {
  if (value.length < 6) {
    callback(new Error('密码不能小于6位'))
  } else {
    callback()
  }
}

export default {
  data() {
    return {
      user: defaultForm,
      saveBtnDisabled: false, // 保存按钮是否禁用,
      validateRules: {
        username: [{ required: true, trigger: 'blur', message: '用户名必须输入' }],
        password: [{ required: true, trigger: 'blur', validator: validatePass }]
      },
            // 默认头像（当 avatar 为空时显示）
      defaultAvatar: 'https://wpimg.wallstcn.com/f778738c-e4f8-4870-b634-56703b4acafe.gif',
      // 可选头像列表
      avatarOptions: [
        'https://wpimg.wallstcn.com/f778738c-e4f8-4870-b634-56703b4acafe.gif',
        'http://thirdwx.qlogo.cn/mmopen/vi_32/Q0j4TwGTfTJPRMhduLbDgmj9mud9LUibylfb09iacQ1lXW3znP1KVAhrhMRR84AgYLzOowk4KKFSe1nj65eqnXgg/132',
        'https://online-education-chn.oss-cn-beijing.aliyuncs.com/avatar/default.jpg',
        'https://cube.elemecdn.com/9/c2/f0ee8a3c7c9638a54940382568c9dpng.png',
        'https://img1.baidu.com/it/u=3343919120,3675334302&fm=253&fmt=auto&app=138&f=JPEG?w=500&h=500',
        'https://pic2.zhimg.com/v2-5efdd705321f6d77ce77e9030a6c8153_r.jpg?source=1940ef5c'
      ],
      avatarDialogVisible: false, // 控制弹窗显示
    }
  },

  // 监听器
  watch: {
    $route(to, from) {
      console.log('路由变化......')
      console.log(to)
      console.log(from)
      this.init()
    }
  },

  // 生命周期方法（在路由切换，组件不变的情况下不会被调用）
  created() {
    console.log('form created ......')
    this.init()
  },

  methods: {

    // 表单初始化
    init() {
      //debugger
      if (this.$route.params && this.$route.params.id) {
        const id = this.$route.params.id
        this.fetchDataById(id)
      } else {
        // 对象拓展运算符：拷贝对象，而不是赋值对象的引用
        this.user = { ...defaultForm }
      }
    },
    // 打开头像选择弹窗
    openAvatarDialog() {
      this.avatarDialogVisible = true
    },
    // 选中头像
    selectAvatar(url) {
      this.user.avatar = url
      this.avatarDialogVisible = false
    },
    saveOrUpdate() {
      this.$refs.user.validate(valid => {
        if (valid) {
          this.saveBtnDisabled = true // 防止表单重复提交
          if (!this.user.id) {
            this.saveData()
          } else {
            this.updateData()
          }
        }
      })
    },

    // 新增用户
    saveData() {
      userApi.save(this.user).then(response => {
        if (response.code === 20000) {
          this.$message({
            type: 'success',
            message: response.msg || '保存成功'
          })
          this.$router.push({ path: '/access/user/list' })
        }
      }).finally(() => {
        this.saveBtnDisabled = false
      })
    },

    // 根据 id 更新记录
    updateData() {
      userApi.updateById(this.user).then(response => {
        if (response.code === 20000) {
          this.$message({
            type: 'success',
            message: response.msg || '更新成功'
          })
          this.$router.push({ path: '/access/user/list' })
        }
      }).finally(() => {
        this.saveBtnDisabled = false
      })
    },

    // 根据id查询记录
    fetchDataById(id) {
      userApi.getById(id).then(response => {
        //debugger
        this.user = response.data.item
      })
    }

  }
}
</script>
