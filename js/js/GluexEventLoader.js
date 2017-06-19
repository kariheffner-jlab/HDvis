THREE.GluexEventLoader = function () {
    this.EventData = null;
    this.materials = {
        posTrack: new THREE.PointsMaterial( {
            color: 0xff0000,
            size: 4,
            opacity:0.6,
            //blending: THREE.AdditiveBlending,
            transparent: true,
            sizeAttenuation: false
        }),
        neuTrack: new THREE.PointsMaterial( {
            color: 0xffff00,
            size: 4,
            opacity:.6,
            //blending: THREE.AdditiveBlending,
            transparent: false,
            sizeAttenuation: false
        }),
        negTrack: new THREE.PointsMaterial( {
            color: 0x00ff00,
            size: 4,
            opacity:.6,
            //blending: THREE.AdditiveBlending,
            transparent: true,
            sizeAttenuation: false
        })
    }
};

THREE.GluexEventLoader.prototype = {

    constructor: THREE.GluexEventLoader,

    group: new THREE.Group(name="GluexEvent"),
    defines: {},
    geometries: {},
    refs: {},
    meshes: [],

    load: function (url, onLoad, onProgress, onError) {

        var scope = this;

        var loader = new THREE.XHRLoader();
        loader.setPath(this.path);
        loader.load(url, function (text) {
            onLoad(scope.parse(text));
        }, onProgress, onError);
    },

    /** Function that works with loaded json */
    parse: function (text) {
        this.EventData = JSON.parse(text);

        var scope = this;

        // Iterate charged_tracks
        this.EventData.charged_tracks.forEach(function (track) {

            var geometry = new THREE.Geometry();
            geometry.name = "track_" + track.id +"_" + track.charge;

            var track_charge = track.charge;

            track.points.forEach(function (point) {
                var vertex = new THREE.Vector3();
                vertex.x = point[0];
                vertex.y = point[1];
                vertex.z = point[2];

                //setRGB(track_color.r,track_color.g,track_color.b);
                geometry.vertices.push( vertex );
                });

            var trackMesh= new THREE.Points( geometry, scope.materials.posTrack );
            //console.log(track_charge);

            if(track_charge == "-1") {
                trackMesh= new THREE.Points( geometry, scope.materials.negTrack );
            }

            trackMesh.name = geometry.name;
            scope.group.add(trackMesh);
            //console.log(track.charge);
        });
        //iterate neutral tracks
        this.EventData.neutral_tracks.forEach(function (track) {

            var geometry = new THREE.Geometry();
            geometry.name = "ntrack_" + track.id +"_" + track.charge;

            track.points.forEach(function (point) {
                var vertex = new THREE.Vector3();
                vertex.x = point[0];
                vertex.y = point[1];
                vertex.z = point[2];

                //setRGB(track_color.r,track_color.g,track_color.b);
                geometry.vertices.push( vertex );
            });

            var trackMesh= new THREE.Points( geometry, scope.materials.neuTrack );
            //scope.materials.neuTrack["color"]=0x0000ff;
            //console.log(track_charge);

            trackMesh.name = geometry.name;
            scope.group.add(trackMesh);
            //console.log(track.charge);
        });


        this.EventData.FCAL_hits.forEach(function (hit) {
            //console.log(hit.id);
            var geometry = new THREE.Geometry();
            geometry.name = "FCALHit_" + hit.id;


            var box=new THREE.BoxGeometry(2,2,hit.E*100);
            var material = new THREE.MeshBasicMaterial({color:0xffffff, transparent:true, opacity:.7});

            var redness=Math.abs(hit.t)*(6);
            if (redness < 255.) {
                //console.log("in if");
                redness = Math.abs(hit.t) * (6);
            }
            else
            {
                redness=255;
            }
            redness=redness/255;
            //console.log(redness);
            if (hit.t >= 0.0) {
                material.color.r = redness;
                material.color.g = 0;
                material.color.b = 0;
            }
            else {
                material.color.r = 0;
                material.color.g = redness;
                material.color.b = 0;
            }

            var boxmesh= new THREE.Mesh(box,material);
            boxmesh.position.x=hit.x;
            boxmesh.position.y=hit.y;
            boxmesh.position.z=(500 + 173.9+50*hit.E);

            scene.add(boxmesh);

            //console.log(track_charge);

            boxmesh.name = geometry.name;
            scope.group.add(boxmesh);
            //console.log(track.charge);
        });

        this.EventData.FCAL_showers.forEach(function (shower) {
            //console.log(hit.id);
            var geometry = new THREE.Geometry();
            geometry.name = "FCALShower_" + shower.id;


            var cone=new THREE.ConeGeometry(10,20,60,60,0,0,2*Math.PI);
            var material = new THREE.MeshBasicMaterial({color:0x0000ff, transparent:true, opacity:.4, sizeAtten: false});

            var conemesh= new THREE.Mesh(cone,material);
            conemesh.position.x=shower.x;
            conemesh.position.y=shower.y;
            conemesh.position.z=shower.z+40;//+40 May not be needed.....
            conemesh.rotation.x = -1*Math.PI/2;
            scene.add(conemesh);

            //setRGB(track_color.r,track_color.g,track_color.b);
            //console.log(track_charge);

            conemesh.name = geometry.name;
            scope.group.add(conemesh);
            //console.log(track.charge);
        });

        /*var testcone= new THREE.ConeGeometry(25,50,64,64,1,0,2*Math.PI);
        var conemat = new THREE.MeshBasicMaterial({color:0x0000ff, transparent:false, opacity:1});
        var testmesh= new THREE.Mesh(testcone,conemat);
        testmesh.rotation.x = -1*Math.PI/2;
        scene.add(testmesh);*/

        return this.group;
    },



};
