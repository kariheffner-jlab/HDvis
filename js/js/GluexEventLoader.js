THREE.GluexEventLoader = function () {
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

    setGeometry: function (geometry) {
        this.geometry = geometry;
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

        var scope = this;
        scene.name="Hall";
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

            if(track_charge == "1") {
                var material = new THREE.PointsMaterial({
                    color: 0xff0000,
                    size: 4,
                    opacity: 0.6,
                    //blending: THREE.AdditiveBlending,
                    transparent: true,
                    sizeAttenuation: false
                })
                var trackMesh = new THREE.Points(geometry, material);
            }//console.log(track_charge);
            else if(track_charge == "-1") {
                var material = new THREE.PointsMaterial({
                    color: 0x00ff00,
                    size: 4,
                    opacity: 0.6,
                    //blending: THREE.AdditiveBlending,
                    transparent: true,
                    sizeAttenuation: false
                })
                var trackMesh= new THREE.Points( geometry, material );
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

            var material = new THREE.PointsMaterial( {
                color: 0xffff00,
                size: 4,
                opacity:.6,
                //blending: THREE.AdditiveBlending,
                transparent: true,
                sizeAttenuation: false
            })
            var trackMesh= new THREE.Points( geometry, material );
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
            boxmesh.position.z=(660+50*hit.E);

            //scene.add(boxmesh);

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
            cone.userData="hi";
            var material = new THREE.MeshBasicMaterial({color:0xffff00, transparent:false, opacity:.4});
            material.side = THREE.DoubleSide;

            var conemesh= new THREE.Mesh(cone,material);
            conemesh.position.x=shower.x;
            conemesh.position.y=shower.y;
            conemesh.position.z=shower.z+20;//+30;//+40 May not be needed.....
            conemesh.rotation.x = -1*Math.PI/2;

            conemesh.name = geometry.name;
            scope.group.add(conemesh);
            //console.log(track.charge);
        });

        this.EventData.TOF_points.forEach(function (point) {
            //console.log(hit.id);
            var geometry = new THREE.Geometry();
            geometry.name = "TOFPoint_" + point.id;

            var material = new THREE.MeshBasicMaterial({color:0xffffff, transparent:true, opacity:.8});
            var tofpoint=new THREE.SphereGeometry(2,32,32,0,6.3,0,6.3);

            var pointmesh= new THREE.Mesh(tofpoint,material);
            pointmesh.position.x=point.x;
            pointmesh.position.y=point.y;
            pointmesh.position.z=point.z;//+40 May not be needed.....

            //scene.add(pointmesh);

            //setRGB(track_color.r,track_color.g,track_color.b);
            //console.log(track_charge);

            pointmesh.name = geometry.name;
            scope.group.add(pointmesh);
            //console.log(track.charge);
        });


       var tofMesh = scope.geometry.getObjectByName("FTOF");

       var TOFReferenceColor= tofMesh.getObjectByName("TOF_p1_m1",true).material.color;

        this.EventData.TOF_hits.forEach(function (hit) {
            //get the object to change and change it
            //console.log("hit id:"+hit.id);
            var plane=hit.plane;
            var bar=hit.bar;
            var end = hit.end;

            var block="FTOB";

            var geoName="TOF_p" + plane + "_m"+(bar-1);

            if((bar-1===21 || bar-1===22) && end===0)
            {
                geoName+="_r";
            }

            //var object = scope.geometry;
            //console.log("Found object="+object);
            //console.log(plane+","+bar+","+end);
            //console.log(geoName);

            var object = tofMesh.getObjectByName(geoName,true);
            //getObjectByName( "TestBox", true );


            if(object)
            {
                if(object.material.color === TOFReferenceColor)
                {
                    object.material.color.setRGB(1,1,0);
                }
               else
               {
                   object.material.color.setRGB(1,object.material.color.g/2.,0);

               }

            }
            else {
                console.log("DIDN'T FIND " + geoName);
            }//console.log(object);



            //console.log(object.geometry.faces.length);

            /*(var rcol=Math.random();
            var gcol=Math.random();
            var bcol=Math.random();

            for ( var i = 0; i < object.geometry.faces.length; i ++ ) {
                if(i%2==0) {
                    rcol=Math.random();
                    gcol=Math.random();
                    bcol=Math.random();
                }
                object.geometry.faces[ i ].color.setRGB( rcol,gcol,bcol );
            }
            object.geometry.colorsNeedUpdate = true*/


        });

        return this.group;
    },



};
