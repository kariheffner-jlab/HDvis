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
        this.shadowGeom=shadowGeometry;
        var tofMesh = geometry.getObjectByName("FTOF");
        if(tofMesh) {
            TOFReferenceColor = tofMesh.getObjectByName("TOFBar_p1_m1", true).material.color;
        }
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
                        "t": schit.t
                    };
                }

            });
        }

        if(this.EventData.charged_tracks) {
            this.EventData.charged_tracks.forEach(function (track) {

                var geometry = new THREE.Geometry();
                geometry.name = "track_" + track.id;//+"_" + track.charge;

                var track_charge = track.charge;

                track.points.forEach(function (point) {
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
                    momentum: track.momentum,
                    TrackChiSq_NDF: track.TrackChiSq_NDF,
                    start_time: track.start_time,
                    steps: track.points
                }
                trackMesh.name = geometry.name;


                scope.group.add(trackMesh);

                var linematerial = new THREE.LineBasicMaterial({color: material.color, linewidth: 2});
                linematerial.side = THREE.DoubleSide;
                var lineBufferGeo = new THREE.BufferGeometry().fromGeometry(geometry);
                var positions = new Float32Array(trackMesh.geometry.vertices.length * 3); // 3 vertices per point
                lineBufferGeo.addAttribute('position', new THREE.BufferAttribute(positions, 3));
                var trackLineMesh = new THREE.Line(lineBufferGeo, linematerial);
                trackLineMesh.frustumCulled = false;
                trackLineMesh.name = "trackline_" + geometry.name;
                trackMesh.add(trackLineMesh);
                //console.log(track.charge);
            });
        }

        //iterate neutral tracks
        if(this.EventData.neutral_tracks) {
            this.EventData.neutral_tracks.forEach(function (track) {

                var geometry = new THREE.Geometry();
                geometry.name = "track_" + track.id;// +"_"; + track.charge;

                track.points.forEach(function (point) {
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
                    momentum: track.momentum,
                    TrackChiSq_NDF: track.TrackChiSq_NDF,
                    start_time: track.start_time,
                    steps: track.points
                }
                trackMesh.name = geometry.name;

                scope.group.add(trackMesh);

                var linematerial = new THREE.LineBasicMaterial({color: material.color, linewidth: 2});
                linematerial.side = THREE.DoubleSide;
                var lineBufferGeo = new THREE.BufferGeometry().fromGeometry(geometry);

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
                    color: 0x0000ff,
                    transparent: true,
                    opacity: .8,
                    visible: vis
                });
                var radius = .5 * Math.log(bpoint.E);
                var bcalpoint = new THREE.SphereGeometry(radius, 32, 32, 0, 6.3, 0, 6.3);

                var bpointmesh = new THREE.Mesh(bcalpoint, material);
                bpointmesh.position.x = bpoint.r * Math.cos(bpoint.theta);
                bpointmesh.position.y = bpoint.r * Math.sin(bpoint.theta);
                bpointmesh.position.z = bpoint.z + 65;//align by 1/2 target length

                bpointmesh.userData = {t: bpoint.t, E: bpoint.E};

                bpointmesh.name = geometry.name;
                scope.group.add(bpointmesh);
                //console.log(track.charge);
            });
        }

        if(this.EventData.FCAL_hits) {
            this.EventData.FCAL_hits.forEach(function (hit) {
                //console.log(hit.id);
                var geometry = new THREE.Geometry();
                geometry.name = "FCALHit_" + hit.id;

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
                    column: hit.column,
                    row: hit.row,
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
                geometry.name = "FCALShower_" + shower.id;


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
                conemesh.position.x = shower.x;
                conemesh.position.y = shower.y;
                conemesh.position.z = shower.z + .5;//+20;d May not be needed.....
                conemesh.rotation.x = -1 * Math.PI / 2;

                conemesh.userData = {fTime: shower.fTime, showerZ: shower.z, fEnergy: shower.fEnergy};

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
                bconemesh.position.x = bshower.x;
                bconemesh.position.y = bshower.y;
                bconemesh.position.z = bshower.z;//Need to shift radially by 1/2 length

                bconemesh.rotation.x = -1 * Math.PI / 2;//Need rotation of showers....but maybe not

                bconemesh.userData = {t: bshower.t, E: bshower.E, z: bshower.z};

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
                    "q": fhit.q,
                    "t": fhit.t,
                    "gLayer": fhit.gLayer,
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
                fdcpseudomesh.position.x = fpseudo.x;
                fdcpseudomesh.position.y = fpseudo.y;
                fdcpseudomesh.position.z = fpseudo.z;

                fdcpseudomesh.userData = {
                    "time": fpseudo.time,
                    "x": fpseudo.x,
                    "y": fpseudo.y,
                    "z": fpseudo.z,
                    "u": fpseudo.u,
                    "v": fpseudo.v
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
                    strawMesh.material.visible = vis;

                    strawMesh.userData.t=cdchit.t
                    scope.geometry.getObjectByName("CDC",true).add(strawMesh);
                }

            });


        }


        if(this.EventData.TOF_points) {
            this.EventData.TOF_points.forEach(function (point) {
                //console.log(hit.id);
                var geometry = new THREE.Geometry();
                geometry.name = "TOFPoint_" + point.id;

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
                pointmesh.position.x = point.x;
                pointmesh.position.y = point.y;
                pointmesh.position.z = point.z;//+40 May not be needed.....

                pointmesh.userData = {t: point.t};

                pointmesh.name = geometry.name;
                scope.group.add(pointmesh);
                //console.log(track.charge);
            });
        }

        var tofMesh = scope.geometry.getObjectByName("FTOF");
        if(tofMesh) {
            tofMesh.children.forEach(function (tofPlane) {
                tofPlane.children.forEach(function (tofSector) {
                    tofSector.children.forEach(function (tofStrip) {
                        tofStrip.material = new THREE.MeshBasicMaterial({
                            transparent: true,
                            opacity: 0.5,
                            vertexColors: THREE.VertexColors
                        });
                        //tofStrip.material.vertexColors=THREE.VertexColors;
                        tofStrip.userData.end0h = 0;
                        tofStrip.userData.end1h = 0;
                        tofStrip.userData.HitTimes = [];
                        //tofStrip.geometry.colorsNeedUpdate = true;
                    });
                });
            });

            var TOFOneHitColor = new THREE.Color;
            TOFOneHitColor.setRGB(1, 1, 0);

            var TOFTwoHitColor = new THREE.Color;
            TOFTwoHitColor.setRGB(1, .66, 0);

            var TOFThreeHitColor = new THREE.Color;
            TOFThreeHitColor.setRGB(1, 0, 0);

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

                    var object = tofMesh.getObjectByName(geoName, true);
                    //getObjectByName( "TestBox", true );

                    if (object) {
                        if (object.geometry.type === "BufferGeometry") {
                            object.geometry = new THREE.Geometry().fromBufferGeometry(object.geometry);
                        }

                        object.userData.HitTimes.push({end: end, time: time});

                        /* var inside=glowMesh.insideMesh.material.uniforms;
                         inside.glowColor.value.setRGB(color.r,color.g,color.b);
                         inside.power.value=.5;
                         var outside=glowMesh.insideMesh.material.uniforms;

                         outside.glowColor.value.setRGB(color.r,color.g,color.b);
                         outside.power.value=.5;
                         console.log(end+":"+object.userData.end0h+","+object.userData.end1h );*/


                        object.geometry.colorsNeedUpdate = true;

                    }
                    else {
                        console.log("DIDN'T FIND " + geoName);
                    }//console.log(object);

                });
            }
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

                var geoName = "BCAL_m" + (module - 1).toString() + "_l" + (layer - 1).toString() + "_s" + (sector - 1).toString()+"_e"+end.toString();


                var modulemesh = scope.shadowGeom.getObjectByName(geoName, true);
                //getObjectByName( "TestBox", true );

                if (modulemesh) {
                    /* if (modulemesh.geometry.type === "BufferGeometry") {
                         modulemesh.geometry = new THREE.Geometry().fromBufferGeometry(modulemesh.geometry);
                     }*/
                    var moduletoadd=modulemesh;//.clone();
                    moduletoadd.userData.t = hit.t;

                    moduletoadd.geometry.colorsNeedUpdate = true;
                    if(end==1)
                        scope.geometry.getObjectByName("BCAL_details", true).add(moduletoadd);
                    else
                        scope.geometry.getObjectByName("BCAL_details2", true).add(moduletoadd);

                }
                else {
                    console.log("DIDN'T FIND " + geoName);
                }//console.log(object);

            });
        }


        return this.group;
    }

};
