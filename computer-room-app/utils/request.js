import { BASE_API } from './config.js';

const PERMISSION_KEY = 'permission_list';

const clearSession = () => {
  uni.removeStorageSync('access_token');
  uni.removeStorageSync(PERMISSION_KEY);
};

const redirectToLogin = () => {
  clearSession();
  uni.showToast({ title: '登录已过期，请重新登录', icon: 'none' });
  setTimeout(() => {
    uni.reLaunch({ url: '/pages/login/login' });
  }, 1500);
};

const isAuthError = (res) => {
  if (!res || !res.data) {
    return res && (res.statusCode === 401 || res.statusCode === 403);
  }
  const code = res.data.code;
  return code === 401 || code === 403;
};

const request = (options) => {
  return new Promise((resolve, reject) => {
    const token = uni.getStorageSync('access_token');
    const header = {
      'Content-Type': 'application/json',
      ...options.header
    };
    if (token) {
      header['Authorization'] = `Bearer ${token}`;
    }

    uni.request({
      url: BASE_API + options.url,
      method: options.method || 'GET',
      data: options.data || {},
      header: header,
      timeout: options.timeout || 10000,
      success: (res) => {
        if (isAuthError(res)) {
          if (res.data && res.data.code === 403) {
            uni.showToast({ title: res.data.msg || '无权限访问', icon: 'none' });
          }
          if (res.data && res.data.code === 401) {
            redirectToLogin();
          }
          reject(res.data || { msg: '未授权' });
          return;
        }
        if (res.statusCode === 200) {
          if (res.data.code === 20000) {
            resolve(res.data);
          } else {
            reject(res.data);
          }
        } else {
          reject({ msg: '网络请求失败', statusCode: res.statusCode });
        }
      },
      fail: (err) => {
        reject({ msg: '网络异常，请检查网络', err });
      }
    });
  });
};

export default request;
