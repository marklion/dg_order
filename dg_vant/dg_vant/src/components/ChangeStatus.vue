<template>
<div class="change_status_show">
    <van-steps :active="num_status" @click-step="handle_change">
        <van-step>预定</van-step>
        <van-step>已购买</van-step>
        <van-step>已发货</van-step>
    </van-steps>
</div>
</template>

<script>
export default {
    name: 'ChangeStatus',
    props: {
        good_order_id: Number,
        cur_status: String,
    },
    data: function () {
        return {
            num_status: 0,
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
    }
}
</script>

<style scoped>
.change_status_show {}
</style>
