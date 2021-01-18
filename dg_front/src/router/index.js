import Vue from 'vue'
import VueRouter from 'vue-router'
import Home from '../views/Home.vue'
import WechatLogin from '../views/WechatLogin.vue'
import DgOrder from '../views/DgOrder.vue'

Vue.use(VueRouter)

const routes = [
  {
    path: '/',
    name: 'Home',
    component: Home
  },
  {
    path:'/wechatlogin',
    name: 'WechatLogin',
    component: WechatLogin
  },
  {
    path:'/dg_order/:order_number',
    name: 'DgOrder',
    component: DgOrder
  }
]

const router = new VueRouter({
  mode: 'history',
  base: process.env.BASE_URL,
  routes
})

export default router
