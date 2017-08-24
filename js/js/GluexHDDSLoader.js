
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

extractRioZ = function(xmlElement, attributeName){
    var values = parseXYZ(xmlElement.getAttribute(attributeName));
    return {inner:values[0], outer:values[1], z:values[2]};
};

extractPlacement = function(xmlElement, posAttName, rotAttName){
    posAttName = typeof posAttName !== 'undefined' ? posAttName : 'X_Y_Z';
    rotAttName = typeof rotAttName !== 'undefined' ? rotAttName : 'rot';
    return {
        position:extractXYZ(xmlElement, posAttName),
        rotation:extractXYZ(xmlElement, rotAttName)
    };
};
createPolyPlaneGeometry = function(data, segments, phiStart, phiLength) {

    segments = typeof segments !== 'undefined' ? segments : 20;
    phiStart = typeof phiStart !== 'undefined' ? phiStart : 0;
    phiLength = typeof phiLength !== 'undefined' ? phiLength : 2*Math.PI;

    var points2d = []
    for (var i = 0; i < data.length; i++) {
        points2d.push(new THREE.Vector2(data[i][1], data[i][2]));
    }

    for (var i = data.length -1; i >= 0; i--) {
        points2d.push(new THREE.Vector2(data[i][0], data[i][2]));
    }

    points2d.push(new THREE.Vector2(data[0][1], data[0][2]));   // Closing the contour

    // use the same points to create a LatheGeometry
    return new THREE.LatheGeometry(points2d, segments, phiStart, phiLength);
};

