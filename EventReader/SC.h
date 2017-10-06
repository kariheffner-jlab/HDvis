//
// Created by tbritton on 6/20/17.
//

#ifndef EVESTANDALONE_SC_H
#define EVESTANDALONE_SC_H

#include <START_COUNTER/DSCHit.h>

class StartC {
public:

    static tao::json::value Add_SCHits(vector<const DSCHit *> SCHits) {

        auto arr = tao::json::value::array({});

        for (uint i = 0; i < SCHits.size(); i++) {
            float t_fADC_touse=SCHits[i]->t_fADC;
            if(t_fADC_touse != t_fADC_touse)
            {
                t_fADC_touse=-10000;
            }

            float t_TDC_touse=SCHits[i]->t_TDC;
            if(t_TDC_touse != t_TDC_touse)
            {
                t_TDC_touse=-10000;
            }

            arr.emplace_back(WriteHitJSON(i, SCHits[i]->sector, SCHits[i]->dE, SCHits[i]->t, t_TDC_touse,t_fADC_touse, SCHits[i]->pulse_height, SCHits[i]->has_fADC, SCHits[i]->has_TDC));


        }


        return arr;

    }

     static tao::json::value WriteHitJSON(int id, int sector, float dE, float t, float t_TDC, float t_fADC, float pulse_height, bool has_fADC, bool has_TDC)
     {
         tao::json::value SCHit({
                                         {"id", id},
                                         {"sector", sector},
                                         {"t", t},
                                         {"dE", dE},
                                         {"t_TDC", t_TDC},
                                         {"t_fADC", t_fADC},
                                         {"pulse_height", pulse_height},
                                         {"has_fADC", has_fADC},
                                         {"has_TDC", has_TDC}
                                 });

         return SCHit;//event_out << tao::json::to_string(SCHit, 4);
     }
};
#endif //EVESTANDALONE_SC_H
