/**
 * Created by tbritton on 6/27/17.
 */

var HDVisConfig = function() {

    this.ftofDetailOptions = {
        "Box only": 'box',
        "Sections": 'sections',
        "Modules":  'modules'
    };

    this.ftofDetail = 'modules'; // "box", "sections", "modules"

    this.positive_tracks=true;
    this.negative_tracks=true;
    this.neutral_tracks=true;
    // Thomas, here is GUI examples:
    // http://workshop.chromeexperiments.com/examples/gui/#1--Basic-Usage
};


function makeGUI(){
    var config = new HDVisConfig();
    var gui = new dat.GUI({load: JSON});
    gui.remember(config);


    gui.addFolder('CDC');

    var tofGuiFolder = gui.addFolder('TOF');
    gui.addFolder('FCAL');

    tofGuiFolder
        .add( config, 'ftofDetail', config.ftofDetailOptions )
        .name('Detalization')
        .onFinishChange(function(value) {
            var ftof = scene.getObjectByName("FTOF", true);

            var tofDetails = function(isBoxVisible, isSectionsVisible, isModulesVisible){

                ftof.children.forEach(function (plane) {
                    plane.material.visible = isBoxVisible;
                    plane.children.forEach(function (region) {
                        region.material.visible = isSectionsVisible;
                        region.children.forEach(function (module) {
                            module.material.visible = isModulesVisible;
                        });
                    });
                });
            };

            switch(value)
            {
                case 'box':
                    tofDetails(true, false, false);
                    break;
                case 'sections':
                    tofDetails(false, true, false);
                    break;
                case 'modules':
                    tofDetails(false, false, true);
                    break;
            }
            // Fires when a controller loses focus.
            //alert("The new value is " + value);
        });

    var Trackinggui= gui.addFolder('Tracking');

    Trackinggui.add(config, 'positive_tracks', config.positive_tracks).name('Show Positive Tracks')
        .onFinishChange(function(value) {
            var eventobjs = scene.getObjectByName("GluexEvent").children;
            gui_TrackVis(eventobjs,1,value);
        });

    Trackinggui.add(config, 'negative_tracks', config.negative_tracks).name('Show Negative Tracks')
        .onFinishChange(function(value) {
            var eventobjs = scene.getObjectByName("GluexEvent").children;
            gui_TrackVis(eventobjs,-1,value);
        });

    Trackinggui.add(config, 'neutral_tracks', config.neutral_tracks).name('Show Neutral Tracks')
        .onFinishChange(function(value) {
            var eventobjs = scene.getObjectByName("GluexEvent").children;
            gui_TrackVis(eventobjs,0,value);
        });

        gui.open();
}


function gui_TrackVis(eventobjs,Trackq,isVis) {

    for(var i=0;i<eventobjs.length;i++)
    {

        if(eventobjs[i].name.split('_')[0]==="track" && eventobjs[i].userData.charge===Trackq)
        {
            eventobjs[i].material.visible=isVis;
        }
    }
}