<template>
  <div class="login-container">
    <!-- 动态背景装饰 -->
    <div class="background-effects">
      <div class="glow-orbit"></div>
      <div class="grid-lines"></div>
    </div>

    <el-form
      ref="loginForm"
      :model="loginForm"
      :rules="loginRules"
      class="login-form"
      auto-complete="on"
      label-position="left"
    >
      <!-- 系统标题区 -->
      <div class="brand-area">
        <div class="logo-wrapper">
          <svg class="server-icon" viewBox="0 0 24 24" fill="none" stroke="currentColor">
            <rect x="2" y="2" width="20" height="20" rx="2" stroke-width="1.5" />
            <path d="M7 7h10M7 12h10M7 17h6" stroke-width="1.8" stroke-linecap="round" />
            <circle cx="18" cy="17" r="1.5" fill="#00e5ff" stroke="none" />
          </svg>
        </div>
        <h2 class="system-title">数据中心机房监控</h2>
        <p class="system-subtitle">环境感知 · 智能反控</p>
      </div>

      <!-- 用户名 -->
      <el-form-item prop="username">
        <span class="input-icon">
          <svg-icon icon-class="user" />
        </span>
        <el-input
          v-model="loginForm.username"
          name="username"
          type="text"
          auto-complete="on"
          placeholder="用户名"
        />
      </el-form-item>

      <!-- 密码 -->
      <el-form-item prop="password">
        <span class="input-icon">
          <svg-icon icon-class="password" />
        </span>
        <el-input
          :type="pwdType"
          v-model="loginForm.password"
          name="password"
          auto-complete="on"
          placeholder="密码"
          @keyup.enter.native="handleLogin"
        />
        <span class="toggle-pwd" @click="showPwd">
          <svg-icon :icon-class="pwdType === 'password' ? 'eye' : 'eye-open'" />
        </span>
      </el-form-item>

      <!-- 登录按钮 -->
      <el-form-item class="login-action">
        <el-button
          :loading="loading"
          type="primary"
          class="login-btn"
          @click="handleLogin"
        >
          <span v-if="!loading">登 录</span>
          <span v-else>验证中...</span>
        </el-button>
      </el-form-item>

      <!-- 提示信息 -->
      <div class="login-tips">
        <div class="tip-item">
          <i class="el-icon-user"></i>
          <span>演示账号：admin / 123456</span>
        </div>
        <div class="tip-item">
          <i class="el-icon-cpu"></i>
          <span>STM32F103C8T6 · ESP8266-01 · DHT11 · MQ2 · Flame Sensor · LDR</span>
        </div>
        <div class="tip-item">
          <i class="el-icon-coin"></i>
          <span>SpringBoot 2.7 · Vue 2 · MySQL 5.7 · Redis · SpringSecurity · RABC</span>
        </div>
      </div>

    </el-form>

    <!-- 底部版权 -->
    <div class="footer-copyright">
      <span>© 2026 中小型数据中心监控系统 · 边缘智能网关</span>
    </div>
  </div>
</template>

<script>
import { isvalidUsername } from '@/utils/validate'

export default {
  name: 'Login',
  data() {
    const validateUsername = (rule, value, callback) => {
      if (!isvalidUsername(value)) {
        callback(new Error('请输入正确的用户名'))
      } else {
        callback()
      }
    }
    const validatePass = (rule, value, callback) => {
      if (value.length < 6) {
        callback(new Error('密码不能小于6位'))
      } else {
        callback()
      }
    }
    return {
      loginForm: {
        username: '',
        password: ''
      },
      loginRules: {
        username: [
          { required: true, trigger: 'blur', validator: validateUsername }
        ],
        password: [
          { required: true, trigger: 'blur', validator: validatePass }
        ]
      },
      loading: false,
      pwdType: 'password',
      redirect: undefined
    }
  },
  watch: {
    $route: {
      handler: function(route) {
        this.redirect = route.query && route.query.redirect
      },
      immediate: true
    }
  },
  methods: {
    showPwd() {
      this.pwdType = this.pwdType === 'password' ? 'text' : 'password'
    },
    handleLogin() {
      this.$refs.loginForm.validate(valid => {
        if (valid) {
          this.loading = true
          this.$store
            .dispatch('Login', this.loginForm)
            .then(() => {
              this.loading = false
              this.$router.push({ path: this.redirect || '/' })
            })
            .catch(() => {
              this.loading = false
            })
        } else {
          console.log('error submit!!')
          return false
        }
      })
    }
  }
}
</script>

