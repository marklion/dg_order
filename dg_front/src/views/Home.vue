<template>
<div class="home">
    <el-container class="main_container">
        <el-header class="header">
            <el-row style="height: 100%; line-height: 100%;">
                <el-col :span="16" style="height: 100%; line-height: 100%;">
                    <h1 class="app_title">团代通</h1>
                </el-col>
                <el-col :span="4" v-if="is_login" style="height: 100%; line-height: 100%;">
                    {{user_name}}
                </el-col>
                <el-col :span="4" v-else style="height: 100%; line-height: 100%;">
                    未登录
                </el-col>
                <el-col :span="4" style="height: 100%; line-height: 100%;">
                    <el-avatar :src="user_logo" fit="cover" :size="60">
                        <img src="https://cube.elemecdn.com/e/fd/0fc7d20532fdaf769a25683617711png.png" />
                    </el-avatar>
                </el-col>
            </el-row>
        </el-header>
        <el-main class="main_show">
            <el-row style="height: 30%"></el-row>
            <h1>批号查询</h1>
            <el-row :gutter="10">
                <el-col :span="18">
                    <el-input clearable v-model="dg_order_number" placeholder="请输入购物批号"></el-input>
                </el-col>
                <el-col :span="6">
                    <el-button type="primary" @click="query_order">查询</el-button>
                </el-col>
            </el-row>
            <el-row style="height: 6%"></el-row>
            <el-row class="order_create" :gutter="10">
                <el-col :span="12">
                    <el-button type="primary" @click="dialog_show=true">创建代购</el-button>
                </el-col>
                <el-col :span="12">
                    <el-button type="primary">创建团购</el-button>
                </el-col>
            </el-row>
        </el-main>
    </el-container>
    <el-dialog title="请填写代购配置" :visible.sync="dialog_show" :modal-append-to-body="false" width="80%">
        <el-form ref="form" :model="dg_form">
            <el-form-item label="代购目的地">
                <el-input v-model="dg_form.destination"></el-input>
            </el-form-item>
            <el-form-item label="代购日期">
                <el-date-picker v-model="dg_form.start_time" type="date" placeholder="选择日期" format="yyyy 年 MM 月 dd 日" value-format="yyyy-MM-dd">
                </el-date-picker>
            </el-form-item>
            <el-form-item label="预计发货日期">
                <el-date-picker v-model="dg_form.deliver_time" type="date" placeholder="选择日期" format="yyyy 年 MM 月 dd 日" value-format="yyyy-MM-dd">
                </el-date-picker>
            </el-form-item>
            <el-form-item label="备注">
                <el-input type="textarea" :rows="3" placeholder="请输入内容" v-model="dg_form.comments">
                </el-input>
            </el-form-item>
            <el-form-item>
                <el-row :gutter="5">
                    <el-col :span="12">
                        <el-button type="primary" @click="dg_order_created">立即创建</el-button>
                    </el-col>
                    <el-col :span="12">
                        <el-button @click="dialog_show=false">取消</el-button>
                    </el-col>
                </el-row>
            </el-form-item>
        </el-form>
    </el-dialog>
</div>
</template>

<script>
import {
    Base64
} from 'js-base64'
export default {
    name: 'Home',
    data: function () {
        return {
            dg_order_number: "",
            user_name: "",
            user_logo: "",
            is_login: false,
            dialog_show: false,
            dg_form: {
                destination: "",
                start_time: "",
                deliver_time: "",
                comments: "",
            },
        };
    },
    methods: {
        get_user_info: function () {
            var vue_this = this;
            var ssid = this.$cookies.get('ssid');
            console.log(this.$remote_rest_url_header);
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
        query_order: function () {
            this.$router.push({
                path: '/dg_order/' + this.dg_order_number
            })
        },
        nav_to_wx_login: function () {
            window.location.href = "https://open.weixin.qq.com/connect/oauth2/authorize?appid=wxa390f8b6f68e9c6d&redirect_uri=http%3a%2f%2fwww.d8sis.cn%2fwechatlogin&response_type=code&scope=snsapi_userinfo&state=%2f#wechat_redirect"
        },
        dg_order_created: function () {
            var vue_this = this;
            Base64.extendString();
            var created_order_req = {
                order: {
                    ssid: this.$cookies.get("ssid"),
                    info: {
                        destination: this.dg_form.destination.toBase64(),
                        start_time: this.dg_form.start_time,
                        deliver_time: this.dg_form.deliver_time,
                        comments: this.dg_form.comments.toBase64(),
                    }
                }
            };
            console.log(created_order_req);
            this.$axios.post(this.$remote_rest_url_header + "dg_order", created_order_req).then(function (resp) {
                vue_this.$router.push({
                    path: '/dg_order/' + resp.data.result
                });
            }).catch(function (err) {
                console.log(err);
            });
        },
    },
    beforeMount: function () {
        this.get_user_info();
    },
}
</script>

<style scoped>
.home {
    position: fixed;
    background-color: rgb(253, 246, 236);
    height: 100%;
    width: 100%;
}

p {
    margin: 0px;
}

.main_container {
    height: 100%;
    text-align: center;
}

.common_center_height {
    height: 100%;
    text-align: center;
    line-height: 100%;
}

.header {
    height: 10%;
    background-color: green;
    text-align: left;
}

.main_show {
    height: 80%;
}

.footer {
    height: 10%;
}

.el-button {
    display: block;
    width: 100%;
    padding-left: 4px;
    padding-right: 4px;
    margin-left: 0px;
}
</style>
