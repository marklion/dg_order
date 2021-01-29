import Vue from 'vue'
import VueRouter from 'vue-router'
import Home from '../views/Home.vue'
import DgOrder from '../views/DgOrder.vue'
import MySelf from '../views/MySelf.vue'
import WechatLogin from '../views/WechatLogin.vue'
import DgOrderGood from '../views/DgOrderGood.vue'

Vue.use(VueRouter)

const routes = [
  {
    path: '/',
    name: 'Home',
    component: Home
  },
  {
    path: '/dg_order',
    name: 'DgOrder',
    component: DgOrder
  },
  {
    path: '/my_self',
    name: 'MySelf',
    component: MySelf
  },
  {
    path:'/wechatlogin',
    name: 'WechatLogin',
    component: WechatLogin
  },
  {
    path: '/dg_order/:order_number',
    name: 'DgOrderGood',
    component: DgOrderGood
  },
]

const router = new VueRouter({
  mode: 'history',
  base: process.env.BASE_URL,
  routes
})

export default router
