//
// Created by tbritton on 6/20/17.
//

#ifndef EVESTANDALONE_CDC_H
#define EVESTANDALONE_CDC_H

#include <CDC/DCDCHit.h>

class CDC
{
public:

    void Add_CDCHits(vector<const DCDCHit*> CDCHits)
    {

        ofstream event_out;
        event_out.open("event.json", ios::app);//JSON
        event_out<<"\"CDC_hits\": "<<"[\n";//JSON

        for(uint i=0;i<CDCHits.size();i++)
        {
            WriteHitJSON(event_out, i, CDCHits[i]->ring, CDCHits[i]->straw, CDCHits[i]->q, CDCHits[i]->t, CDCHits[i]->d, CDCHits[i]->itrack, CDCHits[i]->ptype);
            if(i!=CDCHits.size()-1)
                event_out<<","<<endl;

        }

        event_out<<"]"<<endl;
        event_out.close();

    }

    void WriteHitJSON(ofstream& event_out, int id,	int ring, int straw, float q, float t, float d, int itrack, int ptype)
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

        event_out << tao::json::to_string(CDCHit, 4);
    }

};

#endif //EVESTANDALONE_CDC_H
