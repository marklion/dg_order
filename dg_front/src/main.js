import Vue from 'vue'
import './plugins/axios'
import App from './App.vue'
import './plugins/element.js'
import router from './router'
import cookies from 'vue-cookies'

Vue.prototype.$cookies = cookies;
Vue.prototype.$remote_url = 'http://www.d8sis.cn';
Vue.prototype.$remote_rest_url_header = Vue.prototype.$remote_url + '/dg_rest/';
Vue.config.productionTip = false

new Vue({
  router,
  render: h => h(App)
}).$mount('#app')
