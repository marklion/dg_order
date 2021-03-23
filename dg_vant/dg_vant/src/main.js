import Vue from 'vue'
import './plugins/axios'
import App from './App.vue'
import './plugins/vant.js'
import router from './router'
import less from 'less'
import 'vant/lib/index.less';
import cookies from 'vue-cookies'

Vue.prototype.$cookies = cookies;
Vue.prototype.$remote_url = 'https://www.d8sis.cn';
Vue.prototype.$remote_rest_url_header = Vue.prototype.$remote_url + '/dg_rest/';

Vue.config.productionTip = false
Vue.use(less)

new Vue({
  router,
  render: h => h(App)
}).$mount('#app')
