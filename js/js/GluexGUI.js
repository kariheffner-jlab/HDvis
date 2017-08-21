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

    this.CDCHit_Options ={"Off": 'Off',"Static": 'Static', "Dynamic": 'Dynamic'};
    this.CDCHitVis= 'Dynamic';

    this.FDCHit_Options ={"Off": 'Off',"Static": 'Static', "Dynamic": 'Dynamic'};
    this.FDCHitVis= 'Dynamic';

    this.FDCHitType_Options ={"Both": 'Both',"Cathodes": 'Cathodes', "Anodes": 'Anodes'};
    this.FDCHitTypeVis= 'Anodes';

    this.TOFPoint_Options ={"Off": 'Off',"Static": 'Static', "Dynamic": 'Dynamic'};
    this.TOFPointVis= 'Dynamic';

    this.BCALPoint_Options ={"Off": 'Off',"Static": 'Static', "Dynamic": 'Dynamic'};
    this.BCALPointVis= 'Dynamic';

    this.BCALShower_Options ={"Off": 'Off',"Static": 'Static', "Dynamic": 'Dynamic'};
    this.BCALShowerVis= 'Dynamic';

    this.FCALHit_Options ={"Off": 'Off',"Static": 'Static', "Dynamic": 'Dynamic'};
    this.FCALHitVis= 'Dynamic';

    this.FCALShower_Options ={"Off": 'Off',"Static": 'Static', "Dynamic": 'Dynamic'};
    this.FCALShowerVis= 'Dynamic';

    this.FDCPseudo_Options ={"Off": 'Off',"Static": 'Static', "Dynamic": 'Dynamic'};
    this.FDCPseudoVis= 'Dynamic';

    this.positive_track_swim=false;
    this.negative_track_swim=false;
    this.neutral_track_swim=false;
    this.positive_track_color = [255, 0, 0]; // RGB array
    this.negative_track_color = [0, 255, 0]; // RGB array
    this.neutral_track_color = [255, 255, 0]; // RGB array
    this.bkg_color = 0x000000;
    this.time_scale=5; // ns/sec

    this.FCAL_EScale = 100.;
    this.TrackingChiSq_NDF_cut = 0.;
    this.FCAL_HitEcut = 0.;
    this.FCAL_ShowerEcut = 0.;
    this.TimingsNeedsUpdate=true;

    this.min_clock_time=-100;
    this.max_clock_time=100;

    this.BCAL_ShowerEcut = 0.;
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
    gui.add(config, 'min_clock_time', -1500, 0).name("Min. Clock (RF)").onChange(function(value) { this.min_clock_time=value;config.TimingsNeedsUpdate=true;});
    gui.add(config, 'max_clock_time', 0,1500).name("Max. Clock (RF)").onChange(function(value) { this.max_clock_time=value;config.TimingsNeedsUpdate=true;});

    var cdcGuiFolder=gui.addFolder('CDC');

    var bcalGuiFolder = gui.addFolder('BCAL');

    var fdcGuiFolder = gui.addFolder('FDC');

    var tofGuiFolder = gui.addFolder('TOF');

    var fcalGuiFolder= gui.addFolder('FCAL');

    cdcGuiFolder.add( config, 'CDCHitVis', config.CDCHit_Options )
        .name('CDC Hits').onFinishChange(function(value) { this.CDCHitVis=value;
        if(value==="Off") {
            var eventobjs = scene.getObjectByName("CDC").children[0].children;
            for (var i = 0; i < eventobjs.length; i++) {
                if (eventobjs[i].name.split('_')[0] === "CDCstraw") {
                    eventobjs[i].material.visible = 0;
                }
            }
        }

        config.TimingsNeedsUpdate=true;
    });

    fdcGuiFolder.add( config, 'FDCHitTypeVis', config.FDCHitType_Options )
        .name('FDC Hits to Show').onFinishChange(function(value) { this.FDCHitTypeVis=value;
        if(value==="Cathodes") {
            var eventobjs = scene.getObjectByName("GluexEvent").children;
            for (var i = 0; i < eventobjs.length; i++) {
                if (eventobjs[i].name.split('_')[0] === "FDCHit") {
                    if(eventobjs[i].userData.type===0) {
                        eventobjs[i].material.visible = 0;
                    }
                    else
                    {
                        if( config.FDCHitVis !=="Off")
                            eventobjs[i].material.visible = 1;
                    }
                }
            }
        }
        else if(value==="Anodes") {
            var eventobjs = scene.getObjectByName("GluexEvent").children;
            for (var i = 0; i < eventobjs.length; i++) {
                if (eventobjs[i].name.split('_')[0] === "FDCHit") {
                    if(eventobjs[i].userData.type!==0) {
                        eventobjs[i].material.visible = 0;
                    }
                    else
                    {
                        if( config.FDCHitVis !=="Off")
                            eventobjs[i].material.visible = 1;
                    }
                }
            }
        }
        else
        {
            var eventobjs = scene.getObjectByName("GluexEvent").children;
            for (var i = 0; i < eventobjs.length; i++) {
                if (eventobjs[i].name.split('_')[0] === "FDCHit" && eventobjs[i].material.visible===0 && config.FDCHitVis !=="Off") {
                    eventobjs[i].material.visible = 1;
                }
            }

        }
        config.TimingsNeedsUpdate=true;
    });

    fdcGuiFolder.add( config, 'FDCHitVis', config.FDCHit_Options )
        .name('FDC Hits').onFinishChange(function(value) { this.FDCHitVis=value;
        if(value==="Off") {
            var eventobjs = scene.getObjectByName("GluexEvent").children;
            for (var i = 0; i < eventobjs.length; i++) {
                if (eventobjs[i].name.split('_')[0] === "FDCHit") {
                    eventobjs[i].material.visible = 0;
                }
            }
        }
        else
        {
            var eventobjs = scene.getObjectByName("GluexEvent").children;
            for (var i = 0; i < eventobjs.length; i++) {
                if (eventobjs[i].name.split('_')[0] === "FDCHit" && eventobjs[i].material.visible===0) {
                    eventobjs[i].material.visible = 1;
                }
            }

        }
        config.TimingsNeedsUpdate=true;
    });

    fdcGuiFolder.add( config, 'FDCPseudoVis', config.FDCPseudo_Options )
        .name('FDC Pseudos').onFinishChange(function(value) { this.FDCPseudoVis=value;
        if(value==="Off") {
            var eventobjs = scene.getObjectByName("GluexEvent").children;
            for (var i = 0; i < eventobjs.length; i++) {
                if (eventobjs[i].name.split('_')[0] === "FDCPseudo") {
                    eventobjs[i].material.visible = 0;
                }
            }
        }
        else
        {
            var eventobjs = scene.getObjectByName("GluexEvent").children;
            for (var i = 0; i < eventobjs.length; i++) {
                if (eventobjs[i].name.split('_')[0] === "FDCPseudo" && eventobjs[i].material.visible===0) {
                    eventobjs[i].material.visible = 1;
                }
            }

        }
        config.TimingsNeedsUpdate=true;
    });

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
    fcalGuiFolder.add(config, 'FCAL_HitEcut',0, 3).name("Hit Energy cut (GeV)").onChange(function(value) { this.FCAL_HitEcut=value;
        var eventobjs = scene.getObjectByName("GluexEvent").children;
        for(var i=0;i<eventobjs.length;i++) {
            if (eventobjs[i].name.split('_')[0] === "FCALHit") {
                if(eventobjs[i].userData.E<this.FCAL_HitEcut)
                {
                    eventobjs[i].material.visible=0;
                }
                else
                {
                    eventobjs[i].material.visible=1;
                }
            }
        }
    });
    fcalGuiFolder.add(config, 'FCAL_ShowerEcut',0, 6).name("Shower Energy cut (GeV)").onChange(function(value) { this.FCAL_ShowerEcut=value;
        var eventobjs = scene.getObjectByName("GluexEvent").children;
        for(var i=0;i<eventobjs.length;i++) {
            if (eventobjs[i].name.split('_')[0] === "FCALShower") {
                if(eventobjs[i].userData.fEnergy<this.FCAL_ShowerEcut)
                {
                    eventobjs[i].material.visible=0;
                }
                else
                {
                    eventobjs[i].material.visible=1;
                }
            }
        }
    });

    fcalGuiFolder.add( config, 'FCALHitVis', config.FCALHit_Options )
        .name('FCAL Hits').onFinishChange(function(value) {
        this.FCALHitVis=value;
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
        this.FCALShowerVis=value;
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
        this.TOFHitVis=value;
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
        config.TOFPointVis=value;
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

    bcalGuiFolder.add(config, 'BCAL_ShowerEcut',0, 2).name("Shower Energy cut (GeV)").onChange(function(value) { this.BCAL_ShowerEcut=value;
        var eventobjs = scene.getObjectByName("GluexEvent").children;
        for(var i=0;i<eventobjs.length;i++) {
            if (eventobjs[i].name.split('_')[0] === "BCALShower") {
                if(eventobjs[i].userData.E<this.BCAL_ShowerEcut)
                {
                    eventobjs[i].material.visible=0;
                }
                else
                {
                    eventobjs[i].material.visible=1;
                }
            }
        }
    });

    bcalGuiFolder.add( config, 'BCALPointVis', config.BCALPoint_Options )
        .name('BCAL Points').onFinishChange(function(value) {
        config.BCALPointVis=value;
        if(value==="Off") {
            var eventobjs = scene.getObjectByName("GluexEvent").children;
            for (var i = 0; i < eventobjs.length; i++) {
                if (eventobjs[i].name.split('_')[0] === "BCALPoint") {
                    eventobjs[i].material.visible = 0;
                }
            }
        }
        else
        {
            var eventobjs = scene.getObjectByName("GluexEvent").children;
            for (var i = 0; i < eventobjs.length; i++) {
                if (eventobjs[i].name.split('_')[0] === "BCALPoint" && eventobjs[i].material.visible===0) {
                    eventobjs[i].material.visible = 1;
                }
            }

        }
        config.TimingsNeedsUpdate=true;
    });

    bcalGuiFolder.add( config, 'BCALShowerVis', config.BCALShower_Options )
        .name('BCAL Showers').onFinishChange(function(value) {
        this.BCALShowerVis=value;
        if(value==="Off") {
            var eventobjs = scene.getObjectByName("GluexEvent").children;
            for (var i = 0; i < eventobjs.length; i++) {
                if (eventobjs[i].name.split('_')[0] === "BCALShower") {
                    eventobjs[i].material.visible = 0;
                }
            }
        }
        else
        {
            var eventobjs = scene.getObjectByName("GluexEvent").children;
            for (var i = 0; i < eventobjs.length; i++) {
                if (eventobjs[i].name.split('_')[0] === "BCALShower" && eventobjs[i].material.visible===0) {
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

    positiveTrackgui.add(config, 'positive_track_swim', config.positive_track_swim).name('Show Swim Points')
        .onFinishChange(function(value) {
            var eventobjs = scene.getObjectByName("GluexEvent").children;
            var value_conv="Off";
            if(value==true)
            {
                value_conv="On";
            }
            gui_TrackVis(eventobjs,1,value_conv,config);

        });

    positiveTrackgui.add( config, 'positive_track_line', config.positive_track_lineOptions )
        .name('Track Lines')
        .onFinishChange(function(value) {
            //console.log(value);
            config.TimingsNeedsUpdate=true;
        });

    negativeTrackgui.add(config, 'negative_track_swim', config.negative_track_swim).name('Show Swim Points')
        .onFinishChange(function(value) {
            var eventobjs = scene.getObjectByName("GluexEvent").children;
            var value_conv="Off";
            if(value==true)
            {
                value_conv="On";
            }
            gui_TrackVis(eventobjs,-1,value_conv,config);

        });

    negativeTrackgui.add( config, 'negative_track_line', config.negative_track_lineOptions )
        .name('Track Lines')
        .onFinishChange(function(value) {
            //console.log(value);
            config.TimingsNeedsUpdate=true;
        });

    neutralTrackgui.add(config, 'neutral_track_swim', config.neutral_track_swim).name('Show Swim Points')
        .onFinishChange(function(value) {
            var eventobjs = scene.getObjectByName("GluexEvent").children;
            var value_conv="Off";
            if(value==true)
            {
                value_conv="On";
            }
            gui_TrackVis(eventobjs,0,value_conv,config);
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
                        if((config.positive_track_swim===true && eventobjs[i].userData.charge===1) || (config.negative_track_swim===true && eventobjs[i].userData.charge===-1) || (config.neutral_track_swim===true && eventobjs[i].userData.charge===0)) {
                            eventobjs[i].material.visible = true;
                            eventobjs[i].children[0].material.visible=true;
                        }
                    }
                }
                else {
                    if((config.positive_track_swim===true && eventobjs[i].userData.charge===1) || (config.negative_track_swim===true && eventobjs[i].userData.charge===-1) || (config.neutral_track_swim===true && eventobjs[i].userData.charge===0))
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


function gui_TrackVis(eventobjs,Trackq,isVis,config) {

    var vis=false;
    if(isVis !== "Off")
    {
        vis=true;
    }

    for(var i=0;i<eventobjs.length;i++)
    {

        if(eventobjs[i].name.split('_')[0]==="track" && eventobjs[i].userData.charge===Trackq )
        {
            eventobjs[i].material.visible=vis;
        }
        if(eventobjs[i].userData.TrackChiSq_NDF >= config.TrackingChiSq_NDF_cut && config.TrackingChiSq_NDF_cut !==0 && Trackq!==0)
        {
            eventobjs[i].material.visible=false;
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
