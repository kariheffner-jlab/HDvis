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
        }),
         FCALHit: new THREE.PointsMaterial( {
            color: 0x0000ff,
            size: 4,
            opacity:1,
            //blending: THREE.AdditiveBlending,
            transparent: false,
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
            console.log(hit.id);
            var geometry = new THREE.Geometry();
            geometry.name = "FCALHit_" + hit.id;


            var box=new THREE.BoxGeometry(2,2,hit.E*100);
            var boxmesh= new THREE.Mesh(box,scope.materials.FCALHit);
            boxmesh.position.x=hit.x;
            boxmesh.position.y=hit.y;
            boxmesh.position.z=(500 + 173.9);


            scene.add(boxmesh);

                //setRGB(track_color.r,track_color.g,track_color.b);


            //console.log(track_charge);

            boxmesh.name = geometry.name;
            scope.group.add(boxmesh);
            //console.log(track.charge);
        });

        return this.group;
    },

};
