<template>
<div class="self_order_show">
    <el-card class="one_order" :body-style="{padding: '2px'}">
        <div class="order_id_show">
            {{order_brief.order_owner_name + "→"}} {{order_brief.destination + "→"}} {{order_brief.start_time + "出发"}}
        </div>
        <el-table :data="goods" stripe style="width: 100%" :fig="true" :show-header="false" :cell-style="{padding: '2px 0px'}">
            <el-table-column label="图片" width="80">
                <template slot-scope="scope">
                    <el-image :src="scope.row.picture" fit="cover" style="height: 60px"></el-image>
                </template>
            </el-table-column>
            <el-table-column lable="名称" prop="name" width="60"></el-table-column>
            <el-table-column lable="规格" prop="spec" width="60"></el-table-column>
            <el-table-column lable="数量">
                <template slot-scope="scope">
                    <el-input-number size="mini" v-model="scope.row.number"></el-input-number>
                </template>
            </el-table-column>
        </el-table>
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
        };
    },
    props: {
        belong_order_id: String,
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
        this.$axios.post(this.$remote_rest_url_header + 'my_order/', {
            ssid: this.$cookies.get('ssid'),
            order_id: this.belong_order_id,
        }).then(function (resp) {
            console.log(resp);
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
</style>
