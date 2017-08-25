import './assets/vuetify.min.css'

import Vue from 'vue'
import App from './App'
import router from './router'
import HDvis from '@/hdvis/HDvis'
import Vuetify from 'vuetify'

Vue.use(Vuetify)

Vue.config.productionTip = false

// This is a global mixin, it is applied to every vue instance
Vue.prototype.$hdvis = new HDvis()
Vue.prototype.$slideout = null
// Vue.prototype.$slideout
Vue.config.silent = false

/* eslint-disable no-new */
new Vue({
  el: '#app',
  router,
  template: '<App/>',
  components: { App }
})
