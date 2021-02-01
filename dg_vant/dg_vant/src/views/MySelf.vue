<template>
<div class="my_self_shoe">
    <van-nav-bar title="我的" :fixed="true" :placeholder="true" :safe-area-inset-top="true"></van-nav-bar>
    <van-cell-group title="设置默认收货地址">
        <van-cell title="默认收货地址" :value="address.detailInfo" clickable @click="record_address" />
    </van-cell-group>
</div>
</template>

<script>
import wx from 'weixin-js-sdk'
export default {
    name: 'MySelf',
    data: function () {
        return {
            address: {
                userName: '',
                provinceName: '',
                cityName: '',
                countryName: '',
                detailInfo: '',
                telNumber: ''
            },
        };
    },
    methods: {
        record_address: function () {
            var vue_this = this;
            wx.openAddress({
                success: function (res) {
                    vue_this.address.userName = res.userName; // 收货人姓名
                    vue_this.address.provinceName = res.provinceName; // 国标收货地址第一级地址（省）
                    vue_this.address.cityName = res.cityName; // 国标收货地址第二级地址（市）
                    vue_this.address.countryName = res.countryName; // 国标收货地址第三级地址（国家）
                    vue_this.address.detailInfo = res.detailInfo; // 详细收货地址信息
                    vue_this.address.telNumber = res.telNumber; // 收货人手机号码
                    console.log(vue_this.address);
                }
            });
        },
        randomString: function (len) {
            len = len || 32;
            var $chars = 'ABCDEFGHJKMNPQRSTWXYZabcdefhijkmnprstwxyz2345678'; /****默认去掉了容易混淆的字符oOLl,9gq,Vv,Uu,I1****/
            var maxPos = $chars.length;
            var pwd = '';
            for (var i = 0; i < len; i++) {
                pwd += $chars.charAt(Math.floor(Math.random() * maxPos));
            }
            return pwd;
        },

    },
    mounted: function () {
        var timestamp = (new Date()).getTime();
        var nonceStr = this.randomString(32);
        this.$axios.post(this.$remote_rest_url_header + 'dg_wx_sign', {
            timestamp: timestamp,
            nonceStr: nonceStr,
            url: window.location.href,
        }).then(function (resp) {
            wx.config({
                debug: false,
                appId: 'wxa390f8b6f68e9c6d',
                timestamp: timestamp,
                nonceStr: nonceStr,
                signature: resp.data.result,
                jsApiList: ['OpenAddress']
            });
            wx.ready(function () {
                console.log('success to config wx');
            });
            wx.error(function (err) {
                console.log('fail to config wx');
                console.log(err);
            });
        }).catch(function (err) {
            console.log(err);
        });
    }
}
</script>

<style>

</style>
