<template>
<div class="dg_order_good_show" v-if="should_show">
    <van-nav-bar :title="order_brief.order_owner_name + order_brief.destination + '代购'" :fixed="true" :safe-area-inset-top="true">
        <template #left>
            <van-image :src="order_brief.order_owner_logo" height="45" width="45" round></van-image>
        </template>
        <template #right>
            <div @click="showShare = true">
                <van-icon name="share-o" color="black" size="25px">
                </van-icon>
            </div>
        </template>
    </van-nav-bar>
    <van-share-sheet v-model="showShare" title="立即分享给好友" :options="ShareOptions" @select="onSelect" />
    <van-overlay :show="show_share_dir" @click="show_share_dir = false">
        <div class="dot_3_info">请点击三个点分享</div>
    </van-overlay>
    <div style="height: 46px"></div>
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
    <van-dialog title="预定详情" v-model="add_good_order_diag" :show-confirm-button="false" :show-cancel-button="true" :close-on-click-overlay="true">
        <van-form @submit="on_add_good">
            <van-field v-model="good_add_form.name" name="商品名称" label="商品名称" placeholder="商品名称" :rules="[{ required: true, message: '请填写商品名称' }]" />
            <van-field readonly clickable name="规格" :value="good_add_form.spec" label="规格" placeholder="请选择规格" @click="show_selector = true" />
            <specs-selector v-model="show_selector" :good_name="good_add_form.name" @spec_confirm="input_spec">
            </specs-selector>
            <van-field name="数量" label="数量">
                <template #input>
                    <van-stepper v-model="good_add_form.number" />
                </template>
            </van-field>
            <van-field name="图片" label="商品图片" placeholder="点击上传图片" readonly clickable @click="call_wx_upload_image">
                <template #input v-if="good_add_form.picture != ''">
                    <van-image height="100" width="100" :src="good_img_show_out()"></van-image>
                </template>
            </van-field>
            <div style="margin: 16px;">
                <van-button round block type="info" native-type="submit">提交</van-button>
            </div>
        </van-form>
    </van-dialog>
    <van-divider>总计{{goods_total_count()}}件</van-divider>
    <van-row :gutter="10">
        <van-col :span="12">
            <van-button icon="plus" type="info" round @click="add_good_order_diag = true" block size="small">添加心愿</van-button>
        </van-col>
        <van-col :span="12">
            <van-button icon="exchange" type="warning" round :url="'/my_goods/' + get_order_number()" block size="small">只看自己的</van-button>
        </van-col>
    </van-row>
    <van-pull-refresh v-model="isLoading" @refresh="onRefresh">
        <van-swipe-cell v-for="(good, good_index) in goods_from_server" :key="good_index">
            <van-card :num="good.total" @click-thumb="zoom_picture(good.picture)">
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
                <template #desc>
                    <div class="expend_brief" v-text="has_ordered(good) + '(点击展开)'" @click="expend_detail(good_index)">
                    </div>
                    <div class="info_for_append">左滑购同款</div>
                    <van-popup v-model="buyer_expend[good_index]" position="bottom" :style="{ height: '30%' }" get-container="body">
                        <van-grid>
                            <van-grid-item v-for="(buyer, index) in good.buyer" :key="index">
                                <van-image :src="buyer.user_logo" height="35" width="35" round></van-image>
                                <div>
                                    {{buyer.user_name}}
                                </div>
                                <div class="spec_show">
                                    {{buyer.spec}}x{{buyer.number}}
                                </div>
                            </van-grid-item>
                        </van-grid>
                    </van-popup>
                </template>
            </van-card>
            <template #right>
                <van-button style="height:100%" type="primary" text="购同款" @click="open_append_good_diag(good.name)" />
            </template>

        </van-swipe-cell>

    </van-pull-refresh>
    <van-dialog class="mount_po" :title="append_spec_select_name" v-model="append_good_order_diag" :show-confirm-button="false" :show-cancel-button="true" :close-on-click-overlay="true">
        <van-form @submit="append_good">
            <van-field readonly clickable name="规格" :value="good_add_form.spec" label="规格" placeholder="请选择规格" @click="popup_select_spec" />
            <specs-selector v-model="append_select_spec_show" :good_name="append_spec_select_name" @spec_confirm="input_spec">
            </specs-selector>
            <van-field name="数量" label="数量">
                <template #input>
                    <van-stepper v-model="good_add_form.number" />
                </template>
            </van-field>
            <div style="margin: 16px;">
                <van-button round block type="info" native-type="submit">提交</van-button>
            </div>
        </van-form>
    </van-dialog>
    <van-popup v-model="show_good_img" style="width:100%" @click="show_good_img = false">
        <van-image :src="show_good_img_content" width="100%"></van-image>
    </van-popup>
