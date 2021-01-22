<template>
<div class="my_order_show" v-if="is_login">
    <el-row class="user_info_show" :gutter="5" type="flex" align="middle">
        <el-col :span="6">
            <el-avatar :src="self_info.user_logo" fit="cover" :size="60">
                <img src="https://cube.elemecdn.com/e/fd/0fc7d20532fdaf769a25683617711png.png" />
            </el-avatar>
        </el-col>
        <el-col :span="18">
            <div>{{self_info.user_name}}</div>
        </el-col>
    </el-row>
    <self-order-show v-for="order_id in order_ids" :key="order_id" :belong_order_id="order_id"></self-order-show>
</div>
</template>

<script>
import {
    Base64
} from 'js-base64'
import SelfOrderShow from '../components/SelfOrderShow.vue'
export default {
    name: 'MyOrder',
    data: function () {
        return {
            is_login: false,
            self_info: {
                user_name: '',
                user_logo: ''
            },
            order_ids: []
        };
    },
    components: {
        'self-order-show': SelfOrderShow,
    },
    methods: {
        get_user_info: function () {
            var vue_this = this;
            var ssid = this.$cookies.get('ssid');
            console.log(this.$remote_rest_url_header);
            Base64.extendString();
            this.$axios.get(this.$remote_rest_url_header + 'user_info/' + ssid).then(function (resp) {
                console.log(resp);
                if (resp.data.result.online == true) {
                    vue_this.is_login = true;
                    vue_this.self_info.user_name = resp.data.result.user_name.fromBase64();
                    vue_this.self_info.user_logo = vue_this.$remote_url + resp.data.result.user_logo;
                } else {
                    window.location.href = "https://open.weixin.qq.com/connect/oauth2/authorize?appid=wxa390f8b6f68e9c6d&redirect_uri=http%3a%2f%2fwww.d8sis.cn%2fwechatlogin&response_type=code&scope=snsapi_userinfo&state=%2fmy_order#wechat_redirect"
                }
                console.log(vue_this);
            }).catch(function (err) {
                console.log(err);
                window.location.href = "https://open.weixin.qq.com/connect/oauth2/authorize?appid=wxa390f8b6f68e9c6d&redirect_uri=http%3a%2f%2fwww.d8sis.cn%2fwechatlogin&response_type=code&scope=snsapi_userinfo&state=%2fmy_order#wechat_redirect"
            });
        },
    },
    beforeMount: function () {
        var vue_this = this;
        this.get_user_info();
        this.$axios.get(this.$remote_rest_url_header + "join_id/" + this.$cookies.get('ssid')).then(function (resp) {
            resp.data.result.forEach((element, index) => {
                vue_this.$set(vue_this.order_ids, index, element);
            });
        }).catch(function (err) {
            console.log(err);
        });
    },
}
</script>

<style scoped>
.user_info_show {
    background-color: rgb(194, 127, 127);
}
</style>
