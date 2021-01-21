<template>
<div class="self_order_show">
    <el-card class="one_order">
        <div class="order_id_show">
            {{order_brief.order_owner_name}}
        </div>
    </el-card>
</div>
</template>

<script>
import { Base64 } from 'js-base64';
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
        };
    },
    props: {
        belong_order_id: String,
    },
    beforeMount: function () {
        var vue_this = this;
        Base64.extendString();
        this.$axios.get(this.$remote_rest_url_header + 'dg_order/' + this.belong_order_id).then(function (resp) {
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
    },
}
</script>

<style scoped>

</style>
