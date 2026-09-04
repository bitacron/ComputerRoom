import request from '@/utils/request'

export default {
    getAlarmListPage(query) {
        return request({
            url: '/service/alarm/page',
            method: 'post',
            data: query
        })
    },
    getAlarmSummary() {
        return request({
            url: '/service/alarm/summary',
            method: 'get'
        })
    },
    handleAlarm(id) {
        return request({
            url: `/service/alarm/handle/${id}`,
            method: 'post'
        })
    },
    deleteAlarmById(id) {
        return request({
            url: `/service/alarm/${id}`,
            method: 'delete'
        })
    }
}