createPolyPlaneGeometry2 = function(data, segments, phiStart, phiLength) {

    segments = typeof segments !== 'undefined' ? segments : 20;
    phiStart = typeof phiStart !== 'undefined' ? phiStart : 0;
    phiLength = typeof phiLength !== 'undefined' ? phiLength : 2*Math.PI;

    var points2d = []
    for (var i = 0; i < data.length; i++) {
        points2d.push(new THREE.Vector2(parseFloat(data[i].attributes[0].value.split(" ")[1]), parseFloat(data[i].attributes[0].value.split(" ")[2])));
    }

    for (var i = data.length -1; i >= 0; i--) {
        points2d.push(new THREE.Vector2(parseFloat(data[i].attributes[0].value.split(" ")[0]), parseFloat(data[i].attributes[0].value.split(" ")[2])));
    }

    points2d.push(new THREE.Vector2(parseFloat(data[0].attributes[0].value.split(" ")[1]), parseFloat(data[0].attributes[0].value.split(" ")[2])));   // Closing the contour

    // use the same points to create a LatheGeometry
    return new THREE.LatheGeometry(points2d, segments, phiStart, phiLength);
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
    bcalLayerRioPhi: [],
    bcalDeltaAngle : 2*Math.PI/(48*4),

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
        this.group.add(this.processCDC());
        this.group.add(this.processFDC());
        this.group.add(this.processFCAL());
        this.group.add(this.processSC());
        this.group.add(this.processBCAL());

        var fcalGeo = new THREE.BoxBufferGeometry(236.0, 236.0, 43.0);//was 45
        var fcal = new THREE.Mesh(fcalGeo, new THREE.MeshLambertMaterial({ color: 0xffffff, transparent: true, opacity: .1, side: THREE.DoubleSide }));
        fcal.position.set(0.529, -0.002, 624.906 + 22.5-1);
        this.group.add(fcal);

        return this.group;
    },

    processBCAL: function() {
        var xmlSection = this.xmlSections['BarrelEMcal'];

        // BCAL mother volume
        var bcalGeo = this.tubeGeometry(64.2485, 90.5185, 390.0);
        var bcalMaterial = new THREE.MeshLambertMaterial({ color: 0xcccccc, transparent: true, opacity: .2, side: THREE.DoubleSide })
        var bcal = new THREE.Mesh(bcalGeo, bcalMaterial);
        bcal.name = "BCAL";
        bcal.renderOrder = 100;     // should be higher than FDC and CDC mother volumes render Order

        let bcalDetails = new THREE.Group();
        bcalDetails.name = "BCAL_details"
        const sectorKeys = ["BM01", "BM02", "BM04", "BMF7", "BMFA"];
        const sectorColors = [0xddd1be, 0xe1ddb5, 0xDED29E, 0xB3A580]
        let BCALLayerDimentions = [];
        for (let sectionName of sectorKeys) {
            BCALLayerDimentions.push(extractRioZ(xmlSection.querySelector(`tubs[name="${sectionName}"]`), "Rio_Z"))
        }

        const deltaAngle = 2*Math.PI/(48*4);

        let bcalDeltaAngle = this.bcalDeltaAngle
        for(let moduleIndex=0; moduleIndex<48; moduleIndex++) {
            // Build section geometries
            for(let sectorIndex = 0; sectorIndex<4; sectorIndex++) {
                for(let layerIndex = 0; layerIndex<4; layerIndex++)
                {
                    this.bcalLayerRioPhi[layerIndex] = {
                        inner:BCALLayerDimentions[layerIndex].inner,
                        outer:BCALLayerDimentions[layerIndex+1].inner,
                        phi: bcalDeltaAngle}
                    let pieceGeo = this.tubeGeometry(
                        BCALLayerDimentions[layerIndex].inner,          // inner radius
                        BCALLayerDimentions[layerIndex+1].inner,        // outer radius (take it from next element)
                        2.0,                                         // z length
                        (moduleIndex*4+sectorIndex)*bcalDeltaAngle,      // angle offset
                        bcalDeltaAngle,                                  // angle
                        true,                                        // buffered geometry
                        false)                                       // do not center the geometry to the... center

                    let sectionMat = new THREE.MeshLambertMaterial(
                        { color: sectorColors[layerIndex], transparent: true, opacity: .2, side: THREE.DoubleSide })
                    let sector = new THREE.Mesh(pieceGeo, sectionMat);
                    sector.name = `BCAL_m${moduleIndex}_l${layerIndex}_s${sectorIndex}`
                    bcalDetails.add(sector)
                }
            }
        }

        bcal.add(bcalDetails)
        bcalDetails.translateZ(195)

        // BCAL hits example
        let bcalHits = new THREE.Group();
        bcalHits.name = "BCAL_hits"

        let moduleIndex = 15;
        let sectorIndex = 2;
        let layerIndex = 1;


        let hitGeo = this.tubeGeometry(
            this.bcalLayerRioPhi[layerIndex].inner,        // inner radius
            this.bcalLayerRioPhi[layerIndex].outer,      // outer radius (take it from next element)
            20.0,                                          // z length
            (moduleIndex*4+sectorIndex)*this.bcalDeltaAngle,   // angle offset
            bcalDeltaAngle,                               // angle
            true,                                         // buffered geometry
            false)                                        // do not center the geometry to the... center

        let hitMat = new THREE.MeshLambertMaterial(
            { color: 0xFF0000, transparent: true, opacity: .2, side: THREE.DoubleSide })
                    let hit = new THREE.Mesh(hitGeo, hitMat);
        hit.name = `BCAL_hit_m${moduleIndex}_l${layerIndex}_s${sectorIndex}`
        bcalHits.add(hit)

        bcal.add(bcalHits)
        bcalHits.translateZ(208)


        // BCAL placement in the global main file
        let xmlBcalGlobal = this.HDDS.querySelector('composition[name="barrelPackage"] > posXYZ[volume="BarrelEMcal"]');
        let globalPlacement = extractPlacement(xmlBcalGlobal);
        let localPlacement = extractPlacement(xmlSection.querySelector('composition[name="BarrelEMcal"] > posXYZ[volume="barrelEMcal"]'))
        this.setMeshPlacement(bcal, this.sumPlacements(globalPlacement, localPlacement));

        return bcal;
    },




    processSC: function() {
        var xmlSection = this.xmlSections['StartCntr'];

        //sector geometry
        var data=xmlSection.querySelector('pgon[name="STRC"]').children;
        var SectorGeometry = createPolyPlaneGeometry2(data,data.length,-6*Math.PI/180.,12*Math.PI/180.)
        SectorGeometry.rotateX(-1*Math.PI/2.);
        SectorGeometry.rotateY(Math.PI)


        var SC = new THREE.Group();
        SC.name = "SC";
        var sectors = this.buildSCSectors(xmlSection, 'STRT', 0, SectorGeometry);
        SC.add(sectors);


        var xmlGlobalPlacement = this.HDDS.querySelector('composition[name="barrelPackage"] > posXYZ[volume="StartCntr"]');
        var globalPlacement = extractPlacement(xmlGlobalPlacement);
        var xmlLocalPlacement = xmlSection.querySelector('composition[name="StartCntr"] > posXYZ[volume="startCntr"]');
        var localPlacement = extractPlacement(xmlLocalPlacement);
        this.setMeshPlacement(SC, this.sumPlacements(localPlacement, globalPlacement));

        return SC;
    },
    buildSCSectors: function (scXmlSection, regionShortName, startIndex, SectorGeometry) {

        var XmlComposition = scXmlSection.querySelector('composition[envelope="'+regionShortName+'"]');

        var region = new THREE.Mesh(
            this.tubeGeometry(0,.001,1,0,2*Math.PI,false),
            new THREE.MeshLambertMaterial({ visible:false ,color: 0x436280, transparent:true, opacity: 0.4, side: THREE.DoubleSide })
        );
        region.name='SC_sectors';

        //make/place the sectors
        var sectors = XmlComposition.querySelector('mposPhi[volume="STRC"]');

        var ncopy = parseInt(sectors.getAttribute('ncopy'));

        var Phi0 = parseFloat(sectors.getAttribute('Phi0'));
        var dPhi = (360. / ncopy) * (Math.PI / 180.);

        //Z ROTATION equal to Phi too
        for(var i=0;i<ncopy;i++) {
            var material = new THREE.MeshBasicMaterial({
                transparent: true,
                opacity: 0.6,
                color: 0xffffff,
                side: THREE.DoubleSide,
                visible: true
            });


            var sector = new THREE.Mesh(SectorGeometry.clone(), material);

            sector.name = "SCsector_" + (i+1).toString();
            sector.rotateZ((Phi0 + (i) * dPhi));
            sector.position.set(0 * Math.cos(Math.PI / 2. - (Phi0 + (i) * dPhi)), 0 * Math.sin(Math.PI / 2. - (Phi0 + (i) * dPhi)), 0.0);

            region.add(sector);
        }

        var selector = `posXYZ[volume="startCntr"]`;
        var xmlPlacement = scXmlSection.querySelector(selector);
        var regionPlacement = extractPlacement(xmlPlacement);
        this.setMeshPlacement(region, regionPlacement);
        return region;
    },

    processCDC: function() {
        var xmlSection = this.xmlSections['CentralDC'];

        //var CDCMotherGeom = this.tubeGeometry(11,60,154.75,0,2*Math.PI);
        // var cdc = new THREE.Mesh(CDCMotherGeom, new THREE.MeshLambertMaterial({ visible:true ,color: 0x436280, transparent:true, opacity: 0.4, side: THREE.DoubleSide }));

        var shortWireGeometry =this.tubeGeometry(0,.075,154.5,0,2*Math.PI,true);
        var longWireGeometry =this.tubeGeometry(0,.075,155.5,0,2*Math.PI,true);
        var cdc = this.buildCDCStraws(xmlSection, 'DCLS', 0, shortWireGeometry, longWireGeometry);
        cdc.renderOrder = 50;

        var xmlGlobalPlacement = this.HDDS.querySelector('composition[name="barrelPackage"] > posXYZ[volume="CentralDC"]');
        var globalPlacement = extractPlacement(xmlGlobalPlacement);
        var xmlLocalPlacement = xmlSection.querySelector('composition[name="CentralDC"] > posXYZ[volume="centralDC"]');
        var localPlacement = extractPlacement(xmlLocalPlacement);
        this.setMeshPlacement(cdc, this.sumPlacements(localPlacement, globalPlacement));

        return cdc;
    },

    buildCDCStraws: function (cdcXmlSection, regionShortName, startIndex, ShortWireGeometry, LongWireGeometry) {

        var XmlComposition = cdcXmlSection.querySelector('composition[envelope="'+regionShortName+'"]');

        var arrayofComponents=XmlComposition.children;

        var region = new THREE.Mesh(
            this.tubeGeometry(11,60,154.75,0,2*Math.PI,true),
            new THREE.MeshLambertMaterial({ visible:true ,color: 0x436280, transparent:true, opacity: 0.4, side: THREE.DoubleSide })
        );
        region.name='CDC';

        for (var i=0;i<arrayofComponents.length;i++) {
            if (i < 3)
                continue;


            var regionFullName = arrayofComponents[i].getAttribute('volume');

            var ncopy = parseInt(arrayofComponents[i].getAttribute('ncopy'));
            var Phi0 = parseFloat(arrayofComponents[i].getAttribute('Phi0'));
            var R = parseFloat(arrayofComponents[i].getAttribute('R_Z').split(" ")[0]);
            var dPhi = (360. / ncopy) * (Math.PI / 180.);

            var ring = i - 2;

            for (var j = 1.0; j <= ncopy; j++)
            {
                if(regionFullName==="CDCstrawShort")
                {

                    var material = new THREE.MeshBasicMaterial({
                        transparent: true,
                        opacity: 0.8,
                        color: 0x3dc67d,
                        side: THREE.DoubleSide,
                        visible:false
                    });


                    var module = new THREE.Mesh(ShortWireGeometry.clone(), material);

                    module.name="CDCstraw_"+ring.toString()+"_"+j.toString();
                    module.position.set(R*Math.cos(Math.PI/2.-(Phi0+(j-1.0)*dPhi)), R*Math.sin(Math.PI/2.-(Phi0+(j-1.0)*dPhi)), 0.0);


                    region.add(module);
                }
                else if(regionFullName==="CDCstrawLong")
                {

                    var materialL = new THREE.MeshBasicMaterial({
                        transparent: true,
                        opacity: 0.8,
                        color: 0x3dc67d,
                        side: THREE.DoubleSide,
                        visible:false
                    });
                    var rotX = parseFloat(arrayofComponents[i].getAttribute('rot').split(" ")[0]);

                    var moduleL = new THREE.Mesh(LongWireGeometry.clone(), materialL);

                    moduleL.name="CDCstraw_"+ring.toString()+"_"+j.toString();
                    moduleL.position.set(R*Math.cos(Math.PI/2.-(Phi0+(j-1.0)*dPhi)), R*Math.sin(Math.PI/2.-(Phi0+(j-1.0)*dPhi)), 0.0);
                    moduleL.rotateX(rotX*Math.PI/180.);

                    region.add(moduleL);
                }
            }
        }

        var selector = `posXYZ[volume="CDClayers"]`;
        var xmlPlacement = cdcXmlSection.querySelector(selector);
        var regionPlacement = extractPlacement(xmlPlacement);
        this.setMeshPlacement(region, regionPlacement);
        return region;
    },

    processFDC: function() {
        var xmlSection = this.xmlSections['ForwardDC'];

        var fdcMotherGeom = new THREE.CylinderBufferGeometry( 64.0485, 64.0485, 188, 32 );

        var fdc = new THREE.Mesh(fdcMotherGeom, new THREE.MeshLambertMaterial({ color: 0x436280, transparent:true, opacity: 0.3, side: THREE.DoubleSide }));
        fdc.renderOrder = 50;

        var xmlGlobalPlacement = this.HDDS.querySelector('composition[name="barrelPackage"] > posXYZ[volume="ForwardDC"]');
        var globalPlacement = extractPlacement(xmlGlobalPlacement);
        var xmlLocalPlacement = xmlSection.querySelector('composition[name="ForwardDC"] > posXYZ[volume="forwardDC"]');
        var localPlacement = extractPlacement(xmlLocalPlacement);
        this.setMeshPlacement(fdc, this.sumPlacements(localPlacement, globalPlacement));
        fdc.rotateX(Math.PI/2.0);
        fdc.name="FDC";

        return fdc;
    },

    processFCAL: function () {
        var xmlSection = this.xmlSections['ForwardEMcal'];

        // We use it as a reference geometry
        var xmlBox = xmlSection.querySelectorAll('box[name="LGBL"]')[0];
        var params = parseXYZ(xmlBox.getAttribute('X_Y_Z'));
        var scale =.04;
        var moduleBoxGeometry = new THREE.BoxBufferGeometry(params[0], params[1], params[2]*scale);

        var fcal = new THREE.Group();
        fcal.name = "FCAL";
        var sectionup = this.buildFCALSection(xmlSection, 'LGDT', 30, false, moduleBoxGeometry,scale);
        var sectiondown = this.buildFCALSection(xmlSection, 'LGDB', 0, false, moduleBoxGeometry,scale);
        var sectionleft = this.buildFCALSection(xmlSection, 'LGDN', 28, false, moduleBoxGeometry,scale);//north
        var sectionright = this.buildFCALSection(xmlSection, 'LGDS', 28, true, moduleBoxGeometry,scale);

        fcal.add(sectionup);
        fcal.add(sectiondown);
        fcal.add(sectionleft);
        fcal.add(sectionright);

        // FCAL section has its own placement adjustment
        var xmlSectionPlacement = xmlSection.querySelector('composition[name="ForwardEMcal"] > posXYZ[volume="forwardEMcal"]');
        var sectionInternalPlacement = extractPlacement(xmlSectionPlacement);

        // FCAL placement in the global main file
        var xmlFCALGlobal = this.HDDS.querySelector('composition[name="forwardPackage"] > posXYZ[volume="ForwardEMcal"]');
        var globalPlacement = extractPlacement(xmlFCALGlobal);

        this.setMeshPlacement(fcal, this.sumPlacements(globalPlacement, sectionInternalPlacement));

        return fcal;
    },

    buildFCALSection: function (fcalXmlSection, regionShortName, startIndex, isRight, moduleBoxGeometry,scale) {

        var yXmlComposition = fcalXmlSection.querySelector('composition[envelope="'+regionShortName+'"]');

        var regionFullName = yXmlComposition.getAttribute('name');

        // We will need this to extract region position in the future
        // because we have region positions in xml like:
        //    <composition name="forwardTOF" envelope="FTOF">
        //         <posXYZ volume="forwardTOF_bottom1" X_Y_Z="0.0 -70.125 0.0" />
        //         ....
        // so we need this forwardTOF_bottom1 to extract its placement
        var yXmlMultPosY = yXmlComposition.querySelector('mposY');
        // the xml looks like this:
        // <mposY volume="FTOH" ncopy="2" Z_X = "0.0 0.0" Y0="-3.045" dY="6.09">
        var yVolumeName = yXmlMultPosY.getAttribute('volume');
        var yCopiesCount = parseInt(yXmlMultPosY.getAttribute('ncopy'));
        var y0 = parseFloat(yXmlMultPosY.getAttribute('Y0'));
        var dy = parseFloat(yXmlMultPosY.getAttribute('dY'));

        var xName = 'LGDfullRow';
        if (regionFullName === 'LGDnorth') {
            xName = 'LGDhalfRowNorth'
        }
        else if (regionFullName === 'LGDsouth' ) {
            xName = 'LGDhalfRowSouth'
        }

        var xXmlComposition = fcalXmlSection.querySelector('composition[name="'+xName+'"]');

        // Same as Y but X
        var xXmlMultPosY = xXmlComposition.querySelector('mposX');
        var xCopyCount = parseInt(xXmlMultPosY.getAttribute('ncopy'));
        var x0 = parseFloat(xXmlMultPosY.getAttribute('X0'));
        var dx = parseFloat(xXmlMultPosY.getAttribute('dX'));

        // Region bounding box
        var region = new THREE.Mesh(
            this.boxFromXml(fcalXmlSection, regionShortName,true),
            new THREE.MeshLambertMaterial({visible:false}));
        region.name='FCAL_'+ regionShortName ;

        var material = new THREE.MeshBasicMaterial({
            transparent: true,
            opacity: 0.5,
            color: 0xafd5f7,
            side: THREE.DoubleSide
        });

        // Go through repetitions and create rows
        for(var yIndex=0; yIndex< yCopiesCount; yIndex++){
            // Go through repetitions and create rows
            for(var xIndex=0; xIndex< xCopyCount; xIndex++){

                var module = new THREE.Mesh(moduleBoxGeometry.clone(), material);

                //module.name = "FCAL_" + name + '_' + (startIndex + yIndex) + "_" +xIndex;
                module.name = `FCAL_${regionShortName}_${(startIndex + yIndex)}_${xIndex}`;
                if(isRight) module.name += "_r";

                module.position.set(x0 + xIndex*dx, y0 + yIndex*dy, (45./2)-(45.*scale/2));
                region.add(module);
            }
        }

        // get placement
        var selector = `posXYZ[volume="${regionFullName}"]`;
        var xmlPlacement = fcalXmlSection.querySelector(selector);
        var regionPlacement = extractPlacement(xmlPlacement);
        this.setMeshPlacement(region, regionPlacement);

        return region;
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
            var material = new THREE.MeshBasicMaterial({ transparent: true, opacity: 0.5, vertexColors: THREE.VertexColors });
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

    tubeGeometry: function(rmin, rmax, z, startphi=0, deltaphi=2*Math.PI, isBuffered=true, center=true)
    {
        var shape = new THREE.Shape();
        if ( rmin > 0.0 ) {
            shape.moveTo(0, 0);
            shape.absarc(0, 0, rmin, startphi, startphi+deltaphi, false);
            shape.absarc(0, 0, rmax, startphi+deltaphi, startphi, true);
            shape.closePath();
        }
        else {
            shape.absarc(0, 0, rmax, startphi, deltaphi, false);
        }

        shape.closePath();

        var extrudeSettings = {
            amount : z,
            steps : 1,
            bevelEnabled: false,
            curveSegments: 24
        };

        var geometry = new THREE.ExtrudeGeometry(shape, extrudeSettings);

        if(center){
            geometry.center();
        }
        else {
            geometry.translate(0, 0, -z/2.0 );  // move back to a center of gravity
        }

        if(isBuffered) {
            return new THREE.BufferGeometry().fromGeometry(geometry);
        }
        else
        {
            return geometry
        }
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
    }

};
