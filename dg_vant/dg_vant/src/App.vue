<template>
<div id="app">
    <router-view />
    <div style="height:5rem"></div>
    <van-tabbar route :placeholder="true" :safe-area-inset-bottom="true">
        <van-tabbar-item replace to="/" icon="home-o">首页</van-tabbar-item>
        <van-tabbar-item replace to="/dg_order" icon="cart-o">订单</van-tabbar-item>
        <van-tabbar-item replace to="/my_self" icon="user-o">我的</van-tabbar-item>
    </van-tabbar>
</div>
</template>


<style lang="less">
</style>

