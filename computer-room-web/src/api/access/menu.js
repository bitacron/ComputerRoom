import request from '@/utils/request'

const api_name = '/admin/access/permission'

export default {
  getNestedTreeList() {
    return request({
      url: `${api_name}`,
      method: 'get'
    })
  },
  removeById(id) {
    return request({
      url: `${api_name}/remove/${id}`,
      method: "delete"
    })
  },
  saveLevelOne(menu) {
    return request({
      url: `${api_name}/save`,
      method: "post",
      data: menu
    })
  },
  update(menu) {
    return request({
      url: `${api_name}/update`,
      method: "put",
      data: menu
    })
  },
  toAssign(roleId) {
    return request({
      url: `${api_name}/toAssign/${roleId}`,
      method: 'get'
    })
  },
  doAssign(roleId, permissionId) {
    const permissionIds = Array.isArray(permissionId)
      ? permissionId
      : String(permissionId).split(',').filter(Boolean)
    return request({
      url: `${api_name}/doAssign`,
      method: 'post',
      params: { roleId, permissionId: permissionIds },
      paramsSerializer: params => {
        const search = new URLSearchParams()
        search.append('roleId', params.roleId)
        params.permissionId.forEach(id => search.append('permissionId', id))
        return search.toString()
      }
    })
  }
}
