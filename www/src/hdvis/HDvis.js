import {
  WebGLRenderer,
  PerspectiveCamera,
  Scene,
  MeshPhongMaterial,
  BoxGeometry,
  Mesh,
  AmbientLight,
  DirectionalLight } from 'three'
export default class HDvis {
  constructor () {
    this._rootElementName = ''
    this._isInitialized = false
  }

  /** Thrue if HDvis is initialized on some element */
  get isInitialized () { return this._isInitialized }

  static speak () {
    window.alert('HDvis!@!!')
  }

  /** Initizlizes an HDvis on some DOM element with name rootElementName */
  initialize (rootElementName) {
    this._rootElementName = rootElementName
    this._isInitialized = true

    // Renderer.
    let renderer = new WebGLRenderer()

    // renderer.setPixelRatio(window.devicePixelRatio);
    // renderer.setSize(window.innerWidth, window.innerHeight);
    // Add renderer to page
    // document.body.appendChild(renderer.domElement);

    // get container to contain three.js canvas.
    let container = document.getElementById(this._rootElementName)
    let w = container.clientWidth
    let h = container.clientWidth
    renderer.setSize(w, h)
    container.appendChild(renderer.domElement)

    // Create camera.
    // camera = new THREE.PerspectiveCamera(70, window.innerWidth / window.innerHeight, 1, 1000);
    let camera = new PerspectiveCamera(70, w / h, 1, 1000)
    camera.position.z = 400

    // Create scene.
    let scene = new Scene()

    // Create material
    let material = new MeshPhongMaterial()

    // Create cube and add to scene.
    let geometry = new BoxGeometry(200, 200, 200)
    let mesh = new Mesh(geometry, material)
    scene.add(mesh)

    // Create ambient light and add to scene.
    let light = new AmbientLight(0x404040) // soft white light
    scene.add(light)

    // Create directional light and add to scene.
    let directionalLight = new DirectionalLight(0xffffff)
    directionalLight.position.set(1, 1, 1).normalize()
    scene.add(directionalLight)

    this.scene = scene

    // Add listener for window resize.
    // window.addEventListener('resize', onWindowResize, false);

    // Add stats to page.
    // stats = new Stats()
    // document.body.appendChild(stats.dom)
    this.renderer = renderer
    this.mesh = mesh
    this.camera = camera
  }

  animate () {
    window.requestAnimationFrame(() => this.animate())
    this.mesh.rotation.x += 0.005
    this.mesh.rotation.y += 0.01
    this.renderer.render(this.scene, this.camera)
    // stats.update()
  }
}
