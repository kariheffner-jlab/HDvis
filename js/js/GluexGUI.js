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
    this.bkg_color = 0x000000;

    this.FCAL_EScale = 100.;
    this.TrackingChiSq_NDF_cut = 0.;

    // Thomas, here is GUI examples:
    // http://workshop.chromeexperiments.com/examples/gui/#1--Basic-Usage
};


function makeGUI(){
    var config = new HDVisConfig();
    var gui = new dat.GUI({load: JSON});
    gui.remember(config);

    gui.addColor(config, 'bkg_color', config.bkg_color).name('Bkg Color')
        .onChange(function(chosenColor) {
            renderer.setClearColor(chosenColor);
            //  renderer.clearColor.setRGB(chosenColor[0]/255.,chosenColor[1]/255.,chosenColor[2]/255.);
        });

    gui.addFolder('CDC');

    var tofGuiFolder = gui.addFolder('TOF');

    var fcalGuiFolder= gui.addFolder('FCAL');
    fcalGuiFolder.add(config, 'FCAL_EScale', 1, 1000).name("Energy Scale").onChange(function(value) { this.FCAL_EScale=value;
        var eventobjs = scene.getObjectByName("GluexEvent").children;
        for(var i=0;i<eventobjs.length;i++) {
            if (eventobjs[i].name.split('_')[0] === "FCALHit") {
                eventobjs[i].position.z=(660+(this.FCAL_EScale/2)*eventobjs[i].userData.E);
                eventobjs[i].scale.z=this.FCAL_EScale/100;
                eventobjs[i].geometry.elementsNeedUpdate=true;
                eventobjs[i].geometry.verticesNeedUpdate=true;
            }
        }
    });

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

    var positiveTrackgui=Trackinggui.addFolder('Positive Tracks');
    var negativeTrackgui=Trackinggui.addFolder('Negative Tracks');
    var neutralTrackgui=Trackinggui.addFolder('Neutral Tracks');

    positiveTrackgui.add(config, 'positive_tracks', config.positive_tracks).name('Show Swim Points')
        .onFinishChange(function(value) {
            var eventobjs = scene.getObjectByName("GluexEvent").children;
            gui_TrackVis(eventobjs,1,value);
        });

    negativeTrackgui.add(config, 'negative_tracks', config.negative_tracks).name('Show Swim Points')
        .onFinishChange(function(value) {
            var eventobjs = scene.getObjectByName("GluexEvent").children;
            gui_TrackVis(eventobjs,-1,value);
        });

    neutralTrackgui.add(config, 'neutral_tracks', config.neutral_tracks).name('Show Swim Points')
        .onFinishChange(function(value) {
            var eventobjs = scene.getObjectByName("GluexEvent").children;
            gui_TrackVis(eventobjs,0,value);
        });

    Trackinggui.add(config, 'TrackingChiSq_NDF_cut', 0, 10).name("ChiSq/NDF cut").onChange(function(value) { this.TrackingChiSq_NDF_cut=value;
        var eventobjs = scene.getObjectByName("GluexEvent").children;
        for(var i=0;i<eventobjs.length;i++) {
            if(eventobjs[i].name.split('_')[0]==="track")
            {
                var tracks_chi=eventobjs[i].userData["TrackChiSq_NDF"];
                if(this.TrackingChiSq_NDF_cut !== 0)
                {
                    if(tracks_chi>this.TrackingChiSq_NDF_cut)
                    {
                        eventobjs[i].material.visible = false;
                        eventobjs[i].children[0].material.visible=false;
                    }
                    else
                    {
                        if((config.positive_tracks===true && eventobjs[i].userData.charge===1) || (config.negative_tracks===true && eventobjs[i].userData.charge===-1) || (config.neutral_tracks===true && eventobjs[i].userData.charge===0)) {
                            eventobjs[i].material.visible = true;
                            eventobjs[i].children[0].material.visible=true;
                        }
                    }
                }
                else {
                    if((config.positive_tracks===true && eventobjs[i].userData.charge===1) || (config.negative_tracks===true && eventobjs[i].userData.charge===-1) || (config.neutral_tracks===true && eventobjs[i].userData.charge===0))
                    {
                        eventobjs[i].material.visible = true;
                        eventobjs[i].children[0].material.visible=true;
                    }
                }
            }
        }
    });

    positiveTrackgui.addColor(config, 'positive_track_color', config.positive_track_color).name('Positive Color')
        .onChange(function(colorChosen) {
            var eventobjs = scene.getObjectByName("GluexEvent").children;
            gui_TrackColor(eventobjs,1,colorChosen);
        });

    negativeTrackgui.addColor(config, 'negative_track_color', config.negative_track_color).name('Negative Color')
        .onChange(function(colorChosen) {
            var eventobjs = scene.getObjectByName("GluexEvent").children;
            gui_TrackColor(eventobjs,-1,colorChosen);
        });

    neutralTrackgui.addColor(config, 'neutral_track_color', config.neutral_track_color).name('Neutral Color')
        .onChange(function(colorChosen) {
            var eventobjs = scene.getObjectByName("GluexEvent").children;
            gui_TrackColor(eventobjs,0,colorChosen);
        });

        gui.open();
        return config;
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
            eventobjs[i].children[0].material.color.setRGB(colorChosen[0]/255.,colorChosen[1]/255.,colorChosen[2]/255.);
            eventobjs[i].children[0].geometry.colorsNeedUpdate = true;
            eventobjs[i].material.color.setRGB(colorChosen[0]/255.,colorChosen[1]/255.,colorChosen[2]/255.);
            eventobjs[i].geometry.colorsNeedUpdate = true;

        }
    }

}