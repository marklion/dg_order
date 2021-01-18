<template>
<div>
正在登陆中
</div>
</template>

<script>
export default {
    name: 'WechatLogin',
    beforeMount: function () {
        var vue_this = this;
        console.log(this.$remote_rest_url_header);
        if (this.$route.query.code) {
            this.$axios.post(this.$remote_rest_url_header + 'wechat_login', {
                code: this.$route.query.code
            }).then(
                function (resp) {
                    vue_this.$cookies.set("ssid", resp.data.result);
                    vue_this.$router.replace({name:'Home'});
                }
            ).catch(function (err) {
                console.log(err);
            });
        }
    },
}
</script>

<style>

</style>
