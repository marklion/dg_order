import Vue from 'vue'
import VueRouter from 'vue-router'
import Home from '../views/Home.vue'
import DgOrder from '../views/DgOrder.vue'
import MySelf from '../views/MySelf.vue'
import WechatLogin from '../views/WechatLogin.vue'
import DgOrderGood from '../views/DgOrderGood.vue'
import MyGoods from '../views/MyGoods.vue'
import List from '../views/List.vue'

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
  {
    path: '/my_goods/:order_number',
    name: 'MyGoods',
    component: MyGoods 
  },
  {
    path: '/list/:order_number',
    name: 'List',
    component: List 
  },
]

const router = new VueRouter({
  mode: 'history',
  base: process.env.BASE_URL,
  routes
})

export default router
