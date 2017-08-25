<template>
  <div id="app">
    <nav id="menu">
      <header>
        <h2>Menu</h2>
        <router-link to="/">Home</router-link>
        <router-link to="/hello">Hello</router-link>
        <router-link to="/viewport">ViewPort</router-link>
      </header>
    </nav>

    <main id="panel">
      <header>
        <h2>Panel</h2>
        <button v-on:click="greet">Greet</button>
      </header>
      <router-view></router-view>
    </main>

  </div>
</template>

<script>


  export default {
    name: 'app',
    methods: {
      greet: function (event) {
        this.$slideout.toggle()
      }
    },
    mounted: function () {
      window.alert('App mounted')
      this.$slideout = new Slideout({
        'panel': document.getElementById('panel'),
        'menu': document.getElementById('menu'),
        'padding': 256,
        'tolerance': 70
      })
    }
  }
</script>

<style>

  body {
    width: 100%;
    height: 100%;
  }

  .slideout-menu {
    position: fixed;
    top: 0;
    bottom: 0;
    width: 256px;
    min-height: 100vh;
    overflow-y: scroll;
    -webkit-overflow-scrolling: touch;
    z-index: 0;
    display: none;
  }

  .slideout-menu-left {
    left: 0;
  }

  .slideout-menu-right {
    right: 0;
  }

  .slideout-panel {
    position: relative;
    z-index: 1;
    will-change: transform;
    background-color: #FFF; /* A background-color is required */
    min-height: 100vh;
  }

  .slideout-open,
  .slideout-open body,
  .slideout-open .slideout-panel {
    overflow: hidden;
  }

  .slideout-open .slideout-menu {
    display: block;
  }


  #app {
    font-family: 'Avenir', Helvetica, Arial, sans-serif;
    -webkit-font-smoothing: antialiased;
    -moz-osx-font-smoothing: grayscale;
    text-align: center;
    color: #2c3e50;
    margin-top: 60px;
  }
</style>
