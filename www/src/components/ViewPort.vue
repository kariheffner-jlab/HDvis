<template>
  <div class="ViewPort">

    <div id="threeViewPort">
    </div>

    <div id="eventToolTip" v-bind:style="{ left: toolTip.x + 'px', top: toolTip.y + 'px' }">
      <div class="header">Mighty tooltip</div>
      <table>
        <tr><td>charge  :</td><td>+</td></tr>
        <tr><td>hypoth. :</td><td>pi</td></tr>
        <tr><td>moment  :</td><td>6.54</td></tr>
        <tr><td>chi2:</td><td>25.5</td></tr>
      </table>
    </div>
  </div>
</template>


<script>
  export default {
    name: 'index',
    data () {
      return {
        toolTip: {
          x: 400,
          y: 100
        }
      }
    },

    created: function () {
      this.moveToolTip()
      this.timer = setInterval(this.moveToolTip, 1000)
    },

    mounted: function () {
      // window.alert('ViewPort mount')
      if (this.$hdvis) {
        // window.alert('this.$hdvis !== null && defined')
      }

      if (!this.$hdvis.isInitialized) {
        // window.alert('HDvis is not initialized')
        // this.hdvis.speak()
        this.$hdvis.initialize('threeViewPort')
        this.$hdvis.animate()
      } else {
        // window.alert('HDvis OK')
      }
    },

    methods: {
      moveToolTip: function () {
        if (typeof this.toolTip !== 'undefined') {
          this.toolTip.x += 30
          if (this.toolTip.x > 960) {
            this.toolTip.x = 600
          }
        }
      }
    }
  }
</script>

<!-- Add "scoped" attribute to limit CSS to this component only -->
<style scoped>
  #eventToolTip {

    min-width: 100px;
    min-height: 70px;
    position: absolute;
    background-color: lightgray;

    font-family: Consolas, Menlo, Monaco, monospace;
    font-size: 0.6em;
    opacity: 0.8;
    filter:alpha(opacity=80);
    padding: 7px;

  }

  #eventToolTip .header{
    margin-bottom: 0.5em;
    font-weight: bold;
  }
  #eventToolTip td {
    text-align: right;
  }
  #threeViewPort {
    border: solid aqua;
    width: 500px;
    height: 500px;
    margin: auto;
  }

  .ViewPort{
    position: relative;
  }
</style>
