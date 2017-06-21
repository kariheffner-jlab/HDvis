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

        // FCAL
        var fcalJson = janaGeo['/SITE/HALL/FCAL'];
        var shape = fcalJson.shape;
        var fcalGeom = new THREE.BoxGeometry(shape.x, shape.y, shape.z);
        var fcalMaterial = new THREE.MeshLambertMaterial({ color: 0x436280, transparent: true, opacity: 0.5, side: THREE.DoubleSide });
        var fcalMesh = new THREE.Mesh(fcalGeom, fcalMaterial);
        fcalMesh.position.set(fcalJson.position[0], fcalJson.position[1], fcalJson.position[2]);
        //fcalMesh.rotation.set(fcalJson.rotation[0], fcalJson.rotation[1], fcalJson.rotation[2]);
        fcalMesh.name = "FCAL";
        this.group.add(fcalMesh);

        // FTOF
        var ftofJson = janaGeo['/SITE/HALL/FTOF'];
        var shape = ftofJson.shape;
        var ftofGeom = new THREE.BoxGeometry(shape.x, shape.y, shape.z);
        var ftofMaterial = new THREE.MeshLambertMaterial({ color: 0xa3bad2, transparent: true, opacity: 0.5, side: THREE.DoubleSide });
        var ftofMesh = new THREE.Mesh(ftofGeom, ftofMaterial);
        ftofMesh.position.set(ftofJson.position[0], ftofJson.position[1], ftofJson.position[2]);
        //ftofMesh.rotation.set(ftofJson.rotation[0], ftofJson.rotation[1], ftofJson.rotation[2]);
        ftofMesh.name = "FTOF";
        this.group.add(ftofMesh);

        // BCAL

        var lassJson = janaGeo['/SITE/HALL/LASS/BCAL'];
        var lassGeom = this.tubeGeometryHelper(lassJson.shape);
        var lassMaterial = new THREE.MeshLambertMaterial({ color: 0xa3bad2, transparent: true, opacity: 0.5, side: THREE.DoubleSide });
        var lassMesh = new THREE.Mesh(lassGeom, lassMaterial);
        lassMesh.position.set(lassJson.position[0], lassJson.position[1], lassJson.position[2]);
        lassMesh.rotation.set(Math.PI/2, 0, 0);
        lassMesh.name = "BCAL";
        this.group.add(lassMesh);



        // Global shift due to... global geometry shift
        this.group.position.set(-150.501, 349.986, 500);

        return this.group;
    },

    tubeGeometryHelper: function (solid)
    {
        // Note: need to handle units
        var aunit = solid['aunit'];
        var lunit = solid['lunit'];

        name = solid['name'];
        //console.log(type, name];

        var rmin = solid['rmin'];
        var rmax = solid['rmax'];
        var z = solid['z'];

        var startphi = solid['startphi'];
        var deltaphi = solid['deltaphi'];

        if ( aunit === 'deg' ) {
            startphi *= Math.PI/180.0;
            deltaphi *= Math.PI/180.0;
        }

        var shape = new THREE.Shape();
        // x,y, radius, startAngle, endAngle, clockwise, rotation
        shape.absarc(0, 0, rmax, startphi, deltaphi, false);

        if ( rmin > 0.0 ) {

            var hole = new THREE.Path();
            hole.absarc(0, 0, rmin, startphi, deltaphi, true);
            shape.holes.push(hole);

        }

        var extrudeSettings = {
            amount : z,
            steps : 1,
            bevelEnabled: false,
            curveSegments: 24
        };

        var geometry = new THREE.ExtrudeGeometry(shape, extrudeSettings);
        geometry.center();
        return geometry;


    },
};
