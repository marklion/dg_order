import Vue from 'vue'
import VueRouter from 'vue-router'
import Home from '../views/Home.vue'
import WechatLogin from '../views/WechatLogin.vue'
import DgOrder from '../views/DgOrder.vue'
import MyOrder from '../views/MyOrder.vue'

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
  },
  {
    path: '/my_order',
    name: 'MyOrder',
    component: MyOrder
  }
]

const router = new VueRouter({
  mode: 'history',
  base: process.env.BASE_URL,
  routes
})

export default router
