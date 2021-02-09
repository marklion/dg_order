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
                    <van-button style="height:100%" square type="primary" text="编辑" @click="pop_out_modify(order)" />
                </template>
            </van-swipe-cell>
            <van-dialog title="代购信息修改" v-model="modify_order_diag" :show-confirm-button="false" :show-cancel-button="true" :close-on-click-overlay="true" get-container="body">
                <van-form @submit="onModifyOrder">
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

        </van-tab>
    </van-tabs>
</div>
</template>

<script>
import {
    Base64
} from 'js-base64'
import wx from 'weixin-js-sdk'
export default {
    name: 'DgOrder',
    data: function () {
        return {
            is_login: false,
            created_by_me: [],
            joined_by_me: [],
            active: 0,
            order_brief: {
                id: 0,
                destination: '',
                start_time: '',
                deliver_time: '',
                comments: '',
                contact_qr: '',
            },
            modify_order_diag: false,
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
        on_start_time_confirm: function (date) {
            this.show_start_calendar = false;
            this.order_brief.start_time = `${date.getFullYear()}-${date.getMonth() + 1}-${date.getDate()}`;
        },
        on_deliver_time_confirm: function (date) {
            this.show_deliver_calendar = false;
            this.order_brief.deliver_time = `${date.getFullYear()}-${date.getMonth() + 1}-${date.getDate()}`;
        },
        onModifyOrder: function () {
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
                        vue_this.origModifyOrder(res_server.serverId);
                    },
                });
            } else {
                this.origModifyOrder('');
            }
        },

        origModifyOrder: function (_contact_qr) {
            console.log(this.order_brief);
            this.modify_order_diag = false;
            var vue_this = this;
            Base64.extendString();
            this.$axios.post(this.$remote_rest_url_header + 'order_brief_change', {
                ssid: vue_this.$cookies.get('ssid'),
                order_brief: {
                    id: vue_this.order_brief.id,
                    start_time: vue_this.order_brief.start_time,
                    deliver_time: vue_this.order_brief.deliver_time,
                    destination: vue_this.order_brief.destination.toBase64(),
                    comments: vue_this.order_brief.comments.toBase64(),
                    contact_qr: _contact_qr,
                },
            }).then(function (resp) {
                console.log(resp);
                vue_this.get_created_orders();
                vue_this.$toast.clear();
            }).catch(function (err) {
                console.log(err);
            });
        },
        pop_out_modify: function (_order) {
            console.log(_order);
            this.order_brief.destination = _order.destination;
            this.order_brief.start_time = _order.start_time;
            this.order_brief.deliver_time = _order.deliver_time;
            this.order_brief.comments = _order.comments;
            this.order_brief.id = _order.id;
            this.modify_order_diag = true;
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
            Base64.extendString();
            this.$axios.get(this.$remote_rest_url_header + 'order_created/' + ssid).then(function (resp) {
                resp.data.result.forEach((element, index) => {
                    vue_this.$set(vue_this.created_by_me, index, {
                        id: element.id,
                        user_name: element.user_name.fromBase64(),
                        user_logo: element.user_logo,
                        destination: element.destination.fromBase64(),
                        start_time: element.start_time,
                        deliver_time: element.deliver_time,
                        comments: element.comments.fromBase64(),
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
        this.config_with_wx();
    },
}
</script>

<style>

</style>
