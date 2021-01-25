<template>
<div class="self_order_show">
    <el-card class="one_order" :body-style="{padding: '2px'}">
        <div class="order_id_show">
            {{order_brief.order_owner_name + "→"}} {{order_brief.destination + "→"}} {{order_brief.start_time + "出发"}}
        </div>
        <el-card :body-style="{padding: '2px 2px'}" v-for="good in goods" :key="good.name + good.spec">
            <el-row :gutter="5">
                <el-col :span="6">
                    <el-image :src="good.picture" fit="cover" style="height: 80px"></el-image>
                </el-col>
                <el-col :span="18">
                    <el-row :gutter="5">
                        <el-col :span="18">
                            <div class="good_name_show">{{good.name}}</div>
                        </el-col>
                        <el-col :span="6">
                            <div class="good_count_show">已订购x{{good.number}}</div>
                        </el-col>
                    </el-row>
                    <el-row :gutter="5" type="flex" align="middle">
                        <el-col :span="12">
                            <div class="spec_show">{{good.spec}}</div>
                        </el-col>
                        <el-col :span="12">
                            <el-row :gutter="0" type="flex" justify="center">
                                <el-col :span="12" style="text-align:center">
                                    <el-button type="primary" icon="el-icon-edit" circle @click="show_update_diag(good)"></el-button>
                                </el-col>
                                <el-col :span="12" style="text-align:center">
                                    <el-button type="danger" icon="el-icon-delete" circle @click="delete_good(good)"></el-button>
                                </el-col>
                            </el-row>
                        </el-col>
                    </el-row>
                </el-col>
            </el-row>
        </el-card>

        <el-dialog title="修改订单" :visible.sync="good_update_diag" width="80%">
            <el-form ref="form" :model="good_update_content">
                <el-form-item label="规格">
                    <el-select v-model="good_update_content.spec" placeholder="请选择" filterable allow-create>
                    </el-select>
                </el-form-item>
                <el-form-item lable="数量">
                    <el-input-number v-model="good_update_content.number" :min="1" label="货品数量"></el-input-number>
                </el-form-item>
                <el-button type="success" @click="update_good">确定</el-button>
            </el-form>
        </el-dialog>
    </el-card>
</div>
</template>

<script>
import {
    Base64
} from 'js-base64';
export default {
    name: 'self-order-show',
    data: function () {
        return {
            order_brief: {
                destination: '',
                start_time: '',
                deliver_time: '',
                comments: '',
                order_owner_name: '',
                order_owner_logo: '',
            },
            goods: [],
            good_update_diag: false,
            good_update_good_info: {
                name: '',
                spec: '',
            },
            good_update_content: {
                spec: '',
                number: 0
            },
        };
    },
    props: {
        belong_order_id: String,
    },
    methods: {
        delete_good:function(_good) {
            var vue_this = this;
            this.$axios.post(this.$remote_rest_url_header + 'delete_order', {
                delete_info: {
                    ssid: this.$cookies.get('ssid'),
                    name: _good.name.toBase64(),
                    spec: _good.spec.toBase64(),
                    order_id: this.belong_order_id,
                }
            }).then(function(resp) {
                console.log(resp);
                vue_this.refresh_good_show();
            }).catch(function(err) {
                console.log(err);
            });
        },
        show_update_diag: function (_good) {
            this.good_update_good_info.name = _good.name;
            this.good_update_good_info.spec = _good.spec;
            this.good_update_content.spec = _good.spec;
            this.good_update_content.number = _good.number;
            console.log(this.good_update_good_info);
            this.good_update_diag = true;
        },
        update_good: function () {
            Base64.extendString();
            var vue_this = this;
            this.$axios.post(this.$remote_rest_url_header + 'update_order', {
                update_info: {
                    ssid: this.$cookies.get('ssid'),
                    name: this.good_update_good_info.name.toBase64(),
                    spec: this.good_update_good_info.spec.toBase64(),
                    order_id: this.belong_order_id,
                    content: {
                        spec: this.good_update_content.spec.toBase64(),
                        number: this.good_update_content.number,
                    }
                }
            }).then(function (resp) {
                console.log(resp);
                vue_this.good_update_diag = false;
                vue_this.refresh_good_show();
            }).catch(function (err) {
                console.log(err);
            });
        },
        refresh_good_show: function () {
            var vue_this = this;
            this.$axios.post(this.$remote_rest_url_header + 'my_order/', {
                ssid: this.$cookies.get('ssid'),
                order_id: this.belong_order_id,
            }).then(function (resp) {
                console.log(resp);
                vue_this.goods = [];
                resp.data.result.forEach((element, index) => {
                    vue_this.$set(vue_this.goods, index, {
                        name: element.name.fromBase64(),
                        picture: vue_this.$remote_url + element.picture,
                        spec: element.spec.fromBase64(),
                        number: element.number
                    });
                });
            }).catch(function (err) {
                console.log(err);
            });

        }

    },
    beforeMount: function () {
        var vue_this = this;
        Base64.extendString();
        this.$axios.get(this.$remote_rest_url_header + 'dg_order/' + this.belong_order_id).then(function (resp) {
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
        this.refresh_good_show();
    },
}
</script>

<style scoped>
.order_id_show {
    background-color: rgb(238, 238, 238);
    color: rgb(153, 153, 153);
}

.good_name_show {
    font-weight: bold;
}

.good_spec_show {
    background-color: rgb(240, 240, 240);
    color: rgb(59, 56, 56);
    font-size: 13px;
}

.good_count_show {
    border-radius: 0px;
    border: 1px solid rgb(112, 112, 112);
    text-align: center;
    color: rgb(88, 88, 88);
    font-size: 11px;
    margin-top: 2px;
}

.spec_show {
    height: 60px;
    background-color: rgb(240, 240, 240);
    color: rgb(59, 56, 56);
    font-size: 13px;
}
</style>
