
parseXYZ = function (dataStr) {
    if(dataStr){
        return dataStr
            .split(' ')
            .filter(function(el) {return el;})
            .map(function (x) {return parseFloat(x);});
    }
    return [0,0,0];
};

extractXYZ = function(xmlElement, attributeName){
    var values = parseXYZ(xmlElement.getAttribute(attributeName));
    return {x:values[0], y:values[1], z:values[2]};
};

extractPlacement = function(xmlElement, posAttName, rotAttName){
    return {
        position:extractXYZ(xmlElement, posAttName),
        rotation:extractXYZ(xmlElement, rotAttName)
    };
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
    xmlSections: {},

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
        var scope = this;

        // Extract all <section ...> Each section is usually means one detector description
        this.HDDS.querySelectorAll('section').forEach(function (xmlSection) {
            scope.xmlSections[xmlSection.getAttribute('name')] = xmlSection;
        });

        var xmlTofSection = this.xmlSections['ForwardTOF'];
        var xmlTofPosRot = xmlTofSection.querySelectorAll('composition[name="ForwardTOF"] > posXYZ')[0];
        var tofPlacement = extractPlacement(xmlTofPosRot, 'X_Y_Z', 'rot');

        var xmlTofGlobal = this.HDDS.querySelector('composition[name="forwardPackage"] > posXYZ[volume="ForwardTOF"]');
        var tofGlobalPlacement = extractPlacement(xmlTofGlobal, 'X_Y_Z', 'rot');

        tofPlacement = this.sumPlacements(tofGlobalPlacement, tofPlacement);
        // FTOF
        var ftofGeom = this.boxFromXml(xmlTofSection, "FTOF");
        var ftofMaterial = new THREE.MeshLambertMaterial({ color: 0xa3bad2, transparent: true, opacity: 0.5, side: THREE.DoubleSide });
        var ftofMesh = new THREE.Mesh(ftofGeom, ftofMaterial);
        this.setMeshPlacement(ftofMesh, tofPlacement);
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
        cdcGeom.matrix.setPosition();

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

    boxFromXml:function(xmlElement, boxName){
        var xmlBox = xmlElement.querySelectorAll('box[name="'+boxName+'"]')[0];
        var params = parseXYZ(xmlBox.getAttribute('X_Y_Z'));
        return new THREE.BoxBufferGeometry(params[0], params[1], params[2]);
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
    sumPlacements: function (left, right, zShift) {
        zShift = typeof zShift !== 'undefined' ? zShift : 0;

        // BCAL position as a superposition of BCAL and LASS objects
        var position = {
            x: left.position.x + right.position.x,
            y: left.position.y + right.position.y,
            z: left.position.z + right.position.z + zShift
        };

        var rotation = {
            x: left.rotation.x + right.rotation.x,
            y: left.rotation.y + right.rotation.y,
            z: left.rotation.z + right.rotation.z
        };

        return {position:position, rotation:rotation};
    },

    setMeshPlacement: function (mesh, placement, convToRad) {
        return this.setMeshPositionRotation(mesh, placement.position, placement.rotation, convToRad);
    },

    /**
     *
     * @param mesh
     * @type mesh THREE.Mesh
     * @param rotPos Something with one or both 'rotation', 'position' properties that are index iterable
     * @param convToRad - if true rotation values will be Math.PI/180
     */
    setMeshPositionRotation: function (mesh, position, rotation, convToRad) {

        // Convert to radians set Math.PI/180 OR 1
        convToRad = typeof convToRad !== 'undefined' ? convToRad : true;
        var toRad = convToRad? this.toRad : 1;

        mesh.position.set(position.x, position.y, position.z);
        mesh.rotation.set(rotation.x*toRad, rotation.y*toRad, rotation.z*toRad);


        return mesh;
    },

};
