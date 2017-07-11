//
// Created by tbritton on 6/20/17.
//

#ifndef EVESTANDALONE_SC_H
#define EVESTANDALONE_SC_H

#include <START_COUNTER/DSCHit.h>

class StartC {
public:

    string Add_SCHits(vector<const DSCHit *> SCHits) {

        ostringstream event_out;
        event_out << "\"SC_hits\": " << "[\n";//JSON

        for (uint i = 0; i < SCHits.size(); i++) {
            event_out<<WriteHitJSON(i, SCHits[i]->sector, SCHits[i]->dE, SCHits[i]->t, SCHits[i]->t_TDC,SCHits[i]->t_fADC, SCHits[i]->pulse_height, SCHits[i]->has_fADC, SCHits[i]->has_TDC);
            if (i != SCHits.size() - 1)
                event_out << "," << endl;

        }

        event_out << "]" << endl;
        string outstr = event_out.str();
        return outstr;

    }

    tao::json::value WriteHitJSON(int id, int sector, float dE, float t, float t_TDC, float t_fADC, float pulse_height, bool has_fADC, bool has_TDC)
     {
         tao::json::value SCHit({
                                         {"id", id},
                                         {"sector", sector},
                                         {"t", t},
                                         //{"t_TDC", t_TDC},
                                         //{"t_fADC", t_fADC},
                                         {"pulse_height", pulse_height},
                                         {"has_fADC", has_fADC},
                                         {"has_TDC", has_TDC}
                                 });

         return SCHit;//event_out << tao::json::to_string(SCHit, 4);
     }
};
#endif //EVESTANDALONE_SC_H
