import request from './request.js'

const PERMISSION_KEY = 'permission_list'

export function getPermissions() {
  return uni.getStorageSync(PERMISSION_KEY) || []
}

export function setPermissions(list) {
  uni.setStorageSync(PERMISSION_KEY, list || [])
}

export function clearPermissions() {
  uni.removeStorageSync(PERMISSION_KEY)
}

export function hasPerm(permission) {
  return getPermissions().includes(permission)
}

export async function loadPermissions() {
  const res = await request({
    url: '/admin/access/index/info',
    method: 'GET'
  })
  const list = (res.data && res.data.permissionValueList) || []
  setPermissions(list)
  return list
}
