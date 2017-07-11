//
// Created by tbritton on 6/20/17.
//

#ifndef EVESTANDALONE_BCAL_H
#define EVESTANDALONE_BCAL_H

#include <BCAL/DBCALHit.h>

class BCAL
{
public:

    string Add_BCALHits(vector<const DBCALHit*> BCALHits)
    {

        std::ostringstream event_out;

        event_out<<"\"BCAL_hits\": "<<"[\n";//JSON

        for(uint i=0;i<BCALHits.size();i++)
        {
            event_out<<WriteHitJSON(i, BCALHits[i]->module, BCALHits[i]->layer, BCALHits[i]->sector, BCALHits[i]->end, BCALHits[i]->pulse_peak, BCALHits[i]->E, BCALHits[i]->t, BCALHits[i]->t_raw, BCALHits[i]->cellId);
                if(i!=BCALHits.size()-1)
                    event_out<<","<<endl;

        }

        event_out<<"]"<<endl;
        string outstr = event_out.str();
        return outstr;

    }

    tao::json::value WriteHitJSON(int id, int module, int layer, int sector, DBCALGeometry::End end, int pulse_peak, float E, float t, float t_raw, int cellId)
    {
        tao::json::value BCALHit({
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

        return BCALHit;
    }

};
#endif //EVESTANDALONE_BCAL_H