</div>
</template>

<script>
import {
    Base64
} from 'js-base64'
import wx from 'weixin-js-sdk'
import SpecSelector from '../components/SpecSelector.vue'
export default {
    name: 'DgOrderGood',
    components: {
        'specs-selector': SpecSelector,
    },
    data: function () {
        return {
            show_good_img_content: '',
            show_good_img: false,
            show_share_dir: false,
            showShare: false,
            ShareOptions: [{
                    name: '微信',
                    icon: 'wechat'
                },
                {
                    name: '朋友圈',
                    icon: 'wechat-moments'
                },
            ],
            isLoading: false,
            is_ready: false,
            append_spec_select_name: '',
            append_select_spec_show: false,
            show_selector: false,
            order_brief: {
                order_owner_name: '',
                order_owner_logo: '',
                destination: '',
                start_time: '',
                deliver_time: '',
                comments: ''
            },
            is_login: false,
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
            goods_from_server: [],
            buyer_expend: [],
            goods_total_count: function () {
                var ret = 0;
                this.goods_from_server.forEach(element => {
                    ret += element.total;
                });

                return ret;
            },
            good_add_form: {
                name: '',
                spec: '',
                number: 1,
                picture: ''
            },
            add_good_order_diag: false,
            append_good_order_diag: false,
            apple_img: '',

            good_img_show_out: function () {
                var vue_this = this;
                var ret = vue_this.good_add_form.picture;
                if (vue_this.is_apple()) {
                    ret = vue_this.apple_img;
                }

                return ret;
            },

            should_show: function () {
                return this.is_login && this.is_ready;
            },
        };
    },
    methods: {
        zoom_picture: function (_picture) {
            this.show_good_img_content = _picture;
            this.show_good_img = true;
        },
        onSelect: function (_opt) {
            this.showShare = false;
            this.show_share_dir = true;
            console.log(_opt);
        },
        onRefresh: function () {
            this.refresh_goods();
        },
        popup_select_spec: function () {
            this.append_select_spec_show = true;
        },
        input_spec: function (val) {
            this.good_add_form.spec = val;
        },
        open_append_good_diag: function (_name) {
            this.append_spec_select_name = _name;
            this.append_good_order_diag = true;
        },
        append_good: function () {
            Base64.extendString();
            this.good_add_form.name = this.append_spec_select_name
            this.$toast.loading({
                message: '加载中...',
                forbidClick: true,
                duration: 0,
            });
            this.submit_add_good('');
            this.append_good_order_diag = false;
        },
        expend_detail: function (_index) {
            this.$set(this.buyer_expend, _index, true);
        },
        get_order_number: function () {
            return this.$route.params.order_number;
        },
        get_specs_by_name: function (_name) {
            return ['ce' + _name];
        },
        refresh_goods: function () {
            var vue_this = this;
            this.$axios.get(this.$remote_rest_url_header + 'dg_order/goods/' + this.get_order_number()).then(function (resp) {
                resp.data.result.forEach((element, index) => {
                    Base64.extendString();
                    vue_this.$set(vue_this.goods_from_server, index, {
                        name: element.name.fromBase64(),
                        picture: vue_this.$remote_url + element.picture,
                        total: element.total,
                        buyer: []
                    });
                    element.buyer.forEach((buyer, b_index) => {
                        vue_this.$set(vue_this.goods_from_server[index].buyer, b_index, {
                            user_name: buyer.user_name.fromBase64(),
                            user_logo: vue_this.$remote_url + buyer.user_logo,
                            spec: buyer.spec.fromBase64(),
                            number: buyer.number
                        });
                    });
                });
                vue_this.isLoading = false;
                console.log(vue_this.goods_from_server);
            }).catch(function (err) {
                console.log(err);
            });
        },
        has_ordered: function (_good) {
            var ret = "";
            var buyer = _good.buyer;
            var buyer_name = [];
            buyer.forEach(element => {
                if (!buyer_name.includes(element.user_name)) {
                    buyer_name.push(element.user_name);
                }
            });
            buyer_name.forEach((element, index) => {
                if (index >= 3) {
                    return;
                }
                ret += element + ","
            });
            ret = ret.substring(0, ret.length - 1);
            ret += "等用户参与了订购";

            return ret;
        },
        get_user_info: function () {
            var vue_this = this;
            var ssid = this.$cookies.get('ssid');
            var order_number_path = '%2f' + this.get_order_number();
            this.$axios.get(this.$remote_rest_url_header + 'user_info/' + ssid).then(function (resp) {
                if (resp.data.result.online == true) {
                    Base64.extendString();
                    vue_this.is_login = true;
                } else {
                    window.location.href = "https://open.weixin.qq.com/connect/oauth2/authorize?appid=wxa390f8b6f68e9c6d&redirect_uri=http%3a%2f%2fwww.d8sis.cn%2fwechatlogin&response_type=code&scope=snsapi_userinfo&state=%2fdg_order" + order_number_path + "#wechat_redirect"
                }
                console.log(vue_this);
            }).catch(function (err) {
                console.log(err);
                window.location.href = "https://open.weixin.qq.com/connect/oauth2/authorize?appid=wxa390f8b6f68e9c6d&redirect_uri=http%3a%2f%2fwww.d8sis.cn%2fwechatlogin&response_type=code&scope=snsapi_userinfo&state=%2fdg_order" + order_number_path + "#wechat_redirect"
            });
        },
        submit_add_good: function (_picture) {
            var vue_this = this;
            Base64.extendString();
            vue_this.$axios.post(vue_this.$remote_rest_url_header + 'dg_order/goods', {
                good: {
                    name: vue_this.good_add_form.name.toBase64(),
                    spec: vue_this.good_add_form.spec.toBase64(),
                    number: vue_this.good_add_form.number,
                    ssid: vue_this.$cookies.get('ssid'),
                    order_id: vue_this.order_number,
                    img: _picture
                }
            }).then(function (resp) {
                vue_this.add_good_order_diag = false;
                vue_this.refresh_goods();
                vue_this.good_add_form = {
                    name: '',
                    spec: '',
                    picture: '',
                    number: 1
                };
                console.log(resp);
                vue_this.$toast.clear();
            }).catch(function (err) {
                console.log(err);
                vue_this.$toast.clear();
            });
        },
        on_add_good: function () {
            var vue_this = this;
            console.log(this.good_add_form);
            this.$toast.loading({
                message: '加载中...',
                forbidClick: true,
                duration: 0,
            });
            if (vue_this.good_add_form.picture != '') {
                wx.uploadImage({
                    localId: vue_this.good_add_form.picture,
                    isShowProgressTips: 0,
                    success: function (res_server) {
                        console.log(res_server.serverId);
                        vue_this.submit_add_good(res_server.serverId);
                    },
                });
            } else {
                this.submit_add_good('');
            }
        },
        is_apple: function () {
            return window.__wxjs_is_wkwebview;
        },
        call_wx_upload_image: function () {
            var vue_this = this;
            wx.chooseImage({
                count: 1,
                sizeType: ['compressed'],
                success: function (res) {
                    vue_this.good_add_form.picture = res.localIds[0];
                    if (vue_this.is_apple()) {
                        wx.getLocalImgData({
                            localId: vue_this.good_add_form.picture,
                            success: function (res_get) {
                                vue_this.apple_img = res_get.localData;
                            }
                        });
                    }
                },
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
    beforeMount: function () {
        this.$toast.loading({
            message: '正在打开',
            forbidClick: true,
            duration: 3000,
        });

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
        this.refresh_goods();
    },
    mounted: function () {
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
                jsApiList: ['updateAppMessageShareData', 'chooseImage', 'uploadImage', 'getLocalImgData', 'updateTimelineShareData']
            });
            wx.ready(function () {
                console.log('success to config wx');
                wx.updateAppMessageShareData({
                    title: vue_this.order_brief.order_owner_name + '的代购清单',
                    desc: '点击链接参与代购',
                    link: window.location.href,
                    imgUrl: vue_this.order_brief.order_owner_logo,
                    success: function () {
                        console.log('success to set share btn');
                    }
                });
                wx.updateTimelineShareData({
                    title: vue_this.order_brief.order_owner_name + '的代购清单',
                    link: window.location.href,
                    imgUrl: vue_this.order_brief.order_owner_logo,
                    success: function () {
                        console.log('success to set share btn');
                    }
                });
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

.expend_brief {
    text-decoration: underline;
    font-size: 14px;
}

.spec_show {
    color: rgb(48, 8, 8);
    font-size: 12px;
}

.good_name_show {
    font-size: 20px;
    font-weight: bold;
}

.info_for_append {
    color: rgb(206, 33, 62);
}

.dot_3_info {
    color: aliceblue;
    text-align: end;
    font-size: 20px;
}

.share_div {
    color: pink;
}
</style>
