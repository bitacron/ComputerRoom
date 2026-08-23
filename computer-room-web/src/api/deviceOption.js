import request from '@/utils/request'

export default {
    // report
    getOptionListPage(query) {
        return request({
            url: `/service/deviceOption/pageDeviceOptionCondition`,
            method: 'post',
            data: query
          })
    },
    deleteOptionById(id) {
        return request({
            url: `/service/deviceOption/${id}`,
            method: 'delete'
          })
    },
    getOptionInfo(id) {
        return request({
            url: `/service/deviceOption/getDeviceOption/${id}`,
            method: 'get'
        })
    },
    getCommandListPage(query) {
        return request({
            url: `/service/command/pageCommandCondition`,
            method: 'post',
            data: query
          })
    },
    deleteCommandById(id) {
        return request({
            url: `/service/command/${id}`,
            method: 'delete'
          })
    },
    getCommandInfo(id) {
        return request({
            url: `/service/command/getCommand/${id}`,
            method: 'get'
        })
    },
    controlDevice(data) {
        return request({
            url: '/service/deviceOption/control',
            method: 'post',
            data,
            timeout: 5000
        })
    }
}

