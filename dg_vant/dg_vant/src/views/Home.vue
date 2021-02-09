<template>
<div class="home_show" v-if="is_login">
    <van-nav-bar title="代购助手" :fixed="true" :placeholder="true" :safe-area-inset-top="true" :border="true"></van-nav-bar>

    <div style="text-align: center;">
        <div style="height:200px"></div>
        <van-image :src="user_logo" width="100" height="100" round></van-image>
        <h1>{{user_name}}</h1>
        <van-button round type="info" style="width:140px" @click="create_order_diag = true">创建代购</van-button>
    </div>
    <van-dialog title="请填写代购详情" v-model="create_order_diag" :show-confirm-button="false" show-cancel-button="true" :close-on-click-overlay="true">
        <van-form @submit="onCreateOrder">
            <van-field v-model="order_brief.destination" name="目的地" label="目的地" placeholder="目的地" :rules="[{ required: true, message: '请填写目的地' }]" />
            <van-field readonly clickable name="calendar" :value="order_brief.start_time" label="出发日期" placeholder="点击选择日期" @click="show_start_calendar = true" :rules="[{ required: true, message: '请选择出发日期' }]" />
            <van-calendar v-model="show_start_calendar" @confirm="on_start_time_confirm" get-container="body" />
            <van-field readonly clickable name="calendar" :value="order_brief.deliver_time" label="预计发货日期" placeholder="点击选择日期" @click="show_deliver_calendar = true" :rules="[{ required: true, message: '请选择发货日期' }]" />
            <van-calendar v-model="show_deliver_calendar" @confirm="on_deliver_time_confirm" get-container="body" />
            <van-field v-model="order_brief.comments" name="备注" label="备注" placeholder="备注" rows="2" autosize type="textarea" />
            <van-field name="微信二维码" label="微信二维码" placeholder="点击上传微信二维码" readonly clickable @click="call_wx_upload_contact_qr">
                <template #input v-if="order_brief.contact_qr != ''">
                    <van-image height="100" width="100" :src="good_img_show_out()"></van-image>
                </template>
            </van-field>
            <div style="margin: 16px;">
                <van-button round block type="info" native-type="submit">提交</van-button>
            </div>
        </van-form>
    </van-dialog>
</div>
</template>

<script>
import {
    Base64
} from 'js-base64'
import wx from 'weixin-js-sdk'
export default {
    name: 'Home',
    data: function () {
        return {
            user_logo: '',
            user_name: 'ces',
            is_login: false,
            create_order_diag: false,
            order_brief: {
                destination: '',
                start_time: '',
                deliver_time: '',
                comments: '',
                contact_qr: '',
            },
            show_start_calendar: false,
            show_deliver_calendar: false,
            apple_img: '',
            good_img_show_out: function () {
                var vue_this = this;
                var ret = vue_this.order_brief.contact_qr;
                if (vue_this.is_apple()) {
                    ret = vue_this.apple_img;
                }

                return ret;
            },
        };
    },
    methods: {
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
        config_with_wx: function () {
            var timestamp = (new Date()).getTime();
            var nonceStr = this.randomString(32);
            var vue_this = this;
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
                    jsApiList: ['chooseImage', 'uploadImage', 'getLocalImgData']
                });
                wx.ready(function () {
                    console.log('success to config wx');
                    vue_this.is_ready = true;
                    vue_this.$toast.clear();
                });
                wx.error(function (err) {
                    console.log('fail to config wx');
                    console.log(err);
                });
            }).catch(function (err) {
                console.log(err);
            });
        },
        is_apple: function () {
            return window.__wxjs_is_wkwebview;
        },
        call_wx_upload_contact_qr: function () {
            var vue_this = this;
            wx.chooseImage({
                count: 1,
                sizeType: ['compressed'],
                success: function (res) {
                    vue_this.order_brief.contact_qr = res.localIds[0];
                    if (vue_this.is_apple()) {
                        wx.getLocalImgData({
                            localId: vue_this.order_brief.contact_qr,
                            success: function (res_get) {
                                vue_this.apple_img = res_get.localData;
                            }
                        });
                    }
                },
            });
        },
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
                    window.location.href = "https://open.weixin.qq.com/connect/oauth2/authorize?appid=wxa390f8b6f68e9c6d&redirect_uri=http%3a%2f%2fwww.d8sis.cn%2fwechatlogin&response_type=code&scope=snsapi_userinfo&state=%2f#wechat_redirect"
                }
                console.log(vue_this);
            }).catch(function (err) {
                console.log(err);
                window.location.href = "https://open.weixin.qq.com/connect/oauth2/authorize?appid=wxa390f8b6f68e9c6d&redirect_uri=http%3a%2f%2fwww.d8sis.cn%2fwechatlogin&response_type=code&scope=snsapi_userinfo&state=%2f#wechat_redirect"
            });
        },
        on_start_time_confirm: function (date) {
            this.show_start_calendar = false;
            this.order_brief.start_time = `${date.getFullYear()}-${date.getMonth() + 1}-${date.getDate()}`;
        },
        on_deliver_time_confirm: function (date) {
            this.show_deliver_calendar = false;
            this.order_brief.deliver_time = `${date.getFullYear()}-${date.getMonth() + 1}-${date.getDate()}`;
        },
        orig_create_order: function (_contact_qr) {
            var vue_this = this;
            var created_order_req = {
                order: {
                    ssid: this.$cookies.get("ssid"),
                    info: {
                        destination: this.order_brief.destination.toBase64(),
                        start_time: this.order_brief.start_time,
                        deliver_time: this.order_brief.deliver_time,
                        comments: this.order_brief.comments.toBase64(),
                        contact_qr: _contact_qr,
                    }
                }
            };
            this.$axios.post(this.$remote_rest_url_header + "dg_order", created_order_req).then(function (resp) {
                vue_this.$toast.clear();
                vue_this.$router.push({
                    path: '/dg_order/' + resp.data.result
                });
            }).catch(function (err) {
                console.log(err);
            });

        },
        onCreateOrder: function () {
            var vue_this = this;
            this.$toast.loading({
                message: '加载中...',
                forbidClick: true,
                duration: 0,
            });

            Base64.extendString();
            if (vue_this.order_brief.contact_qr != '') {
                wx.uploadImage({
                    localId: vue_this.order_brief.contact_qr,
                    isShowProgressTips: 0,
                    success: function (res_server) {
                        console.log(res_server.serverId);
                        vue_this.orig_create_order(res_server.serverId);
                    },
                });
            } else {
                this.orig_create_order('');
            }
        }
    },
    beforeMount: function () {
        this.get_user_info();
        this.config_with_wx();
    },
}
</script>

<style lang="less" scoped>
</style>
