// The Vue build version to load with the `import` command
// (runtime-only or standalone) has been set in webpack.base.conf with an alias.
import Vue from 'vue'
import App from './App'
import router from './router'
import HDvis from '@/hdvis/HDvis'

Vue.config.productionTip = false

// This is a global mixin, it is applied to every vue instance
Vue.prototype.$hdvis = new HDvis()

/* eslint-disable no-new */
new Vue({
  el: '#app',
  router,
  template: '<App/>',
  components: { App }
})
