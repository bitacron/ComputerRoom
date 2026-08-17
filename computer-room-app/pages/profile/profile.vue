<template>
  <view class="profile-container">
    <view class="user-card">
      <view class="avatar-area">
        <image class="avatar" :src="userInfo.avatar || '/static/default_avatar.jpg'" mode="aspectFill"></image>
        <view class="user-name">{{ userInfo.name || '用户' }}</view>
        <view class="user-account">账号：{{ userInfo.code }}</view>
      </view>
    </view>

    <view class="info-card">
      <view class="info-item">
        <text class="label">手机号</text>
        <text class="value">{{ userInfo.phone || '未绑定' }}</text>
      </view>
      <view class="info-item">
        <text class="label">邮箱</text>
        <text class="value">{{ userInfo.email || '未绑定' }}</text>
      </view>
      <view class="info-item" v-if="userInfo.gender">
        <text class="label">性别</text>
        <text class="value">{{ userInfo.gender === 1 ? '男' : (userInfo.gender === 0 ? '女' : '未知') }}</text>
      </view>
      <view class="info-item" v-if="userInfo.roles">
        <text class="label">角色</text>
        <text class="value">{{ userInfo.roles }}</text>
      </view>
    </view>

    <view class="action-card">
      <button class="logout-btn" @click="handleLogout">注销登录</button>
    </view>
  </view>
</template>

<script>
import request from '@/utils/request.js';
import { clearPermissions, setPermissions } from '@/utils/permission.js';

export default {
  data() {
    return {
      userInfo: {
        avatar: '',
        name: '',
        code: '',
        phone: '',
        email: '',
        gender: '',
        roles: ''
      }
    };
  },
  onShow() {
    this.fetchUserInfo();
  },
  methods: {
    async fetchUserInfo() {
      try {
        uni.showLoading({ title: '加载中...', mask: true });
        const res = await request({
          // url: '/index/info',   // 使用后端提供的接口
          url: '/admin/access/index/info',   // 使用后端提供的接口
          method: 'GET'
        });
        if (res.data) {
          setPermissions(res.data.permissionValueList || []);
          this.userInfo = {
            avatar: res.data.avatar || '',
            name: res.data.name || '',
            code: res.data.code || '',
            phone: res.data.phone || '',
            email: res.data.email || '',
            gender: res.data.gender,
            roles: res.data.roles
          };
        }
        uni.hideLoading();
      } catch (err) {
        uni.hideLoading();
        console.error('获取用户信息失败', err);
        uni.showToast({ title: err.msg || '获取信息失败', icon: 'none' });
      }
    },
    async handleLogout() {
      uni.showModal({
        title: '提示',
        content: '确定要注销登录吗？',
        success: async (res) => {
          if (res.confirm) {
            uni.showLoading({ title: '注销中...', mask: true });
            try {
              await request({
                url: '/admin/access/index/logout',
                method: 'POST'
              });
            } catch (err) {
              console.error('注销请求失败', err);
            } finally {
              uni.removeStorageSync('access_token');
              clearPermissions();
              uni.hideLoading();
              uni.showToast({ title: '已注销', icon: 'success' });
              uni.reLaunch({ url: '/pages/login/login' });
            }
          }
        }
      });
    }
  }
};
</script>

<style scoped>
/* 样式保持不变，与之前相同 */
.profile-container {
  padding: 20px;
  background-color: #f5f7fa;
  min-height: 100vh;
}
.user-card {
  background-color: #ffffff;
  border-radius: 16px;
  padding: 24px 20px;
  margin-bottom: 16px;
  display: flex;
  flex-direction: column;
  align-items: center;
  box-shadow: 0 2px 8px rgba(0,0,0,0.05);
}
.avatar {
  width: 80px;
  height: 80px;
  border-radius: 50%;
  background-color: #e4e7ed;
  margin-bottom: 12px;
}
.user-name {
  font-size: 18px;
  font-weight: bold;
  color: #303133;
  margin-bottom: 6px;
}
.user-account {
  font-size: 13px;
  color: #909399;
}
.info-card {
  background-color: #ffffff;
  border-radius: 16px;
  padding: 16px 20px;
  margin-bottom: 16px;
}
.info-item {
  display: flex;
  padding: 12px 0;
  border-bottom: 1px solid #f0f0f0;
}
.info-item:last-child {
  border-bottom: none;
}
.info-item .label {
  width: 80px;
  color: #606266;
  font-size: 14px;
}
.info-item .value {
  flex: 1;
  color: #303133;
  font-size: 14px;
}
.action-card {
  background-color: transparent;
  padding: 0;
}
.logout-btn {
  background-color: #f56c6c;
  color: white;
  border: none;
  border-radius: 44px;
  height: 48px;
  line-height: 48px;
  font-size: 16px;
  margin: 0;
  width: 100%;
}
.logout-btn::after {
  border: none;
}
</style>