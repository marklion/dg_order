<template>
<div class="change_status_show">
    <div class="price_show" v-if="price != '' && price">
        <van-row :gutter="5">
            <van-col :span="12" style="text-decoration:underline">
                <div @click="show_price_diag = true">价格：{{price}}</div>
            </van-col>
            <van-col :span="12">
                {{pending_show()}}
            </van-col>
        </van-row>
    </div>
    <van-tag v-else type="warning" @click="show_price_diag = true">点击设置价格</van-tag>
    <van-steps :active="num_status" @click-step="prepare_status_change">
        <van-step>预定</van-step>
        <van-step>已购买</van-step>
        <van-step>已发货</van-step>
    </van-steps>
    <div class="express_show" v-if="express != ''">快递单号：{{express}}</div>
    <van-dialog v-model="show_status_check_diag" title="请填入快递号" @confirm="record_express_order" show-cancel-button showConfirmButton closeOnClickOverlay getContainer="body">
        <div>
            <van-cell title="收货地址" :label="address_show()" />
            <van-field v-model="express_order" name="快递单号" label="快递单号" placeholder="快递单号" :rules="[{ required: true, message: '请填写快递单号' }]" />
        </div>
    </van-dialog>
    <van-dialog v-model="show_price_diag" title="请输入价格" @confirm="set_price" show-cancel-button showConfirmButton closeOnClickOverlay getContainer="body">
        <div>
            <van-field v-model="user_set_price" name="价格" label="价格" :rules="[{ required: true, message: '请填写价格' }]" />
        </div>
    </van-dialog>
</div>
</template>

<script>
export default {
    name: 'ChangeStatus',
    props: {
        good_order_id: Number,
        cur_status: String,
        address: String,
        express: String,
        price: String,
        pending: String,
    },
    data: function () {
        return {
            show_price_diag: false,
            user_set_price: this.price,
            express_order: '',
            address_show: function () {
                var ret = '客人未指定收货地址';
                if (this.address != '') {
                    ret = this.address;
                }

                return ret;
            },
            num_status: 0,
            show_status_check_diag: false,
            pending_show: function () {
                var ret = '未确认';
                if (this.pending == 'confirm') {
                    ret = '已确认';
                }

                return ret;
            }
        };
    },
    watch: {
        cur_status(_val) {
            if (_val == 'booking') {
                this.num_status = 0;
            } else if (_val == 'bought') {
                this.num_status = 1;
            } else if (_val == 'delivered') {
                this.num_status = 2;
            }
        },
    },
    methods: {
        set_price: function () {
            if (this.user_set_price != this.price) {
                this.$emit('set_price', {
                    id: this.good_order_id,
                    price: this.user_set_price,
                });
            }

        },
        record_express_order: function () {
            var vue_this = this;
            vue_this.$axios.post(this.$remote_rest_url_header + 'update_express', {
                ssid: vue_this.$cookies.get('ssid'),
                id: vue_this.good_order_id,
                express: vue_this.express_order,
            }).then(function (resp) {
                vue_this.handle_change(2);
                console.log(resp);
            }).catch(function (err) {
                console.log(err);
            });
        },
        prepare_status_change: function (_status) {
            if (_status == 2) {
                this.show_status_check_diag = true;
            } else {
                this.handle_change(_status);
            }
        },
        handle_change: function (_status) {
            this.num_status = _status;
            var str_status = 'booking';
            if (_status == 0) {
                str_status = 'booking';
            } else if (_status == 1) {
                str_status = 'bought';
            } else if (_status == 2) {
                str_status = 'delivered';
            }
            this.$emit('status_change', {
                id: this.good_order_id,
                status: str_status
            });
        }
    },
    beforeMount: function () {
        if (this.cur_status == 'booking') {
            this.num_status = 0;
        } else if (this.cur_status == 'bought') {
            this.num_status = 1;
        } else if (this.cur_status == 'delivered') {
            this.num_status = 2;
        }
        this.express_order = this.express;
    }
}
</script>

<style scoped>
.change_status_show {}

.express_show {
    color: rgb(11, 84, 179);
    font-size: 11px;
}

.price_show {
    color: rgb(177, 102, 5);
    font-size: 12px;
    text-decoration: underline;
}
</style>
