<template>
<div class="my_goods_show" v-if="is_login">
    <van-nav-bar :title="order_brief.order_owner_name + order_brief.destination + '代购'" :fixed="true" :placeholder="true" :safe-area-inset-top="true">
        <template #left>
            <van-image :src="order_brief.order_owner_logo" height="45" width="45" round></van-image>
        </template>
    </van-nav-bar>
    <van-row :gutter="5" type="flex" align="center" class="status_show">
        <van-col :span="12">
            <div>{{status()}}</div>
        </van-col>
        <van-col :span="12">
            <div class="date_show">
                出发日期:{{order_brief.start_time}}
            </div>
            <div class="date_show">
                发货日期:{{order_brief.deliver_time}}
            </div>
        </van-col>
    </van-row>
    <van-notice-bar left-icon="volume-o" :text="order_brief.comments" />
    <van-divider>左滑修改规格或删除</van-divider>
    <van-button block size="small" type="info" round v-if="!sub_status" @click="show_sub_qr">关注公众号,获取订单状态通知</van-button>
    <br>
    <van-button icon="exchange" type="warning" round :url="'/dg_order/' + get_order_number()" block size="small">查看全部</van-button>
    <van-swipe-cell v-for="(good, index) in goods" :key="index">
        <van-card :num="good.number" :title="good.name" @click-thumb="zoom_picture(good.picture)">
            <template #desc>
                <div class="spec_status_show" v-text="good.spec + '-->' + status_show_str(good.status)"></div>
                <div class="spec_status_show" v-if="good.status == 'delivered'">
                    快递单号：{{good.express}}
                </div>
            </template>
            <template #title>
                <div class="good_name_show">
                    {{good.name}}
                </div>
            </template>
            <template #thumb>
                <van-image :src="good.picture" fit="cover" height="88" width="88">
                    <template v-slot:error>
                        <van-image src="http://www.d8sis.cn/logo_res/no_pic.jpg"></van-image>
                    </template>
                </van-image>
            </template>
            <template #tags>
                <van-tag plain type="primary" @click="open_address_choose(good)">{{address_show(good.address)}}</van-tag>
            </template>
            <template #price>
                <div>
                    <span v-if="good.price != 0">¥{{good.price}}</span>
                    <van-button round type="primary" size="mini" v-if="good.pending == 'pending' && good.price != 0" @click="confirm_price(good)">确认价格</van-button>
                </div>
            </template>

        </van-card>
        <template #right>
            <van-button square text="修改" type="primary" style="height: 100%" :disabled="disable_to_modify(good)" @click="show_modify_diag(good)" />
            <van-button square text="删除" type="danger" style="height: 100%" :disabled="disable_to_modify(good)" @click="delete_good(good)" />
        </template>
    </van-swipe-cell>

    <van-popup v-model="show_good_img" style="width:100%" @click="show_good_img = false">
        <van-image :src="show_good_img_content" width="100%"></van-image>
    </van-popup>

    <van-dialog :title="'修改订单:' + orig_good.name" v-model="modify_diag" :show-confirm-button="true" :show-cancel-button="true" :close-on-click-overlay="true" @confirm="submit_modify">
        <van-field readonly clickable name="规格" :value="modify_good.spec" label="规格" placeholder="请选择规格" @click="show_selector = true" />
        <specs-selector v-model="show_selector" :good_name="orig_good.name" @spec_confirm="input_spec">
        </specs-selector>
        <van-field name="数量" label="数量">
            <template #input>
                <van-stepper v-model="modify_good.number" />
            </template>
        </van-field>
    </van-dialog>
</div>
</template>

