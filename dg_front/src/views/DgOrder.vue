<template>
<div class="dg_order">
    <div class="order_header">
        <el-row :gutter="5" type="flex" align="middle">
            <el-col :span="6">
                <el-button type="text" @click="go_back" style="height: 30px; padding: 0px 0px;">我也做代购</el-button>
            </el-col>
            <el-col :span="18" class="order_brief_show">
                <el-row :gutter="5" type="flex" align="middle">
                    <el-col :span="3">
                        <el-avatar :src="order_brief.order_owner_logo" fit="cover" :size="30">
                        </el-avatar>
                    </el-col>
                    <el-col :span="21">
                        {{order_brief.order_owner_name + '      '}}{{order_brief.destination +  '代购预定单'}}
                    </el-col>
                </el-row>
            </el-col>
        </el-row>
    </div>
    <el-row :gutter="5" type="flex" align="middle" class="order_date_detail">
        <el-col :span="12">
            <el-row :gutter="5" type="flex" align="middle">
                <el-col :span="10">
                    出发时间
                </el-col>
                <el-col :span="14">
                    {{order_brief.start_time}}
                </el-col>
            </el-row>
            <el-row :gutter="5" type="flex" align="middle">
                <el-col :span="10">
                    预计发货
                </el-col>
                <el-col :span="14">
                    {{order_brief.deliver_time}}
                </el-col>
            </el-row>
        </el-col>
        <el-col :span="12">
            <el-button type="text" @click="comments_show = true">查看备注</el-button>
        </el-col>
    </el-row>

    <el-drawer title="备注" :visible.sync="comments_show" direction="btt">
        <el-input type="textarea" autosize placeholder="请输入内容" v-model="order_brief.comments" :disable="true" :readonly="true">
        </el-input>
    </el-drawer>

    <el-dialog title="请输入货品细节" :visible.sync="goods_new_diag" width="80%">
        <el-form ref="form" :model="good_form">
            <el-form-item label="货品名称">
                <el-input v-model="good_form.name"></el-input>
            </el-form-item>
            <el-form-item label="货品规格">
                <el-input v-model="good_form.spec"></el-input>
            </el-form-item>
            <el-form-item label="货品数量">
                <el-input-number v-model="good_form.number" :min="1" label="货品数量"></el-input-number>
            </el-form-item>
            <el-form-item label="参考图片">
                <el-button @click="add_img" v-if="tmp_good_pic==''">添加图片</el-button>
                <el-button @click="tmp_good_pic=''" v-else>取消图片</el-button>
                <el-image :src="is_apple()?tmp_good_show:tmp_good_pic" v-if="tmp_good_pic!=''" style="width: 100px;height: 100px" :fit="cover"></el-image>
            </el-form-item>
        </el-form>
        <el-row :gutter="5" slot="footer">
            <el-col :span="12">
                <el-button @click="goods_new_diag = false">取 消</el-button>
            </el-col>
            <el-col :span="12">
                <el-button type="primary" @click="submit_new_good">确 定</el-button>
            </el-col>
        </el-row>
    </el-dialog>
    <el-row :gutter="10" type="flex" align="middle" class="opt_zone">
        <el-col :span="12">
            <el-button type="primary" round @click="show_new_good_diag" class="el-icon-circle-plus-outline">添加心愿</el-button>
        </el-col>
        <el-col :span="12">
            <el-button type="primary" round @click="show_new_good_diag">我参与的</el-button>
        </el-col>
    </el-row>

    <el-dialog title="请指定规格和数量" :visible.sync="goods_add_diag" width="80%">
        <el-form ref="form" :model="add_good_form">
            <el-form-item lable="规格">
                <el-select v-model="add_good_form.spec" placeholder="请选择" filterable allow-create>
                    <el-option v-for="itr_buyer in good_add_focus.buyer" :key="itr_buyer.user_logo + itr_buyer.spec" :lable="itr_buyer.spec" :value="itr_buyer.spec"></el-option>
                </el-select>
            </el-form-item>
            <el-form-item lable="数量">
                <el-input-number v-model="add_good_form.number" :min="1" label="货品数量"></el-input-number>
            </el-form-item>
        </el-form>
        <el-button @click="add_buy(good_add_focus)">确定</el-button>
    </el-dialog>

    <el-card class="goods_show" v-for="good in goods_from_server" :key="good.name" :body-style="{padding: '0px 0px'}">
        <el-row :gutter="5" type="flex" align="middle" class="good_show_header">
            <el-col :span="12">{{good.name}}</el-col>
            <el-col :span="6">{{good.total}}份</el-col>
            <el-col :span="6">
                <el-button type="primary" @click="show_add_good_diag(good)">我要同款</el-button>
            </el-col>
        </el-row>
        <el-row :guuter="5">
            <el-col :span="8">
                <el-image fit="cover" style="height: 100px" :src="good.picture"></el-image>
            </el-col>
            <el-col :span="16">
                <el-table :data="good.buyer" stripe style="width: 100%" max-height="100" :show-header="false" :row-style="buyer_style" :cell-style="{padding: '2px 0px', color: 'red'}">
                    <el-table-column label="头像" width="30">
                        <template slot-scope="scope">
                            <el-avatar :src="scope.row.user_logo" fit="cover" :size="20">
                            </el-avatar>
                        </template>
                    </el-table-column>
                    <el-table-column label="用户" prop="user_name" width="100"></el-table-column>
                    <el-table-column label="规格" prop="spec" width="80"></el-table-column>
                    <el-table-column label="数量" prop="number"></el-table-column>
                </el-table>
            </el-col>
        </el-row>
    </el-card>
