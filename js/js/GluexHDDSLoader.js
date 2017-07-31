
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
    posAttName = typeof posAttName !== 'undefined' ? posAttName : 'X_Y_Z';
    rotAttName = typeof rotAttName !== 'undefined' ? rotAttName : 'rot';
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

        var loader = new THREE.FileLoader();
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

        this.group.add(this.processFTOF());

        this.group.add(this.processFDC());

        var fcalGeo = new THREE.BoxBufferGeometry(236.0, 236.0, 10.0);
        var fcal = new THREE.Mesh(fcalGeo, new THREE.MeshLambertMaterial({ color: 0x436280, transparent: true, opacity: 0.3, side: THREE.DoubleSide }));
        fcal.position.set(0.529, -0.002, 624.906 + 22.5);
        this.group.add(fcal);
        return this.group;
    },

    processBCAL: function() {
        var xmlSection = this.xmlSections['BarrelEMcal'];

        var bcal = new THREE.Group();
        bcal.name = "BCAL";

        // BCAL placement in the global main file
        var xmlBcalGlobal = this.HDDS.querySelector('composition[name="barrelPackage"] > posXYZ[volume="ForwardTOF"]');
        var globalPlacement = extractPlacement(xmlBcalGlobal);
        this.setMeshPlacement(bcal, globalPlacement);

        return ftof;
    },

    processFDC: function () {
        var xmlSection = this.xmlSections['ForwardDC'];
        var fdc = new THREE.Group();
        fdc.name = "FDC";

        "58.05 64.0485 -99.0"
        "58.05 64.0485 -97.5"
        "0.0 64.0485 -97.5"
        "0.0 64.0485 99.0"

    },


    processFTOF: function () {
        var xmlSection = this.xmlSections['ForwardTOF'];

        var ftof = new THREE.Group();
        ftof.name = "FTOF";
        var plane0 = this.buildTofPlane(xmlSection, 0);
        var plane1 = this.buildTofPlane(xmlSection, 1);
        ftof.add(plane0);
        ftof.add(plane1);

        // Tof placement in the global main file
        var xmlTofGlobal = this.HDDS.querySelector('composition[name="forwardPackage"] > posXYZ[volume="ForwardTOF"]');
        var globalPlacement = extractPlacement(xmlTofGlobal);
        this.setMeshPlacement(ftof, globalPlacement);

        return ftof;
    },

    buildTofPlane: function (xmlSection, planeIndex) {
        var xmlPlacement = xmlSection.querySelectorAll('composition[name="ForwardTOF"] > posXYZ')[planeIndex];
        var placement = extractPlacement(xmlPlacement);

        // Plane
        var geometry = this.boxFromXml(xmlSection, "FTOF");
        var material = new THREE.MeshLambertMaterial({ color: 0xa3bad2, transparent: true, opacity: 0.5, side: THREE.DoubleSide, visible:false });
        var plane = new THREE.Mesh(geometry, material);
        this.setMeshPlacement(plane, placement);
        plane.name = "TOF_plane" + planeIndex;

        //  PUT segments and modules
        var ftob = this.buildTofRegion(xmlSection, 'FTOB', planeIndex, 0, false);
        var ftoz = this.buildTofRegion(xmlSection, 'FTOZ', planeIndex, 19, false);
        var ftos = this.buildTofRegion(xmlSection, 'FTOS', planeIndex, 21, false);
        var fton = this.buildTofRegion(xmlSection, 'FTON', planeIndex, 21, true);
        var ftoy = this.buildTofRegion(xmlSection, 'FTOY', planeIndex, 23, false);
        var ftot = this.buildTofRegion(xmlSection, 'FTOT', planeIndex, 25, false);
        plane.add(ftob);
        plane.add(ftoz);
        plane.add(ftos);
        plane.add(fton);
        plane.add(ftoy);
        plane.add(ftot);

        return plane;
    },

    buildTofRegion: function (tofXmlSection, name, planeNum, startIndex, isRight) {

        var xmlComposition = tofXmlSection.querySelector('composition[envelope="'+name+'"]');

        // We will need this to extract region position in the future
        // because we have region positions in xml like:
        //    <composition name="forwardTOF" envelope="FTOF">
        //         <posXYZ volume="forwardTOF_bottom1" X_Y_Z="0.0 -70.125 0.0" />
        //         ....
        // so we need this forwardTOF_bottom1 to extract its placement
        var regionFullName = xmlComposition.getAttribute('name');

        var xmlMultPosY = xmlComposition.querySelector('mposY');
        // the xml looks like this:
        // <mposY volume="FTOH" ncopy="2" Z_X = "0.0 0.0" Y0="-3.045" dY="6.09">
        var volumeName = xmlMultPosY.getAttribute('volume');
        var ncopy = parseInt(xmlMultPosY.getAttribute('ncopy'));
        var y0 = parseFloat(xmlMultPosY.getAttribute('Y0'));
        var dy = parseFloat(xmlMultPosY.getAttribute('dY'));

        // Region bounding box
        var region = new THREE.Mesh(
            this.boxFromXml(tofXmlSection, name),
            new THREE.MeshLambertMaterial({visible:false}));
        region.name='TOF' + '_plane' + planeNum + '_' + name ;

        // Module box that we will copy
        // Modules should NOT be of buffered geometry, because they get colored by faces
        var moduleBox = this.boxFromXml(tofXmlSection, volumeName, false);

        // Go through repetitions and create modules
        for(var i=0; i< ncopy; i++){
            var material = new THREE.MeshBasicMaterial({ transparent: true, opacity: 0.5, vertexColors: THREE.VertexColors});
            var module = new THREE.Mesh(moduleBox.clone(), material);

            module.name = "TOFBar_p" + planeNum + "_m" + (startIndex+i);
            if(isRight) module.name += "_r";

            module.position.set(0, y0 + i*dy, 0);
            module.userData={end0h:0, end1h:0};
            region.add(module);
        }

        // get placement
        var selector = 'posXYZ[volume="'+regionFullName+'"]';
        var xmlPlacement = tofXmlSection.querySelector(selector);
        var regionPlacement = extractPlacement(xmlPlacement);
        this.setMeshPlacement(region, regionPlacement);

        return region;
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

    boxFromXml:function(xmlElement, boxName, isBuffered){
        isBuffered = (typeof isBuffered !== 'undefined') ?  isBuffered : true;

        var xmlBox = xmlElement.querySelectorAll('box[name="'+boxName+'"]')[0];
        var params = parseXYZ(xmlBox.getAttribute('X_Y_Z'));
        if(isBuffered) return new THREE.BoxBufferGeometry(params[0], params[1], params[2]);
        return new THREE.BoxGeometry(params[0], params[1], params[2]);
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