<script>
import {
    Base64
} from 'js-base64'
import SpecSelector from '../components/SpecSelector.vue'
import {
    ImagePreview
} from 'vant';
import wx from 'weixin-js-sdk'
export default {
    name: 'MyGoods',
    data: function () {
        return {
            disable_to_modify:function(_good) {
                var ret = true;
                if (_good.status == 'booking') {
                    ret = false;
                }

                return ret;
            },
            address_show: function (_address) {
                var ret = '点击选择收货地址';
                if (_address != '') {
                    ret = _address
                }

                return ret;
            },
            status_show_str: function (_status) {
                var ret = '预定';

                switch (_status) {
                    case 'booking':
                        ret = '预定';
                        break;
                    case 'bought':
                        ret = '已购买'
                        break;
                    case 'delivered':
                        ret = '已发货'
                        break;
                    default:
                        break;
                }

                return ret;
            },
            show_good_img: false,
            show_good_img_content: '',
            show_selector: false,
            orig_good: {
                name: '',
                spec: '',
            },
            modify_good: {
                spec: '',
                number: 1
            },
            modify_diag: false,
            is_login: false,
            order_number: '',
            order_brief: {
                order_owner_name: '',
                order_owner_logo: '',
                destination: '',
                start_time: '',
                deliver_time: '',
                comments: ''
            },
            sub_status: false,
            status: function () {
                var cur_date = new Date();
                var start_time_str = this.order_brief.start_time.split('-').join('/');
                var start_time = new Date(start_time_str);
                var deliver_time_str = this.order_brief.deliver_time.split('-').join('/');
                var deliver_time = new Date(deliver_time_str);
                if (cur_date < start_time) {
                    return '火热预定中';
                } else if (cur_date < deliver_time) {
                    return '血拼中';
                } else {
                    return '已结束';
                }
            },
            goods: [],
            is_ready: false,
        };
    },
    components: {
        'specs-selector': SpecSelector
    },
    methods: {
        confirm_price: function (_good) {
            var vue_this = this;
            vue_this.$axios.post(vue_this.$remote_rest_url_header + 'confirm_pending', {
                ssid: vue_this.$cookies.get('ssid'),
                order_id: vue_this.get_order_number(),
                name:_good.name.toBase64(),
                spec:_good.spec.toBase64(),
                pending:'confirm',
            }).then(function(resp) {
                if (resp.data.result == true)
                {
                    vue_this.refresh_good_show();
                }
            }).catch(function(err) {
                console.log(err);
            });
        },
        show_sub_qr: function () {
            ImagePreview(['http://www.d8sis.cn/logo_res/sub_qr.png']);
        },
        open_address_choose: function (_good) {
            var vue_this = this;
            Base64.extendString();
            wx.openAddress({
                success: function (res) {
                    var final_addr = '';
                    final_addr = res.provinceName + ' ';
                    final_addr += res.cityName + '';
                    final_addr += res.detailInfo + '\r\n';
                    final_addr += res.userName + ' ' + res.telNumber;
                    console.log(final_addr);
                    vue_this.$axios.post(vue_this.$remote_rest_url_header + 'update_address', {
                        ssid: vue_this.$cookies.get('ssid'),
                        order_id: vue_this.get_order_number(),
                        name: _good.name.toBase64(),
                        spec: _good.spec.toBase64(),
                        address: final_addr.toBase64(),
                    }).then(function (resp) {
                        vue_this.refresh_good_show();
                        console.log(resp);
                    }).catch(function (err) {
                        console.log(err);
                    });
                }
            });
        },
        zoom_picture: function (_picture) {
            this.show_good_img_content = _picture;
            this.show_good_img = true;
        },
        delete_good: function (_good) {
            var vue_this = this;
            this.$toast.loading({
                message: '加载中...',
                forbidClick: true,
                duration: 0,
            });

            this.$axios.post(this.$remote_rest_url_header + 'delete_order', {
                delete_info: {
                    ssid: this.$cookies.get('ssid'),
                    name: _good.name.toBase64(),
                    spec: _good.spec.toBase64(),
                    order_id: this.get_order_number(),
                }
            }).then(function (resp) {
                console.log(resp);
                vue_this.refresh_good_show();
                vue_this.$toast.clear();
            }).catch(function (err) {
                console.log(err);
            });
        },
        input_spec: function (val) {
            this.modify_good.spec = val;
        },
        show_modify_diag: function (_good) {
            this.orig_good.name = _good.name;
            this.orig_good.spec = _good.spec;
            this.modify_good.spec = _good.spec;
            this.modify_good.number = _good.number;
            this.modify_diag = true;
        },
        submit_modify: function () {
            this.$toast.loading({
                message: '加载中...',
                forbidClick: true,
                duration: 0,
            });
            console.log(this.orig_good);
            console.log(this.modify_good);
            Base64.extendString();
            var vue_this = this;
            this.$axios.post(this.$remote_rest_url_header + 'update_order', {
                update_info: {
                    ssid: this.$cookies.get('ssid'),
                    name: this.orig_good.name.toBase64(),
                    spec: this.orig_good.spec.toBase64(),
                    order_id: this.get_order_number(),
                    content: {
                        spec: this.modify_good.spec.toBase64(),
                        number: this.modify_good.number,
                    }
                }
            }).then(function (resp) {
                console.log(resp);
                vue_this.refresh_good_show();
                vue_this.$toast.clear();
            }).catch(function (err) {
                console.log(err);
            });
        },
        get_order_number: function () {
            return this.$route.params.order_number;
        },
        get_user_info: function () {
            var vue_this = this;
            var ssid = this.$cookies.get('ssid');
            this.$axios.get(this.$remote_rest_url_header + 'user_info/' + ssid).then(function (resp) {
                if (resp.data.result.online == true) {
                    vue_this.is_login = true;
                } else {
                    window.location.href = "https://open.weixin.qq.com/connect/oauth2/authorize?appid=wxa390f8b6f68e9c6d&redirect_uri=http%3a%2f%2fwww.d8sis.cn%2fwechatlogin&response_type=code&scope=snsapi_userinfo&state=%2fmy_goods%2f" + vue_this.order_number + "#wechat_redirect"
                }
                console.log(vue_this);
            }).catch(function (err) {
                console.log(err);
                window.location.href = "https://open.weixin.qq.com/connect/oauth2/authorize?appid=wxa390f8b6f68e9c6d&redirect_uri=http%3a%2f%2fwww.d8sis.cn%2fwechatlogin&response_type=code&scope=snsapi_userinfo&state=%2fmy_goods%2f" + vue_this.order_number + "#wechat_redirect"
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

        refresh_good_show: function () {
            Base64.extendString();
            var vue_this = this;
            this.$axios.post(this.$remote_rest_url_header + 'my_order/', {
                ssid: this.$cookies.get('ssid'),
                order_id: this.get_order_number(),
            }).then(function (resp) {
                console.log(resp);
                vue_this.goods = [];
                resp.data.result.forEach((element, index) => {
                    vue_this.$set(vue_this.goods, index, {
                        name: element.name.fromBase64(),
                        picture: vue_this.$remote_url + element.picture,
                        spec: element.spec.fromBase64(),
                        number: element.number,
                        status: element.status,
                        address: element.address.fromBase64(),
                        express: element.express,
                        price: element.price,
                        pending: element.pending,
                    });
                });
            }).catch(function (err) {
                console.log(err);
            });
        },
        get_sub_status: function () {
            var vue_this = this;
            vue_this.$axios.get(this.$remote_rest_url_header + "sub_status/" + this.$cookies.get('ssid')).then(function (resp) {
                console.log(resp);
                vue_this.sub_status = resp.data.result;
            }).catch(function (err) {
                console.log(err);
            });
        }
    },
    beforeMount: function () {
        this.order_number = this.get_order_number();
        var vue_this = this;
        this.$axios.get(this.$remote_rest_url_header + 'dg_order/' + this.order_number).then(function (resp) {
            Base64.extendString();
            console.log(resp);
            vue_this.order_brief.destination = resp.data.result.info.destination.fromBase64();
            vue_this.order_brief.start_time = resp.data.result.info.start_time;
            vue_this.order_brief.deliver_time = resp.data.result.info.deliver_time;
            vue_this.order_brief.comments = resp.data.result.info.comments.fromBase64();
            vue_this.order_brief.order_owner_name = resp.data.result.order_owner_name.fromBase64();
            vue_this.order_brief.order_owner_logo = vue_this.$remote_url + resp.data.result.order_owner_logo;
        }).catch(function (err) {
            console.log(err);
        });
        this.get_user_info();
        this.refresh_good_show();
        this.get_sub_status();
    },
    mounted: function () {
        this.$toast.loading({
            message: '正在打开',
            forbidClick: true,
            duration: 3000,
        });
        var vue_this = this;
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
                jsApiList: ['OpenAddress'],
                openTagList: ['wx-open-subscribe'],
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
    }
}
</script>

<style scoped>
.date_show {
    color: rgb(235, 74, 135);
    text-align: center;
    background-color: rgb(159, 185, 201);
    font-size: 12px;
}

.status_show {
    font-weight: bold;
    color: rgb(255, 0, 0);
    text-align: center;
    background-color: rgb(125, 216, 211);
}

.good_name_show {
    font-size: 20px;
    font-weight: bold;
}

.spec_status_show {
    font-size: 14px;
    color: red;
}

.address_show {
    background-color: rgb(209, 209, 209);
    color: rgb(18, 126, 67);
}
</style>
