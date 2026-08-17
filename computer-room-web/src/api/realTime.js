import request from '@/utils/request'

// 获取最新环境数据
export function getLastEnvironment() {
  return request({
    url: '/service/environment/getLastEnvironment',
    method: 'get'
  })
}

// 控制散热器（假设接口为 /control/radiator）
export function controlRadiator(status) {
  return request({
    url: '/service/environment/control/radiator',
    method: 'post',
    params: { status } // 根据实际接口调整
  })
}

// 控制 LED（假设接口为 /control/led）
export function controlLed(status) {
  return request({
    url: '/service/environment/control/led',
    method: 'post',
    params: { status }
  })
}
