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

    this.positive_track_lineOptions ={"Off": 'Off',"Static": 'Static', "Dynamic": 'Dynamic'};
    this.positive_track_line= 'Dynamic';

    this.negative_track_lineOptions ={"Off": 'Off',"Static": 'Static', "Dynamic": 'Dynamic'};
    this.negative_track_line= 'Dynamic';

    this.neutral_track_lineOptions ={"Off": 'Off',"Static": 'Static', "Dynamic": 'Dynamic'};
    this.neutral_track_line= 'Dynamic';

    this.TOFHit_Options ={"Off": 'Off',"Static": 'Static', "Dynamic": 'Dynamic'};
    this.TOFHitVis= 'Dynamic';

    this.TOFPoint_Options ={"Off": 'Off',"Static": 'Static', "Dynamic": 'Dynamic'};
    this.TOFPointVis= 'Dynamic';

    this.FCALHit_Options ={"Off": 'Off',"Static": 'Static', "Dynamic": 'Dynamic'};
    this.FCALHitVis= 'Dynamic';

    this.FCALShower_Options ={"Off": 'Off',"Static": 'Static', "Dynamic": 'Dynamic'};
    this.FCALShowerVis= 'Dynamic';

    this.positive_tracks=false;
    this.negative_tracks=false;
    this.neutral_tracks=false;
    this.positive_track_color = [255, 0, 0]; // RGB array
    this.negative_track_color = [0, 255, 0]; // RGB array
    this.neutral_track_color = [255, 255, 0]; // RGB array
    this.bkg_color = 0x000000;
    this.time_scale=5; // ns/sec

    this.FCAL_EScale = 100.;
    this.TrackingChiSq_NDF_cut = 0.;
    this.TimingsNeedsUpdate=true;
    //this.SceneTimeMessage=""

    // Thomas, here is GUI examples:
    // http://workshop.chromeexperiments.com/examples/gui/#1--Basic-Usage
};


