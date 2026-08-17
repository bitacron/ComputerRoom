import axios from 'axios'
import { Message } from 'element-ui'
import store from '../store'
import { getToken } from '@/utils/auth'
import router from '@/router'

const service = axios.create({
  baseURL: process.env.BASE_API,
  timeout: 20000
})

let redirectingToLogin = false

function redirectToLogin(message) {
  if (redirectingToLogin) {
    return
  }
  redirectingToLogin = true
  Message({
    message: message || '登录已过期，请重新登录',
    type: 'error',
    duration: 3 * 1000
  })
  store.dispatch('FedLogOut').then(() => {
    router.replace(`/login?redirect=${router.currentRoute.fullPath}`).finally(() => {
      redirectingToLogin = false
    })
  }).catch(() => {
    redirectingToLogin = false
  })
}

service.interceptors.request.use(
  config => {
    if (store.getters.token) {
      config.headers['token'] = getToken()
    }
    return config
  },
  error => {
    console.log(error)
    return Promise.reject(error)
  }
)

service.interceptors.response.use(
  response => {
    if (response.config.responseType === 'blob' || response.config.responseType === 'arraybuffer') {
      return response
    }

    const res = response.data
    if (res.code === 401 || res.code === 403) {
      redirectToLogin(res.msg || (res.code === 403 ? '无权限访问' : '登录已过期，请重新登录'))
      return Promise.reject(res)
    }
    if (res.code !== 20000) {
      Message({
        message: res.msg || '操作失败',
        type: 'error',
        duration: 5 * 1000
      })
      return Promise.reject(res)
    }
    return response.data
  },
  error => {
    console.log('err' + error)
    const status = error.response && error.response.status
    const data = error.response && error.response.data
    if (status === 401 || status === 403 || (data && (data.code === 401 || data.code === 403))) {
      redirectToLogin((data && data.msg) || '登录已过期，请重新登录')
      return Promise.reject(error)
    }
    Message({
      message: (data && data.msg) || error.message || '网络异常',
      type: 'error',
      duration: 5 * 1000
    })
    return Promise.reject(error)
  }
)

export default service
