import request from '@/utils/request'

export default {
    // report
    getReceiveListPage(receiveQuery) {
        return request({
            url: `/mqtt/receive/report/pageReportCondition`,
            method: 'post',
            data: receiveQuery
          })
    },
    deleteReceiveById(id) {
        return request({
            url: `/mqtt/receive/report/${id}`,
            method: 'delete'
          })
    },
    addReceive(receive) {
        return request({
            url: `/mqtt/receive/report/addReport`,
            method: 'post',
            data: receive
        })
    },
    getReceiveInfo(id) {
        return request({
            url: `/mqtt/receive/report/getReport/${id}`,
            method: 'get'
        })
    },
    updateReceiveInfo(receive) {
        return request({
            url: `/mqtt/receive/report/updateReport`,
            method: 'post',
            data: receive
        })
    },
    // cmd
    getSendListPage(sendQuery) {
        return request({
            url: `/mqtt/send/cmd/pageSendCmdCondition`,
            method: 'post',
            data: sendQuery
          })
    },
    deleteSendById(id) {
        return request({
            url: `/mqtt/send/cmd/${id}`,
            method: 'delete'
          })
    },
    addSend(send) {
        return request({
            url: `/mqtt/send/cmd/addSendCmd`,
            method: 'post',
            data: send
        })
    },
    getSendInfo(id) {
        return request({
            url: `/mqtt/send/cmd/getSendCmd/${id}`,
            method: 'get'
        })
    },
    updateSendInfo(send) {
        return request({
            url: `/mqtt/send/cmd/updateSendCmd`,
            method: 'post',
            data: send
        })
    },
    // cmd_resp
    getCmdRespListPage(cmdRespQuery) {
        return request({
            url: `/mqtt/receive/cmdResp/pageCmdRespCondition`,
            method: 'post',
            data: cmdRespQuery
          })
    },
    deleteCmdRespById(id) {
        return request({
            url: `/mqtt/receive/cmdResp/${id}`,
            method: 'delete'
          })
    },
    addCmdResp(send) {
        return request({
            url: `/mqtt/receive/cmdResp/addCmdResp`,
            method: 'post',
            data: send
        })
    },
    getCmdRespInfo(id) {
        return request({
            url: `/mqtt/receive/cmdResp/getCmdResp/${id}`,
            method: 'get'
        })
    },
    updateCmdRespInfo(send) {
        return request({
            url: `/mqtt/receive/cmdResp/updateCmdResp`,
            method: 'post',
            data: send
        })
    }
}

