//
// Created by tbritton on 6/20/17.
//

#ifndef EVESTANDALONE_SC_H
#define EVESTANDALONE_SC_H

#include <START_COUNTER/DSCHit.h>

class StartC {
public:

    void Add_SCHits(vector<const DSCHit *> SCHits) {

        ofstream event_out;
        event_out.open("event.json", ios::app);//JSON
        event_out << "\"SC_hits\": " << "[\n";//JSON

        for (uint i = 0; i < SCHits.size(); i++) {
            WriteHitJSON(event_out, i, SCHits[i]->sector, SCHits[i]->dE, SCHits[i]->t, SCHits[i]->t_TDC,SCHits[i]->t_fADC, SCHits[i]->pulse_height, SCHits[i]->has_fADC, SCHits[i]->has_TDC);
            if (i != SCHits.size() - 1)
                event_out << "," << endl;

        }

        event_out << "]" << endl;
        event_out.close();

    }

     void WriteHitJSON(ofstream& event_out, int id, int sector, float dE, float t, float t_TDC, float t_fADC, float pulse_height, bool has_fADC, bool has_TDC)
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

         event_out << tao::json::to_string(SCHit, 4);
     }
};
#endif //EVESTANDALONE_SC_H
