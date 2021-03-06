THREE.GluexEventLoader = function (config) {
    this.EventData = null;
};

THREE.GluexEventLoader.prototype = {

    constructor: THREE.GluexEventLoader,

    group: new THREE.Group(name="GluexEvent"),
    defines: {},
    geometries: {},
    refs: {},
    meshes: [],
    geometry:null,
    Configuration:null,
    TOFReferenceColor:null,
    shadowGeom:null,
    setGeometry: function (geometry,shadowGeometry) {
        this.geometry = geometry;
        this.shadowGeom=shadowGeometry.clone();
        this.TOFReferenceColor = new THREE.Color(1,1,1);
    },
    setConfiguration: function (config) {
        this.Configuration = config;
    },

    load: function (url, onLoad, onProgress, onError) {

        this.group.name="GluexEvent";
        var scope = this;

        var loader = new THREE.FileLoader();//XHRLoader();
        loader.setPath(this.path);
        loader.load(url, function (text) {
            onLoad(scope.parse(text));
        }, onProgress, onError);
    },

    /** Function that works with loaded json */
    parse: function (text) {
        this.EventData = JSON.parse(text);
        var eventNum=this.EventData.event_number;
        var scope = this;
        scene.name="Hall";


        var scobj=scope.geometry.getObjectByName("SC");
        if(scobj) {
            var eventobjssc = scobj.children[0].children;
            if (eventobjssc) {
                for (var i = 0; i < eventobjssc.length; i++) {
                    if (eventobjssc[i].name.split('_')[0] === "SCsector") {
                        eventobjssc[i].material.color.setRGB(255. / 255., /*141.*/255. / 255., /*66.*/255. / 255.);
                        eventobjssc[i].userData = {};

                    }
                }
            }
        }
        if(this.EventData.SC_hits) {
            this.EventData.SC_hits.forEach(function (schit) {

                var name="SCsector_"+schit.sector;

                var sectorMesh = scope.geometry.getObjectByName(name);

                if(sectorMesh) {
                    sectorMesh.material.color.setRGB(255. / 255., /*141.*/255. / 255., /*66.*/255. / 255.);

                    sectorMesh.userData = {
                        "t": schit.t,
                        "t_fADC": schit.t_fADC,
                        "t_TDC": schit.t_TDC,
                        "pulse_height": schit.pulse_height,
                        "dE": schit.dE

                    };
                }

            });
        }

        if(this.EventData.charged_tracks) {
            this.EventData.charged_tracks.forEach(function (track) {

                var defaultPIDname=track.BestTrackingPID;

                var track_to_display=track.TrackHypos[0];
                track.TrackHypos.forEach(function (hypo){
                    if(hypo.id===defaultPIDname)
                        track_to_display=hypo;
                });


                if(track_to_display.points.length > 0) {

                    var geometry = new THREE.Geometry();
                    geometry.name = "track_" + track_to_display.id;//+"_" + track.charge;

                    var track_charge = track_to_display.charge;

                    track_to_display.points.forEach(function (point) {
                        var vertex = new THREE.Vector3();
                        vertex.x = point[0];
                        vertex.y = point[1];
                        vertex.z = point[2];

                        //setRGB(track_color.r,track_color.g,track_color.b);
                        geometry.vertices.push(vertex);
                    });

                    var trackcolor = new THREE.Color;
                    var swim_vis = false;


                    if (track_charge === 1) {
                        trackcolor = 0xff0000;
                        if (scope.Configuration.positive_track_swim !== false) {
                            swim_vis = true;
                        }

                    }//console.log(track_charge);
                    else if (track_charge === -1) {
                        trackcolor = 0x00ff00;
                        if (scope.Configuration.negative_track_swim !== false) {
                            swim_vis = true;
                        }
                    }
                    var material = new THREE.PointsMaterial({
                        color: trackcolor,
                        size: 4,
                        opacity: .6,
                        visible: swim_vis,
                        //blending: THREE.AdditiveBlending,
                        transparent: true,
                        sizeAttenuation: false
                    });

                    var trackMesh = new THREE.Points(geometry, material);

                    trackMesh.userData = {
                        charge: track_charge,
                        mass: track_to_display.mass,
                        position: track_to_display.position,
                        momentum: track_to_display.momentum,
                        TrackChiSq_NDF: track_to_display.TrackChiSq_NDF,
                        start_time: track_to_display.start_time,
                        steps: track_to_display.points,
                        track_hypos:track.TrackHypos
                    }
                    trackMesh.name = geometry.name;


                    scope.group.add(trackMesh);

                    var linematerial = new THREE.LineBasicMaterial({color: material.color, linewidth: 2});
                    linematerial.side = THREE.DoubleSide;
                    var lineBufferGeo = new THREE.BufferGeometry().fromGeometry(geometry);
                    lineBufferGeo.drawRange.count = 0;
                    var positions = new Float32Array(trackMesh.geometry.vertices.length * 3); // 3 vertices per point
                    lineBufferGeo.addAttribute('position', new THREE.BufferAttribute(positions, 3));
                    var trackLineMesh = new THREE.Line(lineBufferGeo, linematerial);
                    trackLineMesh.frustumCulled = false;
                    trackLineMesh.name = "trackline_" + geometry.name;
                    trackMesh.add(trackLineMesh);
                    //console.log(track.charge);
                }
            });
        }

        //iterate neutral tracks
        if(this.EventData.neutral_tracks) {
            this.EventData.neutral_tracks.forEach(function (track) {

                var defaultPIDname=track.BestTrackingPID;

                var track_to_display=track.TrackHypos[0];
                track.TrackHypos.forEach(function (hypo){
                    if(hypo.id === defaultPIDname)
                        track_to_display=hypo;
                });

                var geometry = new THREE.Geometry();
                geometry.name = "track_" + track_to_display.id;// +"_"; + track.charge;

                track_to_display.points.forEach(function (point) {
                    var vertex = new THREE.Vector3();
                    vertex.x = point[0];
                    vertex.y = point[1];
                    vertex.z = point[2];

                    geometry.vertices.push(vertex);
                });

                var swim_vis = false;
                if (scope.Configuration.neutral_track_swim !== false) {
                    swim_vis = true;
                }

                var material = new THREE.PointsMaterial({
                    color: 0xffff00,
                    size: 4,
                    opacity: .6,
                    visible: swim_vis,
                    //blending: THREE.AdditiveBlending,
                    transparent: true,
                    sizeAttenuation: false
                });


                var trackMesh = new THREE.Points(geometry, material);

                trackMesh.userData = {
                    charge: 0,
                    mass: track_to_display.mass,
                    position: track_to_display.position,
                    momentum: track_to_display.momentum,
                    TrackChiSq_NDF: track_to_display.TrackChiSq_NDF,
                    start_time: track_to_display.start_time,
                    steps: track_to_display.points,
                    track_hypos:track.TrackHypos
                }
                trackMesh.name = geometry.name;

                scope.group.add(trackMesh);

                var linematerial = new THREE.LineBasicMaterial({color: material.color, linewidth: 2});
                linematerial.side = THREE.DoubleSide;
                var lineBufferGeo = new THREE.BufferGeometry().fromGeometry(geometry);
                var trackLineMesh = new THREE.Line(lineBufferGeo, linematerial);
                var positions = new Float32Array(trackMesh.geometry.vertices.length * 3); // 3 vertices per point
                lineBufferGeo.addAttribute('position', new THREE.BufferAttribute(positions, 3));
                var trackLineMesh = new THREE.Line(lineBufferGeo, linematerial);
                trackLineMesh.frustumCulled = false;
                trackLineMesh.name = "trackline_" + geometry.name;
                trackMesh.add(trackLineMesh);
                //console.log(track.charge);
            });
        }
        if(this.EventData.BCAL_points) {
            this.EventData.BCAL_points.forEach(function (bpoint) {
                //console.log(hit.id);
                var geometry = new THREE.Geometry();
                geometry.name = "BCALPoint_" + "BCALpoint " + bpoint.id;

                var vis = true;
                if (scope.Configuration.BCALPointVis === "Off") {
                    vis = false
                }

                var material = new THREE.MeshBasicMaterial({
                    color: 0x68d6ff,
                    transparent: true,
                    opacity: .8,
                    visible: vis
                });
                var radius = .5 * Math.log(bpoint.E*1000);
                var bcalpoint = new THREE.SphereGeometry(radius, 32, 32, 0, 6.3, 0, 6.3);

                var bpointmesh = new THREE.Mesh(bcalpoint, material);
                bpointmesh.position.x = bpoint.position[0][0]
                bpointmesh.position.y = bpoint.position[0][1]
                bpointmesh.position.z = bpoint.position[0][2] + 65;//align by 1/2 target length

                bpointmesh.userData = {"position": bpoint.position, t: bpoint.t, E: bpoint.E, layer: bpoint.layer, sector: bpoint.sector, module: bpoint.sector};

                bpointmesh.name = geometry.name;
                scope.group.add(bpointmesh);
                //console.log(track.charge);
            });
        }

        if(this.EventData.FCAL_hits) {
            this.EventData.FCAL_hits.forEach(function (hit) {
                //console.log(hit.id);
                var geometry = new THREE.Geometry();
                geometry.name = "FCALHit_" +"FCAL Hit "+ hit.id;

                var box = new THREE.BoxGeometry(2, 2, 1/*hit.E*FCAL_EScale*/);

                var vis = true;
                if (scope.Configuration.FCALHitVis === "Off") {
                    vis = false
                }

                var material = new THREE.MeshBasicMaterial({
                    color: 0xffffff,
                    transparent: true,
                    opacity: .7,
                    visible: vis
                });


                if (hit.t >= 0.0) {
                    material.color.r = 1;
                    material.color.g = 0;
                    material.color.b = 0;
                }
                else {
                    material.color.r = 0;
                    material.color.g = 1;
                    material.color.b = 0;
                }

                var boxmesh = new THREE.Mesh(box, material);
                boxmesh.position.x = hit.x;
                boxmesh.position.y = hit.y;
                boxmesh.position.z = (655 + 15 + 1);//(FCAL_EScale/2)*hit.E);//660

                boxmesh.userData = {
                    E: hit.E,
                    // column: hit.column,
                    // row: hit.row,
                    x: hit.x,
                    y: hit.y,
                    intOverPeak: hit.intOverPeak,
                    t: hit.t
                };

                boxmesh.name = geometry.name;
                scope.group.add(boxmesh);
                //console.log(track.charge);
            });
        }
        if(this.EventData.FCAL_showers) {
            this.EventData.FCAL_showers.forEach(function (shower) {
                //console.log(hit.id);
                var geometry = new THREE.Geometry();
                geometry.name = "FCALShower_" +"FCAL Shower "+shower.id;

                var cone = new THREE.ConeGeometry(1, 1, 60, 60, 0, 0, 2 * Math.PI);//(10,20...)

                var vis = true;
                if (scope.Configuration.FCALShowerVis === "Off") {
                    vis = false
                }

                var material = new THREE.MeshBasicMaterial({
                    color: 0xffff00,
                    transparent: false,
                    opacity: .4,
                    visible: vis
                });
                material.side = THREE.DoubleSide;

                var conemesh = new THREE.Mesh(cone, material);
                conemesh.position.x = shower.position[0][0];
                conemesh.position.y = shower.position[0][1];
                conemesh.position.z = shower.position[0][2] + .5;//+20;d May not be needed.....
                conemesh.rotation.x = -1 * Math.PI / 2;

                conemesh.userData = {"position": shower.position, fTime: shower.fTime,  fEnergy: shower.fEnergy};

                conemesh.name = geometry.name;
                scope.group.add(conemesh);
                //console.log(track.charge);
            });
        }
        if(this.EventData.BCAL_showers) {
            this.EventData.BCAL_showers.forEach(function (bshower) {
                //console.log(hit.id);
                var geometry = new THREE.Geometry();
                geometry.name = "BCALShower_" + "BCAL Shower " + bshower.id;


                var bcone = new THREE.ConeGeometry(1, 1, 60, 60, 0, 0, 2 * Math.PI);//(10,20...)

                var vis = true;
                if (scope.Configuration.BCALShowerVis === "Off") {
                    vis = false
                }

                var bmaterial = new THREE.MeshBasicMaterial({
                    color: 0xffff00,
                    transparent: false,
                    opacity: .4,
                    visible: vis
                });
                bmaterial.side = THREE.DoubleSide;

                var bconemesh = new THREE.Mesh(bcone, bmaterial);
                bconemesh.position.x = bshower.position[0][0];
                bconemesh.position.y = bshower.position[0][1];
                bconemesh.position.z = bshower.position[0][2];

                bconemesh.rotation.x = -1 * Math.PI / 2;//Need rotation of showers....but maybe not

                bconemesh.userData = {position: bshower.position ,t: bshower.t, E: bshower.E, Epreshower: bshower.Epreshower, position: bshower.position, Ncell: bshower.Ncell};

                bconemesh.name = geometry.name;
                scope.group.add(bconemesh);
                //console.log(track.charge);
            });
        }
        if(this.EventData.FDC_hits) {
            this.EventData.FDC_hits.forEach(function (fhit) {
                //console.log(hit.id);
                var geometry = new THREE.Geometry();
                geometry.name = "FDCHit_" + "FDC Hit " + fhit.id;

                //var start=new THREE.Vector3(0,-64,175.548+(fhit.gPlane-1)*2.611);
                //var end=new THREE.Vector3(0,64,175.548+(fhit.gPlane-1)*2.611);
                var rdisp = fhit.u;

                if (fhit.type === 0) {
                    rdisp = Math.sqrt(fhit.midx * fhit.midx + fhit.midy * fhit.midy);
                }

                var len = Math.sqrt(64.0485 * 64.0485 - rdisp * rdisp);
                var start = new THREE.Vector3(0, -1 * len, fhit.midz);
                var end = new THREE.Vector3(0, len, fhit.midz);

                geometry.vertices.push(
                    start, end
                );

                var hit_color = new THREE.Color(0, 0, 0);


                if (fhit.type === 0) {
                    hit_color.r = 244. / 255.;
                    hit_color.g = 66. / 255.;
                    hit_color.b = 223. / 255.;
                }
                else {

                    // console.log("green")
                    hit_color.r = 144. / 255.;
                    hit_color.g = 249. / 255.;
                    hit_color.b = 164. / 255.;
                }

                var vis = false;
                if (scope.Configuration.FDCHitVis !== "Off") {
                    if (scope.Configuration.FDCHitTypeVis === "Cathodes" && fhit.type !== 0) {
                        vis = true;
                    }
                    else if (scope.Configuration.FDCHitTypeVis === "Anodes" && fhit.type === 0) {
                        vis = true;
                    }
                    else {
                        vis = true;
                    }
                }


                var hitmaterial = new THREE.LineBasicMaterial({
                    color: hit_color,
                    transparent: false,
                    opacity: .4,
                    visible: vis
                });
                hitmaterial.side = THREE.DoubleSide;

                var linemesh = new THREE.LineSegments(geometry, hitmaterial);
                linemesh.frustumCulled = false;

                var base_angle = 0;

                if (fhit.gLayer % 6 === 0) {
                    base_angle = -120;
                }
                else if (fhit.gLayer % 6 === 1) {
                    base_angle = 180;
                }
                else if (fhit.gLayer % 6 === 2) {
                    base_angle = 120;
                }
                else if (fhit.gLayer % 6 === 3) {
                    base_angle = 60;
                }
                else if (fhit.gLayer % 6 === 4) {
                    base_angle = 0;
                }
                else if (fhit.gLayer % 6 === 5) {
                    base_angle = -60;
                }

                var radAngle = fhit.angle + (base_angle) * Math.PI / 180.;

                if (fhit.plane === 3/*(fhit.gPlane===3 || fhit.gPlane===6 || fhit.gPlane===9 || fhit.gPlane===12)*/) {
                    radAngle = radAngle - Math.PI + (150. / 180.) * Math.PI;
                }
                else if (fhit.plane === 1) {
                    radAngle = radAngle - (150. / 180.) * Math.PI
                }

                //console.log(fhit.gPlane+" "+base_angle+" "+fhit.plane+": "+radAngle*180./Math.PI);

                if (fhit.type === 0) {
                    linemesh.position.x = fhit.midx;
                    linemesh.position.y = fhit.midy;
                }
                else {

                    if (fhit.plane === 3) {
                        // linemesh.translateX(fhit.u);
                        // linemesh.position.x=fhit.u;
                        linemesh.geometry.vertices[0].x = -1 * fhit.u;
                        linemesh.geometry.vertices[1].x = -1 * fhit.u;
                    }
                    else if (fhit.plane === 1) {
                        //linemesh.translateX(fhit.u);
                        //linemesh.position.x=-1*fhit.u;
                        linemesh.geometry.vertices[0].x = fhit.u;
                        linemesh.geometry.vertices[1].x = fhit.u;

                    }//linemesh.position.x=fhit.u;
                }


                // console.log(linemesh.geometry.vertices[0]);
                var newX0 = linemesh.geometry.vertices[0].x * Math.cos(radAngle) - linemesh.geometry.vertices[0].y * Math.sin(radAngle);
                var newY0 = linemesh.geometry.vertices[0].x * Math.sin(radAngle) + linemesh.geometry.vertices[0].y * Math.cos(radAngle);

                linemesh.geometry.vertices[0].x = newX0;
                linemesh.geometry.vertices[0].y = newY0;

                //console.log(linemesh.geometry.vertices[0]);

                var newX1 = linemesh.geometry.vertices[1].x * Math.cos(radAngle) - linemesh.geometry.vertices[1].y * Math.sin(radAngle);
                var newY1 = linemesh.geometry.vertices[1].x * Math.sin(radAngle) + linemesh.geometry.vertices[1].y * Math.cos(radAngle);

                linemesh.geometry.vertices[1].x = newX1;
                linemesh.geometry.vertices[1].y = newY1;


                linemesh.userData = {
                    "package": fhit.package,
                    "chamber": fhit.chamber,
                    "layer": fhit.layer,
                    "q": fhit.q,
                    "t": fhit.t,
                    "pulse_height_raw": fhit.pulse_height_raw,
                    "plane": fhit.plane,
                    "u": fhit.u,
                    "type": fhit.type
                };

                linemesh.name = geometry.name;
                scope.group.add(linemesh);
                //console.log(track.charge);
            });
        }
        if(this.EventData.FDC_pseudos) {
            this.EventData.FDC_pseudos.forEach(function (fpseudo) {
                //console.log(hit.id);
                var geometry = new THREE.Geometry();
                geometry.name = "FDCPseudo_" + "FDCPseudo " + fpseudo.id;

                var vis = true;
                if (scope.Configuration.FDCPseudoVis === "Off") {
                    vis = false;
                }

                var material = new THREE.MeshBasicMaterial({
                    color: 0x0000ff,
                    transparent: false,
                    opacity: .8,
                    visible: vis
                });
                var radius = 2;
                var fdcpoint = new THREE.SphereGeometry(radius, 32, 32, 0, 6.3, 0, 6.3);

                var fdcpseudomesh = new THREE.Mesh(fdcpoint, material);
                fdcpseudomesh.position.x = fpseudo.position[0][0];
                fdcpseudomesh.position.y = fpseudo.position[0][1];
                fdcpseudomesh.position.z = fpseudo.position[0][2];

                fdcpseudomesh.userData = {
                    "package": fpseudo.package,
                    "chamber": fpseudo.chamber,
                    "position": fpseudo.position,
                    "w": fpseudo.w,
                    "s": fpseudo.s,
                    "time": fpseudo.time,
                    "u": fpseudo.u,
                    "t_u": fpseudo.u_t,
                    "v": fpseudo.v,
                    "t_v": fpseudo.v_t,
                    "dE": fpseudo.dE,
                    "q": fpseudo.q
                };

                fdcpseudomesh.name = geometry.name;
                scope.group.add(fdcpseudomesh);
                //console.log(track.charge);
            });
        }

        //var cdcobj=scope.geometry.getObjectByName("CDC",true);
        // var shadowcdcobj=scope.shadowGeom.getObjectByName("CDC",true)

        if (this.EventData.CDC_hits) {

            this.EventData.CDC_hits.forEach(function (cdchit) {


                var vis = false;
                if (scope.Configuration.CDCHitVis === "Static") {
                    vis = true;
                }

                var name = "CDCstraw_" + cdchit.ring + "_" + cdchit.straw;

                // var strawMesh = scope.geometry.getObjectByName(name, true);
                var strawMesh = scope.shadowGeom.getObjectByName(name, true);
                if (strawMesh) {
                    var strawmeshToAdd= strawMesh;//.clone();
                    strawmeshToAdd.material.visible = vis;
                    strawmeshToAdd.name=name;
                    strawmeshToAdd.userData={t: cdchit.t, q: cdchit.q, d: cdchit.d, itrack: cdchit.itrack, ptype: cdchit.ptype}
                    scope.geometry.getObjectByName("CDC",true).add(strawmeshToAdd);
                }

            });


        }


        if(this.EventData.TOF_points) {
            this.EventData.TOF_points.forEach(function (point) {
                //console.log(hit.id);
                var geometry = new THREE.Geometry();
                geometry.name = "TOFPoint_" +"TOF Point "+ point.id;

                var vis = true;
                if (scope.Configuration.TOFPointVis === "Off") {
                    vis = false
                }

                var material = new THREE.MeshBasicMaterial({
                    color: 0x0000ff,
                    transparent: true,
                    opacity: .8,
                    visible: vis
                });
                var tofpoint = new THREE.SphereGeometry(2, 32, 32, 0, 6.3, 0, 6.3);

                var pointmesh = new THREE.Mesh(tofpoint, material);
                pointmesh.position.x = point.position[0][0];
                pointmesh.position.y = point.position[0][1];
                pointmesh.position.z = point.position[0][2];;//+40 May not be needed.....

                pointmesh.userData = {"position": point.position ,"t": point.t,"tErr": point.tErr, "dE": point.dE};

                pointmesh.name = geometry.name;
                scope.group.add(pointmesh);
                //console.log(track.charge);
            });
        }


        if (this.EventData.TOF_hits) {
            this.EventData.TOF_hits.forEach(function (hit) {
                //get the object to change and change it
                // console.log("hit id:"+hit.id);
                var plane = hit.plane;
                var bar = hit.bar;
                var end = hit.end;
                var time = hit.t;

                var geoName = "TOFBar_p" + plane + "_m" + (bar - 1);

                if ((bar - 1 === 21 || bar - 1 === 22) && end === 0) {
                    geoName += "_r";
                }

                var object = scope.shadowGeom.getObjectByName(geoName, true);
                var sceneobject = scope.geometry.getObjectByName(geoName, true);
                //getObjectByName( "TestBox", true );

                if(sceneobject)
                {
                    sceneobject.userData.HitTimes.push({end: end, time: time});
                    sceneobject.userData.HitEnds.push(end);
                    sceneobject.userData.HitdE.push(hit.dE);
                    sceneobject.userData.HitAmp.push(hit.Amp);
                    sceneobject.userData.Hitt.push(hit.t);
                    sceneobject.userData.Hitt_fADC.push(hit.t_fADC);
                    sceneobject.userData.Hitt_TDC.push(hit.t_TDC);
                }
                else if (object) {
                    if (object.geometry.type === "BufferGeometry") {
                        object.geometry = new THREE.Geometry().fromBufferGeometry(object.geometry);
                    }


                    var objectToAdd=object;//.clone();

                    objectToAdd.userData={
                        plane: hit.plane,
                        bar: hit.bar
                    }

                    if(objectToAdd.userData.HitEnds) {
                        objectToAdd.userData.HitEnds.push(end);
                    }
                    else
                    {
                        objectToAdd.userData.HitEnds=[];
                        objectToAdd.userData.HitEnds.push(end);
                    }

                    if(objectToAdd.userData.HitdE) {
                        objectToAdd.userData.HitdE.push(hit.dE);
                    }
                    else
                    {
                        objectToAdd.userData.HitdE=[];
                        objectToAdd.userData.HitdE.push(hit.dE);
                    }

                    if(objectToAdd.userData.HitAmp) {
                        objectToAdd.userData.HitAmp.push(hit.Amp);
                    }
                    else
                    {
                        objectToAdd.userData.HitAmp=[];
                        objectToAdd.userData.HitAmp.push(hit.Amp);
                    }

                    if(objectToAdd.userData.Hitt) {
                        objectToAdd.userData.Hitt.push(hit.t);
                    }
                    else
                    {
                        objectToAdd.userData.Hitt=[];
                        objectToAdd.userData.Hitt.push(hit.t);
                    }

                    if(objectToAdd.userData.Hitt_fADC) {
                        objectToAdd.userData.Hitt_fADC.push(hit.t_fADC);
                    }
                    else
                    {
                        objectToAdd.userData.Hitt_fADC=[];
                        objectToAdd.userData.Hitt_fADC.push(hit.t_fADC);
                    }

                    if(objectToAdd.userData.Hitt_TDC) {
                        objectToAdd.userData.Hitt_TDC.push(hit.t_TDC);
                    }
                    else
                    {
                        objectToAdd.userData.Hitt_TDC=[];
                        objectToAdd.userData.Hitt_TDC.push(hit.t_TDC);
                    }

                    if(objectToAdd.userData.HitTimes) {
                        objectToAdd.userData.HitTimes.push({end: end, time: time});
                    }
                    else
                    {
                        objectToAdd.userData.HitTimes=[];
                        objectToAdd.userData.HitTimes.push({end: end, time: time});
                    }

                    objectToAdd.geometry.colorsNeedUpdate = true;

                    if(plane==1)
                        scope.geometry.getObjectByName(object.parent.name, true).add(objectToAdd);
                    else
                        scope.geometry.getObjectByName(object.parent.name, true).add(objectToAdd);
                    /* var inside=glowMesh.insideMesh.material.uniforms;
                     inside.glowColor.value.setRGB(color.r,color.g,color.b);
                     inside.power.value=.5;
                     var outside=glowMesh.insideMesh.material.uniforms;

                     outside.glowColor.value.setRGB(color.r,color.g,color.b);
                     outside.power.value=.5;
                     console.log(end+":"+object.userData.end0h+","+object.userData.end1h );*/




                }

                else
                {
                    console.log("DIDN'T FIND " + geoName);
                }//console.log(object);

            });
        }


        // var bcalMesh = scope.shadowGeom.getObjectByName("BCAL");
        if(this.EventData.BCAL_hits) {
            this.EventData.BCAL_hits.forEach(function (hit) {
                //get the object to change and change it
                // console.log("hit id:"+hit.id);
                var module = hit.module;
                var layer = hit.layer;
                var sector = hit.sector;
                var end = hit.end;
                var time = hit.t;

                var geoName = "BCAL_m" + (module).toString() + "_l" + (layer).toString() + "_s" + (sector).toString()+"_e"+end.toString();


                var modulemesh = scope.shadowGeom.getObjectByName(geoName, true);
                var sceneobject = scope.geometry.getObjectByName(geoName, true);
                //getObjectByName( "TestBox", true );

                if (modulemesh) {

                    var moduletoadd=modulemesh;//.clone();
                    moduletoadd.userData = { "E": hit.E, "t": hit.t,"t_raw": hit.t_raw,"pulse_peak":hit.pulse_peak };

                    moduletoadd.geometry.colorsNeedUpdate = true;
                    if(end===1)
                        scope.geometry.getObjectByName("BCAL_details", true).add(moduletoadd);
                    else
                        scope.geometry.getObjectByName("BCAL_details2", true).add(moduletoadd);

                }
                else if(sceneobject)
                {
                    sceneobject.userData = { "E": hit.E, "t": hit.t,"t_raw": hit.t_raw,"pulse_peak":hit.pulse_peak };
                }
                else {
                    console.log("DIDN'T FIND " + geoName);
                }//console.log(object);

            });
        }


        return this.group;
    }

};
