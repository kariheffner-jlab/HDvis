THREE.GluexEventLoader = function () {
    this.EventData = null;
    this.materials = {
        posTrack: new THREE.PointsMaterial( {
            color: 0x0000ff,
            size: 4,
            opacity:0.6,
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

        // Iterate tracks
        this.EventData.tracks.forEach(function (track) {

            var geometry = new THREE.Geometry();
            geometry.name = "track_" + track.id +"_" + track.charge;

            track.points.forEach(function (point) {
                var vertex = new THREE.Vector3();
                vertex.x = point[0];
                vertex.y = point[1];
                vertex.z = point[2];
                geometry.vertices.push( vertex );
                });

            var trackMesh = new THREE.Points( geometry, scope.materials.posTrack );
            trackMesh.name = geometry.name;
            scope.group.add(trackMesh);
            //console.log(track.charge);
        });
        return this.group;
    },

};
