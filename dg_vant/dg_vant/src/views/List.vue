<template>
<div class="list_show" v-if="should_show()">
    <van-nav-bar z-index="90" :title="order_brief.order_owner_name + order_brief.destination + '代购'" :fixed="true" :placeholder="true" :safe-area-inset-top="true">
        <template #left>
            <van-image :src="order_brief.order_owner_logo" height="45" width="45" round></van-image>
        </template>
    </van-nav-bar>
    <van-dropdown-menu>
        <van-dropdown-item v-model="cur_filter" :options="filter_options" />
    </van-dropdown-menu>
    <van-tabs v-model="sort_by">
        <van-tab title="按商品分类">
            <van-row>
                <van-col class="vb_show">
                    <div>共{{calc_count(cur_filter)}}件</div>
                    <van-sidebar v-model="good_name">
                        <van-sidebar-item v-for="name in filter_good_name()" :key="name" :title="name" :badge="get_count_by_name(name)" />
                    </van-sidebar>
                </van-col>
                <van-col>
                    <div class="content_show">
                        <van-image :src="get_picture_by_name(filter_good_name()[good_name])" v-if="get_picture_by_name(filter_good_name()[good_name]) != ''"></van-image>
                        <div v-for="(good, index) in get_goods_by_name(filter_good_name()[good_name])" :key="index">
                            <van-row :gutter="5" type="flex" align="center">
                                <van-col :span="3">
                                    <van-image :src="good.user_logo" width="33" height="33" round></van-image>
                                </van-col>
                                <van-col :span="9">
                                    <div>{{good.user_name}}</div>
                                    <div class="spec_show">{{good.spec}}</div>
                                    <van-button size="mini" round type="danger" @click="remove_good(good.id, '无现货')">删除</van-button>
                                </van-col>
                                <van-col :span="12">
                                    <change-status :price="good.price" :pending="good.pending" @set_price="handle_price_set" :express="good.express" :address="good.address" :cur_status="good.status" :good_order_id="good.id" @status_change="handle_change"></change-status>
                                </van-col>
                            </van-row>
                        </div>
                    </div>
                </van-col>
            </van-row>

        </van-tab>
        <van-tab title="按订购人分类">
            <van-row>
                <van-col class="vb_show">
                    <div>共{{calc_count(cur_filter)}}件</div>
                    <van-sidebar v-model="good_buyer">
                        <van-sidebar-item v-for="buyer_logo in filter_buyer()" :key="buyer_logo" :title="get_user_name_by_logo(buyer_logo)" :badge="get_count_by_logo(buyer_logo)" />
                    </van-sidebar>
                </van-col>
                <van-col>
                    <div class="content_show">
                        <van-card v-for="(good, index) in get_good_by_buyer_logo(filter_buyer()[good_buyer])" :key="index" :desc="good.spec" :title="good.name">
                            <template #thumb>
                                <van-image :src="good.picture" fit="cover" height="88" width="88">
                                    <template v-slot:error>
                                        <van-image src="http://www.d8sis.cn/logo_res/no_pic.jpg"></van-image>
                                    </template>
                                </van-image>
                            </template>
                            <template #num>
                                <div class="status_show"></div>
                                <change-status :price="good.price" :pending="good.pending" @set_price="handle_price_set" :express="good.express" :address="good.address" :cur_status="good.status" :good_order_id="good.id" @status_change="handle_change"></change-status>
                            </template>
                            <template #price>
                                <van-button size="mini" round type="danger" @click="remove_good(good.id, '无现货')">删除</van-button>
                            </template>
                        </van-card>
                    </div>
                </van-col>
            </van-row>

        </van-tab>
    </van-tabs>
</div>
</template>

