THREE.GluexGeometryLoader = function () {
    this.importedGeometry = null;

};

THREE.GluexGeometryLoader.prototype = {

    constructor: THREE.GluexGeometryLoader,

    group: new THREE.Group(),
    defines: {},
    geometries: {},
    refs: {},
    meshes: [],

    load: function (url, onLoad, onProgress, onError) {

        this.group.name="GluexGeometry";
        var scope = this;

        var loader = new THREE.XHRLoader();
        loader.setPath(this.path);
        loader.load(url, function (text) {
            onLoad(scope.process(text));
        }, onProgress, onError);
    },

    /** Function that works with loaded json */
    process: function (text) {
        var janaGeo = JSON.parse(text);
        this.importedGeometry = janaGeo;

        var scope = this;
        var fcalGeom = this.createBox(janaGeo.FCAL);
        var fcalMesh = THREE.Mesh(fcalGeom, new THREE.MeshLambertMaterial({ color: 0x00ff00, transparent: true, opacity: 0.5 }));
        fcalMesh.name = "FCAL";
        this.group.add(fcalMesh);

        return this.group;
    },

    /** Function that works with loaded json */
    getAbsolutePosition: function (path) {
        var tokens = path.split('/');
        for(var i = 0; i<tokens.length; i--)
        {
            console.log(tokens[i])
        }

        return this.group;
    },

    createBox:function(jsonGeo)
    {
        var box = THREE.box(jsonGeo.shape.x, jsonGeo.shape.y, jsonGeo.shape.z);
        box.position.set(jsonGeo.position[0], jsonGeo.position[1], jsonGeo.position[2]);
        box.rotation.set(jsonGeo.rotation[0], jsonGeo.rotation[1], jsonGeo.rotation[2]);
        return box;
    }

};
