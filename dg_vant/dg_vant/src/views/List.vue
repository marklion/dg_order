<template>
<div class="list_show" v-if="should_show()">
    测试用
</div>
</template>

<script>
export default {
    name: 'List',
    data: function () {
        return {
            is_login: false,
            user_is_host: false,
            should_show: function () {
                return this.is_login && this.user_is_host;
            }
        };
    },
    methods: {
        get_user_info: function () {
            var vue_this = this;
            var ssid = this.$cookies.get('ssid');
            this.$axios.get(this.$remote_rest_url_header + 'user_info/' + ssid).then(function (resp) {
                if (resp.data.result.online == true) {
                    vue_this.is_login = true;
                } else {
                    window.location.href = "https://open.weixin.qq.com/connect/oauth2/authorize?appid=wxa390f8b6f68e9c6d&redirect_uri=http%3a%2f%2fwww.d8sis.cn%2fwechatlogin&response_type=code&scope=snsapi_userinfo&state=%2flist%2f" + vue_this.order_number + "#wechat_redirect"
                }
                console.log(vue_this);
            }).catch(function (err) {
                console.log(err);
                window.location.href = "https://open.weixin.qq.com/connect/oauth2/authorize?appid=wxa390f8b6f68e9c6d&redirect_uri=http%3a%2f%2fwww.d8sis.cn%2fwechatlogin&response_type=code&scope=snsapi_userinfo&state=%2flist%2f" + vue_this.order_number + "#wechat_redirect"
            });
        },
        get_order_number: function () {
            return this.$route.params.order_number;
        },
        get_host_user: function () {
            var vue_this = this;
            vue_this.$axios.post(vue_this.$remote_rest_url_header + 'host_of', {
                ssid: vue_this.$cookies.get('ssid'),
                order_id: vue_this.get_order_number(),
            }).then(function (resp) {
                if (resp.data.result) {
                    vue_this.user_is_host = true;
                }
            }).catch(function (err) {
                console.log(err);
            });
        },
    },
    beforeMount: function () {
        this.get_user_info();
        this.get_host_user();
    },
}
</script>

<style>

</style>
