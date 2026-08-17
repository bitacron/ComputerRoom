import request from '@/utils/request'

export default {
    getDeviceListPage(deviceQuery) {
        return request({
            url: `/service/device/pageDeviceCondition`,
            method: 'post',
            data: deviceQuery
          })
    },
    deleteDeviceById(id) {
        return request({
            url: `/service/device/${id}`,
            method: 'delete'
          })
    },
    addDevice(device) {
        return request({
            url: `/service/device/addDevice`,
            method: 'post',
            data: device
        })
    },
    getDeviceInfo(id) {
        return request({
            url: `/service/device/getDevice/${id}`,
            method: 'get'
        })
    },
    updateDeviceInfo(device) {
        return request({
            url: `/service/device/updateDevice`,
            method: 'post',
            data: device
        })
    },
    getDeviceSelect() {
        return request({
            url: `/service/device/getDeviceSelect`,
            method: 'get'
        })
    }
}