<style lang="scss">
/* 全局覆盖 Element UI 样式 ———— 适配暗黑科技风 */
.login-container {
  .el-input {
    width: 100%;
    input {
      background: transparent !important;
      border: none;
      color: #e5f0ff;
      height: 52px;
      padding-left: 50px;
      font-size: 15px;
      letter-spacing: 0.5px;

      &::placeholder {
        color: rgba(180, 210, 255, 0.5);
        font-weight: 300;
      }

      /* 自动填充样式 */
      &:-webkit-autofill,
      &:-webkit-autofill:hover,
      &:-webkit-autofill:focus {
        -webkit-box-shadow: 0 0 0px 1000px #0f1a2b inset !important;
        -webkit-text-fill-color: #e5f0ff !important;
        caret-color: #00e5ff;
        transition: background-color 5000s ease-in-out 0s;
      }
    }
  }

  .el-form-item {
    margin-bottom: 28px;
    border: 1px solid rgba(0, 229, 255, 0.15);
    background: rgba(8, 20, 36, 0.65);
    backdrop-filter: blur(12px);
    border-radius: 16px;
    transition: all 0.25s ease;
    box-shadow: 0 8px 20px rgba(0, 0, 0, 0.3);

    &:hover {
      border-color: rgba(0, 229, 255, 0.4);
      box-shadow: 0 0 12px rgba(0, 180, 255, 0.2);
    }

    &.is-error {
      border-color: #ff5e7c;
      background: rgba(30, 10, 20, 0.7);
    }
  }

  .el-form-item__error {
    color: #ff7b93;
    padding-left: 15px;
    font-size: 12px;
    letter-spacing: 0.3px;
  }

  .el-button {
    border-radius: 40px;
    font-weight: 600;
    letter-spacing: 2px;
  }
}
</style>

<style lang="scss" scoped>
/* 设计变量 */
$deep-bg: #0a121e;
$accent-cyan: #00e5ff;
$accent-blue: #2a6df4;
$card-bg: rgba(12, 28, 45, 0.5);
$text-light: #ecf5ff;

