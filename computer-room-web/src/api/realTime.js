import request from '@/utils/request'

export function getLastEnvironment(deviceKey) {
  return request({
    url: '/service/environment/getLastEnvironment',
    method: 'get',
    params: { deviceKey }
  })
}
