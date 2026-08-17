import Vue from 'vue'
import Router from 'vue-router'

// in development-env not use lazy-loading, because lazy-loading too many pages will cause webpack hot update too slow. so only in production use lazy-loading;
// detail: https://panjiachen.github.io/vue-element-admin-site/#/lazy-loading

Vue.use(Router)

/* Layout */
import Layout from '../views/layout/Layout'

/**
* hidden: true                   if `hidden:true` will not show in the sidebar(default is false)
* alwaysShow: true               if set true, will always show the root menu, whatever its child routes length
*                                if not set alwaysShow, only more than one route under the children
*                                it will becomes nested mode, otherwise not show the root menu
* redirect: noredirect           if `redirect:noredirect` will no redirect in the breadcrumb
* name:'router-name'             the name is used by <keep-alive> (must set!!!)
* meta : {
    title: 'title'               the name show in submenu and breadcrumb (recommend set)
    icon: 'svg-name'             the icon show in the sidebar,
  }
**/
export const constantRoutes = [
  { path: '/login', component: () => import('@/views/login/index'), hidden: true },
  { path: '/404', component: () => import('@/views/404'), hidden: true },

  // 首页
  {
    path: '/',
    component: Layout,
    redirect: '/dashboard',
    name: 'Dashboard',
    children: [{
      path: 'dashboard',
      component: () => import('@/views/dashboard/index'),
      meta: { title: '首页', icon: 'dashboard' }
    }]
  }]
/**
 * 动态路由
 */
