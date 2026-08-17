import request from '@/utils/request'

export default {

    // 1.环境数据列表
    getEnvironmentListPage(environmentQuery) {
        return request({
            url: `/service/environment/pageEnvironmentCondition`,
            method: 'post',
            data: environmentQuery
          })
    },
    deleteEnvironmentById(id) {
        return request({
            url: `/service/environment/${id}`,
            method: 'delete'
          })
    },
    addEnvironment(environment) {
        return request({
            url: `/service/environment/addEnvironment`,
            method: 'post',
            data: environment
        })
    },
    getEnvironmentInfo(id) {
        return request({
            url: `/service/environment/getEnvironment/${id}`,
            method: 'get'
        })
    },
    updateEnvironmentInfo(environment) {
        return request({
            url: `/service/environment/updateEnvironment`,
            method: 'post',
            data: environment
        })
    },
    // 导出Excel
    exportEnvironmentList(params) {
        return request({
            url: '/service/environment/export',
            method: 'post',
            data: params,
            responseType: 'blob' // 关键：指定响应类型为 blob
        })
    }
}

