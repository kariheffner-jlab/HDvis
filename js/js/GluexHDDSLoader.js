
parseXYZ = function (dataStr) {
    return dataStr
        .split(' ')
        .filter(function(el) {return el;})
        .map(function (x) {return parseFloat(x);});
};

THREE.GluexHDDSLoader = function () {
    this.importedGeometry = null;
    this.HDDS = null;

};

THREE.GluexHDDSLoader.prototype = {

    constructor: THREE.GluexHDDSLoader,



    group: new THREE.Group(),
    toRad: Math.PI/180.0,
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
            onLoad(scope.parse(text));
        }, onProgress, onError)
    },

    /** Function that works with loaded json */
    parse: function (text) {

        this.HDDS = new DOMParser().parseFromString( text, 'text/xml' );
        //var elements = this.HDDS.querySelectorAll('composition[name=');
        //var xmlTofBox = this.HDDS.querySelectorAll('section[name="ForwardTOF"] > box[name="FTOF"]')[0];

        var xmlTofSection = this.HDDS.querySelectorAll('section[name="ForwardTOF"]')[0];
        var xmlTofBox = xmlTofSection.querySelectorAll('box[name="FTOF"]')[0];
        var xmlTofPosRot = xmlTofSection.querySelectorAll('composition[name="ForwardTOF"] > posXYZ')[0];
        var tofPosRot = {
            position: parseXYZ(xmlTofPosRot.getAttribute('X_Y_Z')),
            rotation: parseXYZ(xmlTofPosRot.getAttribute('rot'))
        };

        var xmlTofGlobal = this.HDDS.querySelector('composition[name="forwardPackage"] > posXYZ[volume="ForwardTOF"]');
        var tofGlobalPosRot = {
            position: parseXYZ(xmlTofGlobal.getAttribute('X_Y_Z')),
            rotation: parseXYZ(xmlTofGlobal.getAttribute('rot'))
        };

        tofPosRot = this.sumPositionRotation(tofGlobalPosRot, tofPosRot);

        // FTOF
        var tofBoxParams = parseXYZ(xmlTofBox.getAttribute('X_Y_Z'));

        var ftofGeom = new THREE.BoxGeometry(tofBoxParams[0], tofBoxParams[1], tofBoxParams[2]);
        var ftofMaterial = new THREE.MeshLambertMaterial({ color: 0xa3bad2, transparent: true, opacity: 0.5, side: THREE.DoubleSide });
        var ftofMesh = new THREE.Mesh(ftofGeom, ftofMaterial);
        ftofMesh.position.set(tofPosRot.position.x, tofPosRot.position.y, tofPosRot.position.z);
        ftofMesh.rotation.set(tofPosRot.rotation.x, tofPosRot.rotation.y, tofPosRot.rotation.z);
        ftofMesh.name = "FTOF";
        this.group.add(ftofMesh);

        return this.group;
    },


    /** Function that works with loaded json */
    process: function (text) {
        var janaGeo = JSON.parse(text);
        this.importedGeometry = janaGeo;

        var toRad = Math.PI/180.0;
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
        shape = ftofJson.shape;
        var ftofGeom = new THREE.BoxGeometry(shape.x, shape.y, shape.z);
        var ftofMaterial = new THREE.MeshLambertMaterial({ color: 0xa3bad2, transparent: true, opacity: 0.5, side: THREE.DoubleSide });
        var ftofMesh = new THREE.Mesh(ftofGeom, ftofMaterial);
        ftofMesh.position.set(ftofJson.position[0], ftofJson.position[1], ftofJson.position[2]);
        ftofMesh.rotation.set(ftofJson.rotation[0]*toRad, ftofJson.rotation[1]*toRad, ftofJson.rotation[2]*toRad);
        ftofMesh.name = "FTOF";
        this.group.add(ftofMesh);

        // BCAL
        var lassJson = janaGeo['/SITE/HALL/LASS'];

        var bcalJson = janaGeo['/SITE/HALL/LASS/BCAL'];
        var bcalGeom = this.tubeGeometryHelper(bcalJson.shape);
        var bcalMaterial = new THREE.MeshLambertMaterial({ color: 0xa3bad2, transparent: true, opacity: 0.5, side: THREE.DoubleSide });
        var bcalMesh = new THREE.Mesh(bcalGeom, bcalMaterial);

        // BCAL position as a superposition of BCAL and LASS objects
        var pos = {
            x: bcalJson.position[0] + lassJson.position[0],
            y: bcalJson.position[1] + lassJson.position[1],
            z: bcalJson.position[2] + lassJson.position[2]
        };

        // BCAL rotation as a superposition of BCAL and LASS objects
        var rot = {
            x: (bcalJson.rotation[0] + lassJson.rotation[0])*toRad,
            y: (bcalJson.rotation[1] + lassJson.rotation[1])*toRad,
            z: (bcalJson.rotation[2] + lassJson.rotation[2])*toRad
        };

        //bcalMesh.position.set(bcalJson.position[0], bcalJson.position[1], bcalJson.position[2]);
        bcalMesh.position.set(pos.x, pos.y, pos.z);
        bcalMesh.rotation.set(rot.x, rot.y, rot.z);
        bcalMesh.name = "BCAL";
        this.group.add(bcalMesh);

        // CDC
        var cdcJson = janaGeo['/SITE/HALL/LASS/CDC'];
        shape = cdcJson.shape;
        var cdcTubes = {
            0: this.tubeFromPolyConeShape(shape, 0, 1),
            1: this.tubeFromPolyConeShape(shape, 2, 3),
            3: this.tubeFromPolyConeShape(shape, 4, 5)
        };

        for(var i=0; i<cdcTubes.length; i++) {
            cdcTubes[i].updateMatrix();
        }
        var cdcGeom = new THREE.Geometry();
        cdcGeom.matrix.setPosition()

        var cdcTube2 = this.tubeFromPolyConeShape(shape, 0, 1);
        var cdcGeom = new THREE.BoxGeometry(shape.x, shape.y, shape.z);
        var cdcMaterial = new THREE.MeshLambertMaterial({ color: 0xa3bad2, transparent: true, opacity: 0.5, side: THREE.DoubleSide });
        var cdcMesh = new THREE.Mesh(cdcGeom, cdcMaterial);
        cdcMesh.position.set(cdcJson.position[0], cdcJson.position[1], cdcJson.position[2]);
        cdcMesh.rotation.set(cdcJson.rotation[0]*toRad, cdcJson.rotation[1]*toRad, cdcJson.rotation[2]*toRad);
        cdcMesh.name = "FTOF";
        this.group.add(cdcMesh);

        // Global shift due to... global geometry shift
        this.group.position.set(-150.501, 349.986, 500);

        return this.group;
    },

    tubeFromPolyConeShape: function(shape, startZPlaneIndex, endZPlaneIndex)
    {
        // Note: need to handle units
        var aunit = shape['aunit'];
        var lunit = shape['lunit'];

        name = shape['name'];
        //console.log(type, name];

        var rmin = shape.zplanes[startZPlaneIndex]['rmin'];
        var rmax = shape.zplanes[startZPlaneIndex]['rmax'];
        var z = Math.abs(shape.zplanes[endZPlaneIndex].z - shape.zplanes[startZPlaneIndex].z);

        var startphi = shape['startphi'];
        var deltaphi = shape['deltaphi'];

        if ( aunit === 'deg' ) {
            startphi *= Math.PI/180.0;
            deltaphi *= Math.PI/180.0;
        }
        return this.tubeGeometry(rmin, rmax, z, startphi, deltaphi);

    },

    tubeGeometry: function(rmin, rmax, z, startphi, deltaphi)
    {
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
        return this.tubeGeometry(rmin, rmax, z, startphi, deltaphi);
    },


    /**
     * Summs position and rotation vectors. And adds z shift to position.z
     *
     * @param left left object with .rotation and .position vectors
     * @param right
     * @param zShift will be added to final pos.z
     * @param convertToRad if true rotation will be * Math.PI/180
     * @return {{position: {x: number, y: number, z: number}, rotation: {x: number, y: number, z: number}}}
     */
    sumPositionRotation: function (left, right, zShift, convertToRad) {
        zShift = typeof zShift !== 'undefined' ? zShift : 0;
        convertToRad = typeof convertToRad !== 'undefined' ? convertToRad : false;

        // BCAL position as a superposition of BCAL and LASS objects
        var position = {
            x: left.position[0] + right.position[0],
            y: left.position[1] + right.position[1],
            z: left.position[2] + right.position[2] + zShift
        };

        // BCAL rotation as a superposition of BCAL and LASS objects
        var toRad = convertToRad ? this.toRad: 1;

        // BCAL rotation as a superposition of BCAL and LASS objects
        var rotation = {
            x: (left.rotation[0] + right.rotation[0])*toRad,
            y: (left.rotation[1] + right.rotation[1])*toRad,
            z: (left.rotation[2] + right.rotation[2])*toRad
        };

        return {position:position, rotation:rotation};
    },
};