<script>
import {
    Base64
} from 'js-base64'
import ChangeStatus from '../components/ChangeStatus.vue';
import { Dialog } from 'vant';
export default {
    name: 'List',
    components: {
        'change-status': ChangeStatus,
    },
    data: function () {
        return {
            cur_filter: 'all',
            filter_options: [{
                    text: '全部',
                    value: 'all'
                },
                {
                    text: '未购买',
                    value: 'booking'
                },
                {
                    text: '已购买',
                    value: 'bought'
                },
                {
                    text: '已发货',
                    value: 'delivered'
                },
            ],
            calc_count: function (_status) {
                var ret = 0;
                this.all_goods.forEach(element => {
                    if (element.status == _status || _status == 'all') {
                        ret += 1;
                    }
                });

                return ret;
            },
            good_buyer: 0,
            sort_by: 0,
            good_name: 0,
            is_login: false,
            user_is_host: false,
            order_brief: {
                order_owner_name: '',
                order_owner_logo: '',
                destination: '',
                start_time: '',
                deliver_time: '',
                comments: ''
            },
            all_goods: [],
            should_show: function () {
                return this.is_login && this.user_is_host;
            },
            filter_buyer: function () {
                var ret = [];
                var vue_this = this;

                this.all_goods.forEach(element => {
                    if (vue_this.status_in_filter(element.status, vue_this.cur_filter) && ret.indexOf(element.user_logo) == -1) {
                        ret.push(element.user_logo);
                    }
                });
                ret.sort();

                return ret;
            },
            status_in_filter: function (_status, _cur_filter) {
                var ret = false;
                if ('all' == this.cur_filter) {
                    ret = true;
                } else {
                    if (_status == _cur_filter) {
                        ret = true;
                    }
                }

                return ret;
            },
            filter_good_name: function () {
                var ret = [];
                var vue_this = this;
                this.all_goods.forEach(element => {
                    if (vue_this.status_in_filter(element.status, vue_this.cur_filter) && ret.indexOf(element.name) == -1) {
                        ret.push(element.name);
                    }
                });
                ret.sort();
                return ret;
            },
            get_count_by_logo: function (_logo) {
                var ret = 0;
                var vue_this = this;
                this.all_goods.forEach(element => {
                    if (vue_this.status_in_filter(element.status, vue_this.cur_filter) && element.user_logo == _logo) {
                        ret = ret + 1;
                    }
                });

                return ret;
            },
            get_count_by_name: function (_name) {
                var ret = 0;
                var vue_this = this;
                this.all_goods.forEach(element => {
                    if (element.name == _name && vue_this.status_in_filter(element.status, vue_this.cur_filter)) {
                        ret = ret + 1;
                    }
                });
                return ret;
            },
            get_user_name_by_logo: function (_logo) {
                var ret = '';

                this.all_goods.forEach(element => {
                    if (element.user_logo == _logo) {
                        ret = element.user_name;
                        return;
                    }
                });

                return ret;
            },
            get_picture_by_name: function (_name) {
                var ret = '';
                this.all_goods.forEach(element => {
                    if (element.name == _name) {
                        ret = element.picture;
                        return;
                    }
                });

                return ret;
            },
            get_good_by_buyer_logo: function (_logo) {
                var ret = [];
                var vue_this = this;
                this.all_goods.forEach(element => {
                    if (vue_this.status_in_filter(element.status, vue_this.cur_filter) && element.user_logo == _logo) {
                        ret.push(element);
                    }
                });

                return ret;
            },
            get_goods_by_name: function (_name) {
                var ret = [];
                var vue_this = this;

                this.all_goods.forEach(element => {
                    if (_name == element.name && vue_this.status_in_filter(element.status, vue_this.cur_filter)) {
                        ret.push(element)
                    }
                });

                return ret;
            },
        };
    },
    methods: {
        remove_good: function (_id, _comments) {
            var vue_this = this;
            Dialog.confirm({
                    title: '确定要删除吗',
                    message: '删除后会向顾客发送一条通知，并且删除不可恢复',
                })
                .then(() => {
                    Base64.extendString();
                    vue_this.$axios.post(vue_this.$remote_rest_url_header + 'giveup_good', {
                        ssid: vue_this.$cookies.get('ssid'),
                        id: _id,
                        comment: _comments.toBase64(),
                    }).then(function (resp) {
                        console.log(resp);
                        if (resp.data.result == true) {
                            vue_this.get_all_goods();
                        }
                    }).catch(function (err) {
                        console.log(err);
                    });
                })
                .catch(() => {

                });

        },
        handle_price_set: function (_ps) {
            console.log(_ps);
            var vue_this = this;
            this.$axios.post(this.$remote_rest_url_header + 'update_pending', {
                ssid: this.$cookies.get('ssid'),
                id: _ps.id,
                pending: 'pending'
            }).then(function (resp) {
                if (resp.data.result == true) {
                    vue_this.get_all_goods();
                }
            }).catch(function (err) {
                console.log(err);
            });
            this.$axios.post(this.$remote_rest_url_header + 'update_price', {
                ssid: this.$cookies.get('ssid'),
                id: _ps.id,
                price: _ps.price,
            }).then(function (resp) {
                if (resp.data.result == true) {
                    vue_this.get_all_goods();
                }
            }).catch(function (err) {
                console.log(err);
            });
        },
        handle_change: function (_cs) {
            console.log(_cs);
            var vue_this = this;
            this.$axios.post(this.$remote_rest_url_header + 'update_status', {
                ssid: this.$cookies.get('ssid'),
                id: _cs.id,
                status: _cs.status,
            }).then(function (resp) {
                if (resp.data.result == true) {
                    vue_this.get_all_goods();
                }
                console.log(resp);
            }).catch(function (err) {
                console.log(err);
            });
        },
        get_all_goods: function () {
            Base64.extendString();
            var vue_this = this;
            this.all_goods = [];
            vue_this.$axios.get(vue_this.$remote_rest_url_header + 'dg_order/all_goods/' + vue_this.get_order_number()).then(function (resp) {
                console.log(resp);
                resp.data.result.forEach((element, index) => {
                    vue_this.$set(vue_this.all_goods, index, {
                        name: element.name.fromBase64(),
                        id: element.id,
                        picture: vue_this.$remote_url + element.picture,
                        spec: element.spec.fromBase64(),
                        user_name: element.user_name.fromBase64(),
                        user_logo: vue_this.$remote_url + element.user_logo,
                        status: element.status,
                        address: element.address.fromBase64(),
                        express: element.express,
                        price: element.price,
                        pending: element.pending,
                    });
                });
                console.log(vue_this.all_goods);
            }).catch(function (err) {
                console.log(err);
            });
        },
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
        this.get_all_goods();
    },
}
</script>

<style lang="less" scoped>
.content_show {
    width: calc(100vw - 80px);
    height: calc(100vh - 140px);
    overflow: auto;
}

.vb_show {
    height: calc(100vh - 140px);
    overflow: auto;
}

.spec_show {
    font-size: 14px;
    color: gray;
}

.status_show {
    width: 140px;
}
</style>
