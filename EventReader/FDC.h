//
// Created by tbritton on 6/20/17.
//

#ifndef EVESTANDALONE_FDC_H
#define EVESTANDALONE_FDC_H

#include <FDC/DFDCHit.h>

class FDC
{
public:

    void Add_FDCHits(vector<const DFDCHit*> FDCHits)
    {

        ofstream event_out;
        event_out.open("event.json", ios::app);//JSON
        event_out<<"\"FDC_hits\": "<<"[\n";//JSON

        for(uint i=0;i<FDCHits.size();i++)
        {
            WriteHitJSON(event_out, i, FDCHits[i]->layer, FDCHits[i]->module, FDCHits[i]->element, FDCHits[i]->plane, FDCHits[i]->gPlane, FDCHits[i]->gLayer, FDCHits[i]->q, FDCHits[i]->pulse_height, FDCHits[i]->pulse_height_raw, FDCHits[i]->t, FDCHits[i]->r, FDCHits[i]->d, FDCHits[i]->type);
            if(i!=FDCHits.size()-1)
                event_out<<","<<endl;

        }

        event_out<<"]"<<endl;
        event_out.close();

    }

    void WriteHitJSON(ofstream& event_out, int id, int layer, int module, int element, int plane, int gPlane, int gLayer, float q, float pulse_height, float pulse_height_raw, float t, float r, float d, int type)
    {
        tao::json::value FDCHit({
                                        {"id", id},
                                        {"layer", layer},
                                        {"module", module},
                                        {"element", element},
                                        {"plane", plane},
                                        {"gPlane", gPlane},
                                        {"gLayer", gLayer},
                                        {"q", q},
                                        {"pulse_height", pulse_height},
                                        {"pulse_height_raw", pulse_height_raw},
                                        {"t", t},
                                        {"r", r},
                                        {"d", d},
                                        {"type", type}

                                });

        event_out << tao::json::to_string(FDCHit, 4);
    }

};

#endif //EVESTANDALONE_FDC_H
