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
    <van-button icon="exchange" type="warning" round :url="'/dg_order/' + get_order_number()" block size="small">查看全部</van-button>
    <van-swipe-cell v-for="(good, index) in goods" :key="index">
        <van-card :num="good.number" :title="good.name" @click-thumb="zoom_picture(good.picture)">
            <template #desc>
                <div class="spec_status_show" v-text="good.spec + '-->' + status_show_str(good.status)"></div>
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

        </van-card>
        <template #right>
            <van-button square text="修改" type="primary" style="height: 100%" @click="show_modify_diag(good)" />
            <van-button square text="删除" type="danger" style="height: 100%" @click="delete_good(good)" />
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
export default {
    name: 'MyGoods',
    data: function () {
        return {
            status_show_str:function(_status) {
                var ret = '预定';

                switch (_status) {
                    case 'booking':
                        ret = '预定';
                        break;
                    case 'bought' :
                        ret = '已购买'
                        break; 
                    case 'delivered' :
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
        };
    },
    components: {
        'specs-selector': SpecSelector
    },
    methods: {
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
                    });
                });
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
    },
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
</style>
