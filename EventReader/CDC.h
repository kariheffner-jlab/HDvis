//
// Created by tbritton on 6/20/17.
//

#ifndef EVESTANDALONE_CDC_H
#define EVESTANDALONE_CDC_H

#include <CDC/DCDCHit.h>

class CDC
{
public:

    string Add_CDCHits(vector<const DCDCHit*> CDCHits)
    {

        ostringstream event_out;

        event_out<<"\"CDC_hits\": "<<"[\n";//JSON

        for(uint i=0;i<CDCHits.size();i++)
        {
            event_out<<WriteHitJSON(i, CDCHits[i]->ring, CDCHits[i]->straw, CDCHits[i]->q, CDCHits[i]->t, CDCHits[i]->d, CDCHits[i]->itrack, CDCHits[i]->ptype);
            if(i!=CDCHits.size()-1)
                event_out<<","<<endl;

        }

        event_out<<"]"<<endl;
        string outstr = event_out.str();
        return outstr;

    }

    tao::json::value WriteHitJSON(int id, int ring, int straw, float q, float t, float d, int itrack, int ptype)
    {
        tao::json::value CDCHit({
                                         {"id", id},
                                         {"ring", ring},
                                         {"straw", straw},
                                         {"q", q},
                                         {"t", t},
                                         {"d", d},
                                         {"itrack", itrack},
                                         {"ptype", ptype}
                                 });

        return CDCHit;//, 4);
    }

};

#endif //EVESTANDALONE_CDC_H
