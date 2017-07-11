//
// Created by tbritton on 6/20/17.
//

#ifndef EVESTANDALONE_FDC_H
#define EVESTANDALONE_FDC_H

#include <FDC/DFDCHit.h>

class FDC
{
public:

    static tao::json::value Add_FDCHits(vector<const DFDCHit*> FDCHits)
    {
        auto arr = tao::json::value::array({});

        for(uint i=0;i<FDCHits.size();i++)
        {
            arr.emplace_back(WriteHitJSON(i, FDCHits[i]->layer, FDCHits[i]->module, FDCHits[i]->element, FDCHits[i]->plane, FDCHits[i]->gPlane, FDCHits[i]->gLayer, FDCHits[i]->q, FDCHits[i]->pulse_height, FDCHits[i]->pulse_height_raw, FDCHits[i]->t, FDCHits[i]->r, FDCHits[i]->d, FDCHits[i]->type));
        }

        return arr;
    }

    static tao::json::value WriteHitJSON(int id, int layer, int module, int element, int plane, int gPlane, int gLayer, float q, float pulse_height, float pulse_height_raw, float t, float r, float d, int type)
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

        return FDCHit;//event_out << tao::json::to_string(FDCHit, 4);
    }

};

#endif //EVESTANDALONE_FDC_H
