<template>
<div class="dg_order_show" v-if="is_login">
    <van-nav-bar title="订单" :fixed="true" :placeholder="true" :safe-area-inset-top="true"></van-nav-bar>
    <van-tabs v-model="active" animated>
        <van-tab title="我参与的代购">
            <van-divider>左滑查看我购买的</van-divider>
            <van-swipe-cell v-for="order in joined_by_me" :key="order.id">
                <van-cell :title="order.user_name" :value="order.destination" :label="order.start_time" is-link :url="/dg_order/ + order.id" />
                <template #right>
                    <van-button style="height:100%" square type="primary" text="我的" :url="/my_goods/ + order.id" />
                </template>
            </van-swipe-cell>
        </van-tab>

        <van-tab title="我创建的代购">
            <van-swipe-cell v-for="order in created_by_me" :key="order.id">
                <van-cell :title="order.user_name" :value="order.destination" :label="order.start_time" is-link :url="/dg_order/ + order.id" />
                <template #right>
                    <van-button style="height:100%" square type="primary" text="编辑" />
                </template>
            </van-swipe-cell>
        </van-tab>
    </van-tabs>
</div>
</template>

<script>
import {
    Base64
} from 'js-base64'
export default {
    name: 'DgOrder',
    data: function () {
        return {
            is_login: false,
            created_by_me: [],
            joined_by_me: [],
            active: 0,
        };
    },
    methods: {
        get_user_info: function () {
            var vue_this = this;
            var ssid = this.$cookies.get('ssid');
            this.$axios.get(this.$remote_rest_url_header + 'user_info/' + ssid).then(function (resp) {
                if (resp.data.result.online == true) {
                    Base64.extendString();
                    vue_this.user_name = resp.data.result.user_name.fromBase64();
                    console.log(vue_this.user_name);
                    vue_this.user_logo = vue_this.$remote_url + resp.data.result.user_logo;
                    vue_this.is_login = true;
                } else {
                    window.location.href = "https://open.weixin.qq.com/connect/oauth2/authorize?appid=wxa390f8b6f68e9c6d&redirect_uri=http%3a%2f%2fwww.d8sis.cn%2fwechatlogin&response_type=code&scope=snsapi_userinfo&state=%2fdg_order#wechat_redirect"
                }
                console.log(vue_this);
            }).catch(function (err) {
                console.log(err);
                window.location.href = "https://open.weixin.qq.com/connect/oauth2/authorize?appid=wxa390f8b6f68e9c6d&redirect_uri=http%3a%2f%2fwww.d8sis.cn%2fwechatlogin&response_type=code&scope=snsapi_userinfo&state=%2fdg_order#wechat_redirect"
            });
        },
        get_created_orders: function () {
            var vue_this = this;
            var ssid = this.$cookies.get('ssid');
            this.$axios.get(this.$remote_rest_url_header + 'order_created/' + ssid).then(function (resp) {
                resp.data.result.forEach((element, index) => {
                    vue_this.$set(vue_this.created_by_me, index, {
                        id: element.id,
                        user_name: element.user_name.fromBase64(),
                        user_logo: element.user_logo,
                        destination: element.destination.fromBase64(),
                        start_time: element.start_time,
                        deliver_time: element.deliver_time,
                    });
                });
            }).catch(function (err) {
                console.log(err);
            });
        },
        get_joind_orders: function () {
            var vue_this = this;
            this.$axios.get(this.$remote_rest_url_header + "join_id/" + this.$cookies.get('ssid')).then(function (resp) {
                resp.data.result.forEach((element, index) => {
                    vue_this.$axios.get(vue_this.$remote_rest_url_header + 'dg_order/' + element).then(function (resp) {
                        Base64.extendString();
                        console.log(resp);
                        vue_this.$set(vue_this.joined_by_me, index, {
                            id: resp.data.result.order_id,
                            user_name: resp.data.result.order_owner_name.fromBase64(),
                            user_logo: resp.data.result.order_owner_logo,
                            destination: resp.data.result.info.destination.fromBase64(),
                            start_time: resp.data.result.info.start_time,
                            deliver_time: resp.data.result.info.deliver_time,
                        });
                    }).catch(function (err) {
                        console.log(err);
                    });
                });
            }).catch(function (err) {
                console.log(err);
            });
        }
    },
    beforeMount: function () {
        this.get_user_info();
        this.get_created_orders();
        this.get_joind_orders();
    },
}
</script>

<style>

</style>
