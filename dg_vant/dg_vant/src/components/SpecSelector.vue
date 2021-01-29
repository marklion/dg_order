<template>
<div class="spec_selector_show">
    <van-popup v-model="real_select_show" round position="bottom" get-container="body">
        <van-picker show-toolbar :columns="specs_from_server" @confirm="change_item">
            <template #cancel>
                <van-button native-type="button" @click="show_customerize = true" type="primary" size="small">自定义</van-button>
            </template>
        </van-picker>
    </van-popup>
    <van-dialog v-model="show_customerize" title="自定义规格" :closeOnClickOverlay="true" @confirm="add_item" get-container="body">
        <van-field v-model="fin_item" label="规格" placeholder="例如：200ml，小瓶或小份等" />
    </van-dialog>
</div>
</template>

<script>
import {
    Base64
} from 'js-base64'
export default {
    name: 'SpecsSelector',
    model: {
        prop: 'select_show',
        event: 'update',
    },
    props: {
        select_item: String,
        good_name: String,
        select_show: Boolean,
    },
    data: function () {
        return {
            fin_item: '',
            specs_from_server: ['默认规格'],
            show_customerize: false,
            mount_point: function () {
                return document.querySelector(".mount_po");
            },
            real_select_show: this.select_show,
        }
    },
    watch: {
        good_name(val) {
            var vue_this = this;
            Base64.extendString();
            console.log(val.toBase64(true));
            vue_this.specs_from_server = ['默认规格'];
            this.$axios.get(this.$remote_rest_url_header + 'specs_by_name/' + val.toBase64(true)).then(function (resp) {
                resp.data.result.forEach((element, index) => {
                    var my_spec = element.fromBase64();
                    if (my_spec != '') {
                        vue_this.$set(vue_this.specs_from_server, index + 1, my_spec);
                    }
                });
                console.log(vue_this);
            }).catch(function (err) {
                console.log(err);
            });
        },
        select_show(val) {
            this.real_select_show = val;
        },
        real_select_show(val) {
            this.$emit('update', val);
        }
    },
    methods: {
        change_item: function (val) {
            this.fin_item = val;
            this.$emit('spec_confirm', this.fin_item);
            this.real_select_show = false;
        },
        add_item: function () {
            this.specs_from_server.push(this.fin_item);
            console.log(this);
            this.change_item(this.fin_item);
        },
    },
    beforeMount: function () {
        var vue_this = this;
        Base64.extendString();
        this.$axios.get(this.$remote_rest_url_header + 'specs_by_name/' + this.good_name.toBase64(true)).then(function (resp) {
            resp.data.result.forEach((element, index) => {
                var my_spec = element.fromBase64();
                if (my_spec != '') {
                    vue_this.$set(vue_this.specs_from_server, index + 1, my_spec);
                }
            });
            console.log(vue_this);
        }).catch(function (err) {
            console.log(err);
        });
    },
}
</script>

<style scoped>
.specs_selector_show {
    width: 100%;
}
</style>
