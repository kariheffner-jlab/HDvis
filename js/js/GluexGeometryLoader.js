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
    tofDefaultColor: 0xa3bad2,

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

        var toRad = Math.PI/180.0;

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
        this.processFTOF();

        // LASS adds positions and rotation shifts
        var lassJson = janaGeo['/SITE/HALL/LASS'];

        // BCAL
        var bcalJson = janaGeo['/SITE/HALL/LASS/BCAL'];
        var bcalGeom = this.tubeGeometryFromJson(bcalJson.shape);
        var bcalMaterial = new THREE.MeshLambertMaterial({ color: 0xa3bad2, transparent: true, opacity: 0.5, side: THREE.DoubleSide });
        var bcalMesh = new THREE.Mesh(bcalGeom, bcalMaterial);

        var bcalOrient = this.sumPositionRotation(lassJson, bcalJson, 0, toRad);
        bcalMesh.position.set(bcalOrient.position.x, bcalOrient.position.y, bcalOrient.position.z);
        bcalMesh.rotation.set(bcalOrient.rotation.x, bcalOrient.rotation.y, bcalOrient.rotation.z);
        bcalMesh.name = "BCAL";
        this.group.add(bcalMesh);

        // CDC
        var cdcJson = janaGeo['/SITE/HALL/LASS/CDC'];
        var cdcShape = cdcJson.shape;
        var zShift = 0;
        if (cdcShape.zplanes){
            zShift = -cdcShape.zplanes[0].z;
        }
        else {
            console.warn("'/SITE/HALL/LASS/CDC' !shape.zplanes");
        }

        var cdcGeom = this.polyShapeGeometryFromJson(cdcShape);
        var cdcMaterial = new THREE.MeshLambertMaterial({ color: 0xa3bad2, transparent: true, opacity: 0.5, side: THREE.DoubleSide });
        var cdcMesh = new THREE.Mesh(cdcGeom, cdcMaterial);
        var cdcOrient = this.sumPositionRotation(lassJson, cdcJson, zShift, toRad);
        cdcMesh.position.set(cdcOrient.position.x, cdcOrient.position.y, cdcOrient.position.z);
        cdcMesh.rotation.set(cdcOrient.rotation.x, cdcOrient.rotation.y, cdcOrient.rotation.z);
        cdcMesh.name = "CDC";
        this.group.add(cdcMesh);

        // Global shift due to... global geometry shift
        this.group.position.set(-150.501, 349.986, 500);

        return this.group;
    },



    processFTOF: function () {
        // FTOF
        var toRad = Math.PI/180.0;
        var ftofJson = this.importedGeometry['/SITE/HALL/FTOF'];
        shape = ftofJson.shape;
        var ftofGeom = new THREE.BoxGeometry(shape.x, shape.y, shape.z);
        var ftofMaterial = new THREE.MeshLambertMaterial({
            color: 0xa3bad2, transparent: true,
            opacity: 0.5, visible:false });
        var ftofMesh = new THREE.Mesh(ftofGeom, ftofMaterial);
        ftofMesh.position.set(ftofJson.position[0], ftofJson.position[1], ftofJson.position[2]);
        ftofMesh.rotation.set(ftofJson.rotation[0]*toRad, ftofJson.rotation[1]*toRad, ftofJson.rotation[2]*toRad);
        ftofMesh.name = "FTOF";



        // SEGMENT B
        var ftob = this.buildTofSection('/SITE/HALL/FTOF/FTOB', 'FTOB', 0, 0, false);
        var ftoz = this.buildTofSection('/SITE/HALL/FTOF/FTOZ', 'FTOZ', 0, 19, false);
        var ftos = this.buildTofSection('/SITE/HALL/FTOF/FTOS', 'FTOS', 0, 21, false);
        var fton = this.buildTofSection('/SITE/HALL/FTOF/FTON', 'FTON', 0, 21, true);
        var ftoy = this.buildTofSection('/SITE/HALL/FTOF/FTOY', 'FTOY', 0, 23, false);
        var ftot = this.buildTofSection('/SITE/HALL/FTOF/FTOT', 'FTOT', 0, 25, false);
        ftofMesh.add(ftob);
        //ftofMesh.add(ftoz);
        //ftofMesh.add(ftos);
        //ftofMesh.add(fton);
        //ftofMesh.add(ftoy);
        ftofMesh.add(ftot);

        this.group.add(ftofMesh);

    },

    buildTofSection: function (path, sectionName, planeNum, startIndex, isRight) {
        var scope = this;

        var sectionJson = this.importedGeometry[path];
        var shape = sectionJson.shape;
        var section = new THREE.Mesh(
            new THREE.BoxGeometry(shape.x, shape.y, shape.z),
            new THREE.MeshLambertMaterial({visible:false}));
        section.name=sectionName;

        // That is ROOT magic... We have to add position of the subnode to the section
        var divisionJson = this.importedGeometry[path + '/' + sectionJson.division.volumeref];
        var posRot = this.sumPositionRotation(sectionJson, divisionJson, 0, 1);


        // Repetitions number is defined in .division.number
        // Width of one module is boundingBox.y/repetitions number
        //var moduleY = divisionJson.shape.y;
        var moduleY = sectionJson.division.width;

        // Go through repetitions and create modules
        for(var i=0; i< sectionJson.division.number; i++){

            var moduleGeo = new THREE.BoxGeometry(shape.x, moduleY, shape.z);

            var material = new THREE.MeshBasicMaterial({
                color: scope.tofDefaultColor,
                transparent: true,
                opacity: 0.5,
                vertexColors: THREE.VertexColors});
            var module = new THREE.Mesh(moduleGeo, material);

            module.position.set(0, 0 + i*moduleY, 0);


            module.name = "TOFBar_" + planeNum + "_" + (startIndex+i);
            if(isRight) module.name += "_r";
            section.add(module);
        }

        section.position.set(sectionJson.position[0],sectionJson.position[1],sectionJson.position[2]);

        return section;
    },

    /**
     * Summs position and rotation vectors. And adds z shift to position.z
     *
     * @param left left object with .rotation and .position vectors
     * @param right
     * @param zShift will be added to final pos.z
     * @return {position, rotation}
     */
    sumPositionRotation: function (left, right, zShift, toRad) {

        // BCAL position as a superposition of BCAL and LASS objects
        var position = {
            x: left.position[0] + right.position[0],
            y: left.position[1] + right.position[1],
            z: left.position[2] + right.position[2] + zShift
        };

        // BCAL rotation as a superposition of BCAL and LASS objects
        var rotation = {
            x: (left.rotation[0] + right.rotation[0])*toRad,
            y: (left.rotation[1] + right.rotation[1])*toRad,
            z: (left.rotation[2] + right.rotation[2])*toRad
        };

        return {position:position, rotation:rotation};
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

    tubeGeometryFromJson: function (solid)
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

    polyShapeGeometryFromJson: function (shape) {

        var aunit = shape['aunit'];
        var lunit = shape['lunit'];

        var startphi = shape['startphi'];
        var deltaphi = shape['deltaphi'];

        if (aunit === 'deg') {
            startphi *= Math.PI / 180.0;
            deltaphi *= Math.PI / 180.0;
        }

        var z_zero = 0;
        if (shape.zplanes){
            z_zero = shape.zplanes[0].z;
        }
        else {
            console.warn("polyShapeGeometryFromJson !shape.zplanes");
        }


        var outerPath = [];
        var len = shape.zplanes.length;
        for ( var i = 0; i < len; i ++ ) {
            var z = shape.zplanes[i].z - z_zero;
            var rmax = shape.zplanes[i].rmax;

            // don't add the same points
            if(i>0){
                var lastZ = outerPath[outerPath.length-1].x;
                var lastRmax = outerPath[outerPath.length-1].y;

                if(lastZ===z && lastRmax ===rmax) {
                    continue;
                }
            }

            outerPath.push( new THREE.Vector2( z, rmax ) );
        }

        var geometry = new THREE.LatheBufferGeometry( outerPath, /* segments */ 24, startphi, deltaphi );
        return geometry;
    }
};