.login-container {
  position: fixed;
  top: 0;
  left: 0;
  width: 100%;
  height: 100%;
  background: radial-gradient(circle at 30% 30%, #13203a, #050a14);
  display: flex;
  align-items: center;
  justify-content: center;
  font-family: 'Inter', 'Helvetica Neue', sans-serif;

  /* 背景特效层 */
  .background-effects {
    position: absolute;
    width: 100%;
    height: 100%;
    overflow: hidden;
    pointer-events: none;

    .glow-orbit {
      position: absolute;
      top: -20%;
      right: -10%;
      width: 70%;
      height: 70%;
      background: radial-gradient(circle, rgba(0, 229, 255, 0.08) 0%, transparent 70%);
      border-radius: 50%;
      filter: blur(60px);
      animation: slow-drift 20s infinite alternate;
    }

    .grid-lines {
      position: absolute;
      inset: 0;
      background-image: 
        linear-gradient(rgba(0, 229, 255, 0.03) 1px, transparent 1px),
        linear-gradient(90deg, rgba(0, 229, 255, 0.03) 1px, transparent 1px);
      background-size: 50px 50px;
      mask-image: radial-gradient(circle at 50% 50%, black 40%, transparent 90%);
    }
  }

  /* 登录卡片 */
  .login-form {
    position: relative;
    width: 580px;  /* 原 480px，加宽100px */
    max-width: 94%; /* 原 92%，略微增加边距余量 */
    padding: 40px 40px 32px; /* 水平内边距从 36px 增至 40px */
    background: $card-bg;
    backdrop-filter: blur(20px);
    border-radius: 36px;
    border: 1px solid rgba(0, 229, 255, 0.2);
    box-shadow: 0 30px 50px rgba(0, 0, 0, 0.6), 0 0 0 1px rgba(0, 200, 255, 0.1) inset;
    z-index: 10;
    animation: float-in 0.6s cubic-bezier(0.16, 1, 0.3, 1);
  }

  /* 品牌区域 */
  .brand-area {
    text-align: center;
    margin-bottom: 40px;

    .logo-wrapper {
      width: 64px;
      height: 64px;
      margin: 0 auto 16px;
      background: linear-gradient(145deg, #0e2642, #02101c);
      border-radius: 20px;
      display: flex;
      align-items: center;
      justify-content: center;
      border: 1px solid rgba(0, 229, 255, 0.5);
      box-shadow: 0 10px 18px -6px #00000080, 0 0 15px #00e5ff33;
      
      .server-icon {
        width: 36px;
        height: 36px;
        color: $accent-cyan;
        stroke: $accent-cyan;
      }
    }

    .system-title {
      font-size: 28px;
      font-weight: 700;
      margin: 0 0 8px;
      background: linear-gradient(135deg, #ffffff 0%, #aad0ff 100%);
      -webkit-background-clip: text;
      background-clip: text;
      color: transparent;
      letter-spacing: 1px;
      text-shadow: 0 2px 10px rgba(0, 160, 255, 0.3);
    }

    .system-subtitle {
      font-size: 13px;
      color: #9bb9e0;
      letter-spacing: 2px;
      text-transform: uppercase;
      opacity: 0.8;
      margin: 0;
    }
  }

  /* 输入框内部图标 */
  .input-icon {
    position: absolute;
    left: 18px;
    top: 50%;
    transform: translateY(-50%);
    color: $accent-cyan;
    font-size: 20px;
    z-index: 5;
    opacity: 0.9;
    filter: drop-shadow(0 0 6px #00e5ff66);
  }

  .toggle-pwd {
    position: absolute;
    right: 18px;
    top: 16px;
    color: #7f9fbf;
    cursor: pointer;
    font-size: 20px;
    z-index: 5;
    transition: color 0.2s;
    &:hover {
      color: $accent-cyan;
    }
  }

  /* 登录按钮 */
  .login-action {
    margin-top: 36px;
    margin-bottom: 20px;
  }

  .login-btn {
    width: 100%;
    height: 52px;
    border: none;
    background: linear-gradient(98deg, #005c97, #00a6ff);
    background-size: 200% 100%;
    border-radius: 40px !important;
    font-size: 18px;
    font-weight: 600;
    color: white;
    letter-spacing: 4px;
    box-shadow: 0 12px 25px -8px #005c97, 0 0 15px #00a6ff66;
    transition: all 0.3s;
    border: 1px solid rgba(255, 255, 255, 0.15);

    &:hover {
      background-position: 100% 0;
      box-shadow: 0 15px 28px -6px #0077be, 0 0 20px #00d0ff;
      transform: translateY(-2px);
    }

    &:active {
      transform: translateY(2px);
      box-shadow: 0 5px 15px #003f66;
    }

    ::v-deep span {
      display: inline-block;
    }
  }

  /* 提示区域 */
  .login-tips {
    margin-top: 18px;
    padding: 14px 12px;
    background: rgba(0, 20, 40, 0.4);
    border-radius: 18px;
    backdrop-filter: blur(4px);
    border: 1px dashed rgba(0, 200, 255, 0.25);

    .tip-item {
      display: flex;
      align-items: center;
      color: #b0d4ff;
      font-size: 13px;
      margin-bottom: 8px;
      
      i {
        margin-right: 10px;
        font-size: 16px;
        color: $accent-cyan;
        width: 20px;
        text-align: center;
      }

      &:last-child {
        margin-bottom: 0;
      }
    }
  }

  /* 底部版权 */
  .footer-copyright {
    position: absolute;
    bottom: 24px;
    left: 0;
    right: 0;
    text-align: center;
    color: #5c7f9c;
    font-size: 12px;
    letter-spacing: 1.5px;
    z-index: 5;
    opacity: 0.7;
    pointer-events: none;
  }
}

/* 动画 */
@keyframes slow-drift {
  0% { transform: translate(0, 0) scale(1); opacity: 0.5; }
  100% { transform: translate(-5%, 4%) scale(1.2); opacity: 0.8; }
}

@keyframes float-in {
  0% { opacity: 0; transform: translateY(30px) scale(0.97); }
  100% { opacity: 1; transform: translateY(0) scale(1); }
}

/* 适配小屏幕 —— 宽度小于600px时恢复紧凑布局 */
@media (max-width: 600px) {
  .login-form {
    width: 480px;      /* 小屏幕下回退至原宽度 */
    max-width: 92%;
    padding: 30px 22px 24px;
  }
  .system-title {
    font-size: 24px !important;
  }
}
</style>