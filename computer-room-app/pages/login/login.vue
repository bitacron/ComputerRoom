<template>
  <view class="login-container">
    <view class="login-card">
      <!-- Logo区域 -->
      <view class="logo-area">
        <view class="logo-icon-box">
          <text class="logo-icon-main">🗄️</text>
          <text class="logo-icon-sub">⚡</text>
        </view>
        <text class="app-name">机房监控系统</text>
        <text class="app-sub">CRMS</text>
      </view>

      <!-- 表单区域 -->
      <view class="form-area">
        <!-- 账号输入 -->
        <view class="input-wrapper">
          <view class="input-icon">👤</view>
          <input 
            class="input-field" 
            v-model="username" 
            placeholder="请输入用户名" 
            placeholder-class="placeholder"
          />
        </view>

        <!-- 密码输入（猴子切换） -->
        <view class="input-wrapper">
          <view class="input-icon">🔒</view>
          <input 
            class="input-field" 
            :type="showPassword ? 'text' : 'password'" 
            v-model="password" 
            placeholder="请输入密码" 
            placeholder-class="placeholder"
            @confirm="handleLogin"
          />
          <view class="eye-icon" @click="showPassword = !showPassword">
            {{ showPassword ? '🙉' : '🙈' }}
          </view>
        </view>

        <!-- 登录按钮 -->
        <button 
          class="login-btn" 
          :loading="loading" 
          hover-class="login-btn-hover"
          @click="handleLogin"
        >
          <text v-if="!loading">登 录</text>
          <text v-else>登录中...</text>
        </button>

        <!-- 演示账号提示 -->
        <view class="demo-tip">
          <text class="tip-icon">🔐</text>
          <text>演示账号: admin / 123456</text>
        </view>
      </view>
    </view>
  </view>
</template>

<script>
import request from '@/utils/request.js';
import { loadPermissions } from '@/utils/permission.js';

export default {
  data() {
    return {
      username: 'admin',
      password: '123456',
      loading: false,
      showPassword: false
    };
  },
  onShow() {
    const token = uni.getStorageSync('access_token');
    if (token) {
      uni.switchTab({ url: '/pages/realTime/realTime' });
    }
  },
  methods: {
    async handleLogin() {
      if (!this.username || !this.password) {
        uni.showToast({ title: '请输入账号和密码', icon: 'none' });
        return;
      }
      this.loading = true;
      try {
        const res = await request({
          url: '/admin/access/login',
          method: 'POST',
          data: {
            username: this.username,
            password: this.password
          }
        });
        if (res.data && res.data.token) {
          uni.setStorageSync('access_token', res.data.token);
          await loadPermissions();
          uni.showToast({ title: '登录成功', icon: 'success' });
          uni.switchTab({ url: '/pages/realTime/realTime' });
        } else {
          throw new Error('登录返回数据异常');
        }
      } catch (err) {
        console.error(err);
        uni.showToast({ title: err.msg || '登录失败，请检查账号密码', icon: 'none' });
      } finally {
        this.loading = false;
      }
    }
  }
};
</script>

<style scoped>
/* 整体页面背景 —— 柔和浅灰色 */
.login-container {
  display: flex;
  align-items: center;
  justify-content: center;
  min-height: 100vh;
  background: #f5f7fa;  /* 浅灰底，比纯白更有层次 */
  padding: 30rpx;
  box-sizing: border-box;
}

/* 登录卡片 —— 纯白圆角，轻微阴影 */
.login-card {
  width: 100%;
  max-width: 650rpx;
  background-color: #ffffff;
  border-radius: 48rpx;
  padding: 60rpx 40rpx 50rpx;
  box-shadow: 0 20rpx 60rpx rgba(0, 0, 0, 0.08), 0 0 0 1rpx rgba(0, 0, 0, 0.02);
  transition: all 0.3s;
}

/* Logo区域 */
.logo-area {
  display: flex;
  flex-direction: column;
  align-items: center;
  margin-bottom: 50rpx;
}

.logo-icon-box {
  position: relative;
  margin-bottom: 20rpx;
}

.logo-icon-main {
  font-size: 100rpx;
  filter: drop-shadow(0 8rpx 16rpx rgba(59, 130, 246, 0.15));
}

.logo-icon-sub {
  position: absolute;
  bottom: -8rpx;
  right: -20rpx;
  font-size: 48rpx;
  background: #ffffff;
  padding: 8rpx;
  border-radius: 24rpx;
  box-shadow: 0 0 0 3rpx #3B82F6, 0 8rpx 16rpx rgba(0, 0, 0, 0.08);
}

.app-name {
  font-size: 38rpx;
  font-weight: 700;
  color: #1e293b;  /* 深灰蓝，保持专业感 */
  letter-spacing: 2rpx;
  margin-top: 12rpx;
}

.app-sub {
  font-size: 26rpx;
  color: #64748b;
  letter-spacing: 8rpx;
  font-weight: 500;
  margin-top: 6rpx;
  text-transform: uppercase;
}

/* 表单区域 */
.form-area {
  margin-top: 20rpx;
}

/* 输入框包装器 */
.input-wrapper {
  display: flex;
  align-items: center;
  background: #f8fafc;  /* 极浅灰背景 */
  border-radius: 60rpx;
  padding: 0 30rpx;
  margin-bottom: 30rpx;
  border: 1rpx solid #e2e8f0;
  transition: all 0.25s ease;
}

.input-wrapper:focus-within {
  background: #ffffff;
  border-color: #3B82F6;
  box-shadow: 0 0 0 6rpx rgba(59, 130, 246, 0.08);
}

.input-icon {
  font-size: 40rpx;
  margin-right: 20rpx;
  color: #64748b;
  width: 40rpx;
  text-align: center;
}

.input-field {
  flex: 1;
  height: 96rpx;
  font-size: 30rpx;
  color: #0f172a;
  background: transparent;
  outline: none;
}

.placeholder {
  color: #94a3b8;
  font-weight: 400;
}

/* 猴子切换图标 */
.eye-icon {
  font-size: 44rpx;
  padding: 10rpx 0 10rpx 20rpx;
  color: #64748b;
  transition: color 0.2s;
}
.eye-icon:active {
  color: #3B82F6;
}

/* 登录按钮 */
.login-btn {
  width: 100%;
  height: 100rpx;
  line-height: 100rpx;
  background: #3B82F6;  /* 纯色蓝，简洁商务 */
  color: #ffffff;
  font-size: 36rpx;
  font-weight: 600;
  border-radius: 60rpx;
  border: none;
  box-shadow: 0 12rpx 24rpx -8rpx rgba(59, 130, 246, 0.3);
  transition: all 0.2s;
  letter-spacing: 6rpx;
  margin-top: 30rpx;
}

.login-btn-hover {
  background: #2563eb;
  transform: scale(0.98);
  box-shadow: 0 6rpx 16rpx -6rpx rgba(59, 130, 246, 0.4);
}

/* 演示提示 */
.demo-tip {
  display: flex;
  align-items: center;
  justify-content: center;
  margin-top: 40rpx;
  font-size: 24rpx;
  color: #64748b;
  background: #f8fafc;
  padding: 16rpx 24rpx;
  border-radius: 60rpx;
  border: 1rpx dashed #cbd5e1;
}

.tip-icon {
  margin-right: 10rpx;
  font-size: 28rpx;
}
</style>