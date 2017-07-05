//
// Created by tbritton on 6/20/17.
//

#ifndef EVESTANDALONE_BCAL_H
#define EVESTANDALONE_BCAL_H

#include <BCAL/DBCALHit.h>

class BCAL
{
public:

    void Add_BCALHits(vector<const DBCALHit*> BCALHits)
    {

        ofstream event_out;
        event_out.open("event.json", ios::app);//JSON
        event_out<<"\"BCAL_hits\": "<<"[\n";//JSON

        for(uint i=0;i<BCALHits.size();i++)
        {
            WriteHitJSON(event_out, i, BCALHits[i]->module, BCALHits[i]->layer, BCALHits[i]->sector, BCALHits[i]->end, BCALHits[i]->pulse_peak, BCALHits[i]->E, BCALHits[i]->t, BCALHits[i]->t_raw, BCALHits[i]->cellId);
                if(i!=BCALHits.size()-1)
                    event_out<<","<<endl;

        }

        event_out<<"]"<<endl;
        event_out.close();

    }

    void WriteHitJSON(ofstream& event_out, int id, int module, int layer, int sector, DBCALGeometry::End end, int pulse_peak, float E, float t, float t_raw, int cellId)
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

        event_out << tao::json::to_string(BCALHit, 4);
    }

};
#endif //EVESTANDALONE_BCAL_H