export const asyncRoutes = [
  {
    path: '/access',
    component: Layout,
    redirect: '/access/user/list',
    name: '权限管理',
    meta: { title: '权限管理', icon: 'chart' },
    children: [
      {
        path: 'user/list',
        name: '用户管理',
        component: () => import('@/views/access/user/list'),
        meta: { title: '用户管理' }
      },
      {
        path: 'role/list',
        name: '角色管理',
        component: () => import('@/views/access/role/list'),
        meta: { title: '角色管理' }
      },
      {
        path: 'role/form',
        name: '角色添加',
        component: () => import('@/views/access/role/form'),
        meta: { title: '角色添加' },
        hidden: true
      },
      {
        path: 'role/update/:id',
        name: '角色修改',
        component: () => import('@/views/access/role/form'),
        meta: { title: '角色修改' },
        hidden: true
      },
      {
        path: 'role/distribution/:id',
        name: '角色权限',
        component: () => import('@/views/access/role/roleForm'),
        meta: { title: '角色权限' },
        hidden: true
      },
      {
        path: 'menu/list',
        name: '菜单管理',
        component: () => import('@/views/access/menu/list'),
        meta: { title: '菜单管理' }
      },
      {
        path: 'user/add',
        name: '用户添加',
        component: () => import('@/views/access/user/form'),
        meta: { title: '用户添加' },
        hidden: true
      },
      {
        path: 'user/update/:id',
        name: '用户修改',
        component: () => import('@/views/access/user/form'),
        meta: { title: '用户修改' },
        hidden: true
      },
      {
        path: 'user/role/:id',
        name: '用户角色',
        component: () => import('@/views/access/user/roleForm'),
        meta: { title: '用户角色' },
        hidden: true
      }
    ]
  },

    // 实时数据
    {
      path: '/realTime',
      component: Layout,
      children: [
        {
          path: 'index',
          name: 'Environment',
          component: () => import('@/views/realTime/index'),
          meta: { title: '实时数据', icon: 'real' }
        }
      ]
    },
  
    // 环境数据管理
    {
      path: '/environment',
      component: Layout,
      redirect: '/environment/table',
      name: '环境数据管理',
      meta: { title: '环境数据管理', icon: 'form' },
      children: [
        {
          path: 'table',
          name: '环境数据列表',
          component: () => import('@/views/environment/list'),
          meta: { title: '环境数据列表', icon: 'table' }
        },
        {
          path: 'save',
          name: '添加环境数据',
          component: () => import('@/views/environment/save'),
          meta: { title: '手动记录', icon: 'write' }
        },
        {
          path: 'edit/:id',
          name: 'EnvironmentrEdit',
          component: () => import('@/views/environment/save'),
          meta: { title: '编辑环境数据', noCache: true },
          hidden: true
        }
      ]
    },
  
    // 消息管理
    {
      path: '/mqtt',
      component: Layout,
      redirect: '/mqtt/receiveReport',
      name: '消息管理',
      meta: { title: '消息管理', icon: 'mqtt' },
      children: [
        {
          path: 'receiveReport',
          name: '接收消息',
          component: () => import('@/views/mqtt/receiveReport'),
          meta: { title: '接收消息列表', icon: 'receive' }
        },
        {
          path: 'sendCmd',
          name: '发送消息',
          component: () => import('@/views/mqtt/sendCmd'),
          meta: { title: '发送消息列表', icon: 'send' }
        },
        {
          path: 'receiveCmdResp',
          name: '命令响应消息',
          component: () => import('@/views/mqtt/receiveCmdResp'),
          meta: { title: '命令响应消息', icon: 'receive' }
        }
      ]
    },
  
    // 数据统计
    {
      path: '/statistics',
      component: Layout,
      redirect: '/statistics/line',
      name: 'Statistics',
      meta: { title: '数据统计', icon: 'statistics' },
      children: [
        {
          path: 'line',
          name: 'LineChart',
          component: () => import('@/views/statistics/lineChart'),
          meta: { title: '折线图', icon: 'line' }
        },
        {
          path: 'bar',
          name: 'BarChart',
          component: () => import('@/views/statistics/barChart'),
          meta: { title: '柱状图', icon: 'bar' }
        }
      ]
    },
  
    // 账户管理
    // {
    //   path: '/account',
    //   component: Layout,
    //   redirect: '/account/table',
    //   name: '账户管理',
    //   meta: { title: '账户管理', icon: 'form', roles: ['admin'] },
    //   children: [
    //     {
    //       path: 'table',
    //       name: '账户列表',
    //       component: () => import('@/views/account/list'),
    //       meta: { title: '账户列表', icon: 'table' }
    //     },
    //     {
    //       path: 'save',
    //       name: '添加账户',
    //       component: () => import('@/views/account/save'),
    //       meta: { title: '添加账户', icon: 'write' }
    //     },
    //     {
    //       path: 'edit/:id',
    //       name: 'AccountEdit',
    //       component: () => import('@/views/account/save'),
    //       meta: { title: '编辑账户', noCache: true },
    //       hidden: true
    //     }
    //   ]
    // },
  
    // 反控记录
    {
      path: '/deviceOption',
      component: Layout,
      redirect: '/deviceOption/index',
      name: '反控记录',
      meta: { title: '反控记录', icon: 'control' },
      children: [
        {
          path: 'index',
          name: '操作记录',
          component: () => import('@/views/deviceOption/index'),
          meta: { title: '操作记录', icon: 'option' }
        },
        {
          path: 'command',
          name: '指令列表',
          component: () => import('@/views/deviceOption/command'),
          meta: { title: '指令列表', icon: 'command' }
        }
      ]
    },
    {
      path: '/device',
      component: Layout,
      redirect: '/device/table',
      name: '设备数据',
      meta: { title: '设备数据', icon: 'device' },
      children: [
        {
          path: 'table',
          name: '设备数据',
          component: () => import('@/views/device/list'),
          meta: { title: '设备数据', icon: 'deviceList' }
        },
        {
          path: 'save',
          name: '添加设备',
          component: () => import('@/views/device/save'),
          meta: { title: '手动记录', icon: 'deviceAdd' }
        },
        {
          path: 'edit/:id',
          name: 'EnvironmentrEdit',
          component: () => import('@/views/device/save'),
          meta: { title: '编辑设备', noCache: true },
          hidden: true
        }
      ]
    },
  { path: '*', redirect: '/404', hidden: true }
]

const createRouter = () => new Router({
  // mode: 'history', // require service support
  scrollBehavior: () => ({ y: 0 }),
  routes: constantRoutes
})

const router = createRouter()

// export default new Router({
//   // mode: 'history', //后端支持可开
//   scrollBehavior: () => ({ y: 0 }),
//   routes: constantRouterMap
// })

export default router
