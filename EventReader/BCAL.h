//
// Created by tbritton on 6/20/17.
//

#ifndef EVESTANDALONE_BCAL_H
#define EVESTANDALONE_BCAL_H

#include <BCAL/DBCALHit.h>
#include <BCAL/DBCALShower.h>

class BCAL
{
public:

    static tao::json::value Add_BCALHits(vector<const DBCALHit*> BCALHits)
    {
        auto arr = tao::json::value::array({});

        for(uint i=0;i<BCALHits.size();i++)
        {
            arr.emplace_back(WriteHitJSON(i, BCALHits[i]->module, BCALHits[i]->layer, BCALHits[i]->sector, BCALHits[i]->end, BCALHits[i]->pulse_peak, BCALHits[i]->E, BCALHits[i]->t, BCALHits[i]->t_raw, BCALHits[i]->cellId));

        }
        return arr;
    }

    static tao::json::value WriteHitJSON(int id, int module, int layer, int sector, DBCALGeometry::End end, int pulse_peak, float E, float t, float t_raw, int cellId)
    {
        return tao::json::value({
                                        {"id", id},
                                        {"module", module},
                                        {"layer", layer},
                                        {"sector", sector},
                                        //{"end", end},
                                        {"pulse_peak", pulse_peak},
                                        {"E", E},
                                        {"t", t},
                                        {"t_raw", t_raw},
                                        {"cellId", cellId}
                                });
    }

};
#endif //EVESTANDALONE_BCAL_H
