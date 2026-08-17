/**
 * 后端 API 根地址（HTTP / WebSocket 共用）
 * 换环境时只改这一处即可
 */
export const BASE_API = 'http://192.168.153.1:8080'

/**
 * 由 BASE_API 推导 WebSocket 地址（http→ws，https→wss）
 */
export function getWsUrl(path = '/ws/environment/websocket') {
  const base = String(BASE_API || '').replace(/\/$/, '')
  const wsBase = base.replace(/^http/i, 'ws')
  const suffix = path.startsWith('/') ? path : `/${path}`
  return `${wsBase}${suffix}`
}