function makeGUI(scene){
    var config = new HDVisConfig();
    var gui = new dat.GUI({load: JSON});
    gui.remember(config);

    gui.addColor(config, 'bkg_color', config.bkg_color).name('Bkg Color')
        .onChange(function(chosenColor) {
            renderer.setClearColor(chosenColor);
            //  renderer.clearColor.setRGB(chosenColor[0]/255.,chosenColor[1]/255.,chosenColor[2]/255.);
        });

    //gui.add(config, 'SceneTimeMessage').name("Event Time (ns)").listen();

    gui.add(config, 'time_scale', .1, 20).name("Time Scale (ns/s)").onChange(function(value) { this.time_scale=value;});

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

    fcalGuiFolder.add( config, 'FCALHitVis', config.FCALHit_Options )
        .name('FCAL Hits').onFinishChange(function(value) {
        if(value==="Off") {
            var eventobjs = scene.getObjectByName("GluexEvent").children;
            for (var i = 0; i < eventobjs.length; i++) {
                if (eventobjs[i].name.split('_')[0] === "FCALHit") {
                    eventobjs[i].material.visible = 0;
                }
            }
        }
        else
        {
            var eventobjs = scene.getObjectByName("GluexEvent").children;
            for (var i = 0; i < eventobjs.length; i++) {
                if (eventobjs[i].name.split('_')[0] === "FCALHit" && eventobjs[i].material.visible===0) {
                    eventobjs[i].material.visible = 1;
                }
            }

        }
        config.TimingsNeedsUpdate=true;
    });
    fcalGuiFolder.add( config, 'FCALShowerVis', config.FCALShower_Options )
        .name('FCAL Showers').onFinishChange(function(value) {
        if(value==="Off") {
            var eventobjs = scene.getObjectByName("GluexEvent").children;
            for (var i = 0; i < eventobjs.length; i++) {
                if (eventobjs[i].name.split('_')[0] === "FCALShower") {
                    eventobjs[i].material.visible = 0;
                }
            }
        }
        else
        {
            var eventobjs = scene.getObjectByName("GluexEvent").children;
            for (var i = 0; i < eventobjs.length; i++) {
                if (eventobjs[i].name.split('_')[0] === "FCALShower" && eventobjs[i].material.visible===0) {
                    eventobjs[i].material.visible = 1;
                }
            }

        }
        config.TimingsNeedsUpdate=true;
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

    tofGuiFolder.add( config, 'TOFHitVis', config.TOFHit_Options )
        .name('TOF Hits').onFinishChange(function(value) {
        if(value==="Off") {
            var geoobjs = scene.getObjectByName("FTOF",true);

            for (var plane = 0; plane < geoobjs.children.length; plane++) {
                for (var block = 0; block < geoobjs.children[plane].children.length; block++) {
                    for (var bar = 0; bar < geoobjs.children[plane].children[block].children.length; bar++) {

                        for (var i = 0; i < geoobjs.children[plane].children[block].children[bar].geometry.faces.length; i++) {
                            if(geoobjs.children[plane].children[block].children[bar].geometry.faces[i].color.r!==1 ||
                                geoobjs.children[plane].children[block].children[bar].geometry.faces[i].color.g!==1 ||
                                geoobjs.children[plane].children[block].children[bar].geometry.faces[i].color.b!==1)
                            {
                                geoobjs.children[plane].children[block].children[bar].geometry.faces[i].color.r=1;
                                geoobjs.children[plane].children[block].children[bar].geometry.faces[i].color.g=1;
                                geoobjs.children[plane].children[block].children[bar].geometry.faces[i].color.b=1;
                                geoobjs.children[plane].children[block].children[bar].geometry.colorsNeedUpdate = true;
                            }
                        }

                    }
                }
            }
        }
        config.TimingsNeedsUpdate=true;
    });

    tofGuiFolder.add( config, 'TOFPointVis', config.TOFPoint_Options )
        .name('TOF Points').onFinishChange(function(value) {
        if(value==="Off") {
            var eventobjs = scene.getObjectByName("GluexEvent").children;
            for (var i = 0; i < eventobjs.length; i++) {
                if (eventobjs[i].name.split('_')[0] === "TOFPoint") {
                    eventobjs[i].material.visible = 0;
                }
            }
        }
        else
        {
            var eventobjs = scene.getObjectByName("GluexEvent").children;
            for (var i = 0; i < eventobjs.length; i++) {
                if (eventobjs[i].name.split('_')[0] === "TOFPoint" && eventobjs[i].material.visible===0) {
                    eventobjs[i].material.visible = 1;
                }
            }

        }
        config.TimingsNeedsUpdate=true;
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

    positiveTrackgui.add( config, 'positive_track_line', config.positive_track_lineOptions )
        .name('Track Lines')
        .onFinishChange(function(value) {
            //console.log(value);
            config.TimingsNeedsUpdate=true;
        });

    negativeTrackgui.add(config, 'negative_tracks', config.negative_tracks).name('Show Swim Points')
        .onFinishChange(function(value) {
            var eventobjs = scene.getObjectByName("GluexEvent").children;
            gui_TrackVis(eventobjs,-1,value);

        });

    negativeTrackgui.add( config, 'negative_track_line', config.negative_track_lineOptions )
        .name('Track Lines')
        .onFinishChange(function(value) {
            //console.log(value);
            config.TimingsNeedsUpdate=true;
        });

    neutralTrackgui.add(config, 'neutral_tracks', config.neutral_tracks).name('Show Swim Points')
        .onFinishChange(function(value) {
            var eventobjs = scene.getObjectByName("GluexEvent").children;
            gui_TrackVis(eventobjs,0,value);
        });

    neutralTrackgui.add( config, 'neutral_track_line', config.neutral_track_lineOptions )
        .name('Track Lines')
        .onFinishChange(function(value) {
            // console.log(value);
            config.TimingsNeedsUpdate=true;
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
