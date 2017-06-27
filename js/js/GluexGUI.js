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
    this.positive_track_color = [255, 0, 0]; // RGB array
    this.negative_track_color = [0, 255, 0]; // RGB array
    this.neutral_track_color = [255, 255, 0]; // RGB array

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

    Trackinggui.addColor(config, 'positive_track_color', config.positive_track_color).name('Positive Color')
        .onChange(function(colorChosen) {
            var eventobjs = scene.getObjectByName("GluexEvent").children;
            gui_TrackColor(eventobjs,1,colorChosen);
        });

    Trackinggui.addColor(config, 'negative_track_color', config.negative_track_color).name('Negative Color')
        .onChange(function(colorChosen) {
            var eventobjs = scene.getObjectByName("GluexEvent").children;
            gui_TrackColor(eventobjs,-1,colorChosen);
        });

    Trackinggui.addColor(config, 'neutral_track_color', config.neutral_track_color).name('Neutral Color')
        .onChange(function(colorChosen) {
            var eventobjs = scene.getObjectByName("GluexEvent").children;
            gui_TrackColor(eventobjs,0,colorChosen);
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

function gui_TrackColor(eventobjs,Trackq,colorChosen) {

    for(var i=0;i<eventobjs.length;i++)
    {

        if(eventobjs[i].name.split('_')[0]==="track" && eventobjs[i].userData.charge===Trackq)
        {
            eventobjs[i].material.color.setRGB(colorChosen[0]/255.,colorChosen[1]/255.,colorChosen[2]/255.);
            eventobjs[i].geometry.colorsNeedUpdate = true;

        }
    }
}