</div>
</template>

<script>
import {
    Base64
} from 'js-base64'
import wx from 'weixin-js-sdk'
export default {
    name: "DgOrder",
    data: function () {
        return {
            order_number: '',
            order_brief: {
                destination: '',
                start_time: '',
                deliver_time: '',
                comments: '',
                order_owner_name: '',
                order_owner_logo: '',
            },
            tmp_good_pic: '',
            tmp_good_show: '',
            good_form: {
                name: '',
                spec: '',
                number: 1,
                picture: ''
            },
            goods_new_diag: false,
            goods_from_server: [],
            is_apple: function () {
                return window.__wxjs_is_wkwebview;
            },
            is_login: false,
            goods_add_diag: false,
            good_add_focus: {},
            add_good_form: {
                spec: '',
                number: 1,
            },
            buyer_style: {
                height: '10px',
            },
            comments_show: true,
        }
    },
    methods: {
        show_new_good_diag: function () {
            if (this.is_login != true) {
                window.location.href = "https://open.weixin.qq.com/connect/oauth2/authorize?appid=wxa390f8b6f68e9c6d&redirect_uri=http%3a%2f%2fwww.d8sis.cn%2fwechatlogin&response_type=code&scope=snsapi_userinfo&state=%2fdg_order%2f" + this.get_order_number() + "#wechat_redirect"
            }
            this.goods_new_diag = true;
        },
        show_add_good_diag: function (_good) {
            if (this.is_login != true) {
                window.location.href = "https://open.weixin.qq.com/connect/oauth2/authorize?appid=wxa390f8b6f68e9c6d&redirect_uri=http%3a%2f%2fwww.d8sis.cn%2fwechatlogin&response_type=code&scope=snsapi_userinfo&state=%2fdg_order%2f" + this.get_order_number() + "#wechat_redirect"
            }
            this.good_add_focus = _good;
            this.goods_add_diag = true;
        },
        get_order_number: function () {
            return this.$route.params.order_number;
        },
        go_back: function () {
            this.$router.push({
                path: '/'
            })
        },
        add_buy: function (_good) {
            var vue_this = this;
            Base64.extendString();
            vue_this.$axios.post(vue_this.$remote_rest_url_header + 'dg_order/goods', {
                good: {
                    name: _good.name.toBase64(),
                    spec: vue_this.add_good_form.spec.toBase64(),
                    number: vue_this.add_good_form.number,
                    ssid: vue_this.$cookies.get('ssid'),
                    order_id: vue_this.order_number,
                    img: _good.picture
                }
            }).then(function (resp) {
                vue_this.goods_add_diag = false;
                vue_this.refresh_goods();
                vue_this.add_good_form = {
                    spec: '',
                    number: 1
                };
                vue_this.good_add_focus = {};
                console.log(resp);
            }).catch(function (err) {
                console.log(err);
            });
        },
        submit_good_orig: function (serverId) {
            var vue_this = this;
            vue_this.good_form.picture = serverId;
            vue_this.$axios.post(vue_this.$remote_rest_url_header + 'dg_order/goods', {
                good: {
                    name: vue_this.good_form.name.toBase64(),
                    spec: vue_this.good_form.spec.toBase64(),
                    number: vue_this.good_form.number,
                    ssid: vue_this.$cookies.get('ssid'),
                    order_id: vue_this.order_number,
                    img: vue_this.good_form.picture
                }
            }).then(function (resp) {
                vue_this.goods_new_diag = false;
                vue_this.refresh_goods();
                vue_this.good_form = {
                    name: '',
                    spec: '',
                    picture: '',
                    number: 1
                };
                console.log(resp);
            }).catch(function (err) {
                console.log(err);
            });
        },
        submit_new_good: function () {
            Base64.extendString();
            var vue_this = this;
            if (this.tmp_good_pic != '') {
                wx.uploadImage({
                    localId: vue_this.tmp_good_pic,
                    isShowProgressTips: 0,
                    success: function (res_server) {
                        console.log(res_server.serverId);
                        vue_this.submit_good_orig(res_server.serverId);
                    },
                });
            } else {
                vue_this.submit_good_orig("none");
            }
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
        refresh_goods: function () {
            var vue_this = this;
            this.$axios.get(this.$remote_rest_url_header + 'dg_order/goods/' + this.order_number).then(function (resp) {
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
                console.log(vue_this.goods_from_server);
            }).catch(function (err) {
                console.log(err);
            });

        },
        add_img: function () {
            var vue_this = this;
            wx.chooseImage({
                count: 1,
                sizeType: ['compressed'],
                success: function (res) {
                    if (vue_this.is_apple()) {
                        wx.getLocalImgData({
                            localId: res.localIds[0],
                            success: function (res_get) {
                                vue_this.tmp_good_show = res_get.localData;
                            }
                        });
                    }
                    vue_this.tmp_good_pic = res.localIds[0];
                },
            });
        },
        get_user_info: function () {
            var vue_this = this;
            var ssid = this.$cookies.get('ssid');
            console.log(this.$remote_rest_url_header);
            this.$axios.get(this.$remote_rest_url_header + 'user_info/' + ssid).then(function (resp) {
                if (resp.data.result.online == true) {
                    vue_this.is_login = true;
                }
                console.log(vue_this);
            }).catch(function (err) {
                console.log(err);
            });
        },

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
        this.refresh_goods();
        this.get_user_info();
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
                jsApiList: ['updateAppMessageShareData', 'chooseImage', 'uploadImage', 'getLocalImgData']
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
.bread-crum {
    font-size: 18px;
}

.dg_order {
    background-color: rgb(253, 246, 236);
}

.el-button {
    display: block;
    width: 100%;
    padding-left: 4px;
    padding-right: 4px;
    margin-left: 0px;
}

.goods_show {
    margin-bottom: 5px;
    background-color: rgb(197, 207, 207);
}

.good_show_header {
    background-color: rgb(226, 224, 117);
    padding: 0px 0px;
}

.order_brief_show {
    background-color: rgb(105, 162, 209);
}

.order_date_detail {
    font-size: 12px;
    color: rgb(182, 29, 29);
    background-color: rgb(216, 191, 213);
}

.opt_zone {
    width: 80%;
    position: fixed;
    bottom: 60px;
    left: 10%;
    z-index: 99;
    opacity: 0.8;
}
</style>
