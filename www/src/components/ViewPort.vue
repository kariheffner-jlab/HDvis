<template>
  <div class="ViewPort">

    <div id="threeViewPort">

    </div>
  </div>
</template>


<script>
  export default {
    name: 'index',
    data () {
      return {
        hdvis: null
      }
    },
    mounted: function () {
      window.alert('ViewPort mount')
      if (this.$hdvis) {
        window.alert('this.$hdvis !== null && defined')
      }

      if (!this.$hdvis.isInitialized) {
        window.alert('HDvis is not initialized')
        // this.hdvis.speak()
        this.$hdvis.initialize('threeViewPort')
        this.$hdvis.animate()
      } else {
        window.alert('HDvis OK')
      }
    }
  }
</script>

<!-- Add "scoped" attribute to limit CSS to this component only -->
<style scoped>
  #threeViewPort {
    border: solid aqua;
    width: 300px;
    height: 300px;
    margin: auto;

  }
</style>
