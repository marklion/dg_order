<template>
<div class="dg_order">
    <el-breadcrumb separator-class="el-icon-arrow-right" class="bread-crum">
        <el-breadcrumb-item :to="{ path: '/' }">首页</el-breadcrumb-item>
        <el-breadcrumb-item>{{order_brief.destination +  '代购 购物批号:' + order_number}}</el-breadcrumb-item>
    </el-breadcrumb>
    <el-row :gutter="5" type="flex" align="middle">
        <el-col :span="12">
            托付人：{{order_brief.order_owner_name}}
        </el-col>
        <el-col :span="12">
            <el-avatar :src="order_brief.order_owner_logo" fit="cover" :size="60">
                <img src="https://cube.elemecdn.com/e/fd/0fc7d20532fdaf769a25683617711png.png" />
            </el-avatar>
        </el-col>
    </el-row>
    <el-row :gutter="5" type="flex" align="middle">
        <el-col :span="8">
            出发时间
        </el-col>
        <el-col :span="16">
            <el-date-picker v-model="order_brief.start_time" :readonly="true" type="date" format="yyyy 年 MM 月 dd 日" value-format="yyyy-MM-dd"></el-date-picker>
        </el-col>
    </el-row>
    <el-row :gutter="5" type="flex" align="middle">
        <el-col :span="8">
            预计发货时间
        </el-col>
        <el-col :span="16">
            <el-date-picker v-model="order_brief.deliver_time" :readonly="true" type="date" format="yyyy 年 MM 月 dd 日" value-format="yyyy-MM-dd"></el-date-picker>
        </el-col>
    </el-row>
    <el-collapse v-model="activeNames">
        <el-collapse-item title="备注" name="1">
            <el-input type="textarea" autosize placeholder="请输入内容" v-model="order_brief.comments" :disable="true" :readonly="true">
            </el-input>
        </el-collapse-item>
    </el-collapse>

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
                <el-button @click="add_img">添加图片</el-button>
                <el-image :src="tmp_good_pic" v-if="tmp_good_pic!=''"></el-image>
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
    <el-button type="primary" @click="goods_new_diag = true">新货品</el-button>

    <el-card class="goods_show" v-for="good in goods_from_server" :key="good.name">
        <el-row :guuter="5">
            <el-col :span="8">
                {{good.name}} 共 {{good.total}} 份
            </el-col>
            <el-col :span="16">
                <div v-for="buyer in good.buyer" :key="buyer.user_logo + buyer.spec">
                    {{buyer.user_name}} 要买 {{buyer.number}} 份 {{buyer.spec}}
                </div>
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
            tmp_good_pic:'',
            good_form: {
                name: '',
                spec: '',
                number: 1,
                picture:''
            },
            activeNames: ['1'],
            goods_new_diag: false,
            goods_from_server: [{
                name: '',
                picture: '',
                total: 0,
                buyer: [],
            }],
        }
    },
    methods: {
        get_order_number: function () {
            return this.$route.params.order_number;
        },
        go_back: function () {
            this.$router.go(-1);
        },
        submit_new_good: function () {
            Base64.extendString();
            var vue_this = this;
            this.$axios.post(this.$remote_rest_url_header + 'dg_order/goods', {
                good: {
                    name: this.good_form.name.toBase64(),
                    spec: this.good_form.spec.toBase64(),
                    number: this.good_form.number,
                    ssid: this.$cookies.get('ssid'),
                    order_id: this.order_number,
                    img: this.good_form.picture
                }
            }).then(function (resp) {
                vue_this.refresh_goods();
                console.log(resp);
            }).catch(function (err) {
                console.log(err);
            });
            this.goods_new_diag = false;
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
                        total: element.total,
                        buyer: []
                    });
                    element.buyer.forEach((buyer, b_index) => {
                        vue_this.$set(vue_this.goods_from_server[index].buyer, b_index, {
                            user_name: buyer.user_name.fromBase64(),
                            user_logo: buyer.user_logo,
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
                    wx.uploadImage({
                        localId: res.localIds[0],
                        isShowProgressTips: 1,
                        success: function (res_server) {
                            console.log(res_server.serverId);
                            vue_this.good_form.picture = res_server.serverId;
                            vue_this.tmp_good_pic = res.localIds[0];
                        },
                    });
                },
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
                debug: true,
                appId: 'wxa390f8b6f68e9c6d',
                timestamp: timestamp,
                nonceStr: nonceStr,
                signature: resp.data.result,
                jsApiList: ['updateAppMessageShareData', 'chooseImage', 'uploadImage']
            });
        }).catch(function (err) {
            console.log(err);
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
    },
}
</script>

<style scoped>
.bread-crum {
    font-size: 18px;
}

.el-button {
    display: block;
    width: 100%;
    padding-left: 4px;
    padding-right: 4px;
    margin-left: 0px;
}
</style>
