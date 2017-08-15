//
// Created by tbritton on 6/20/17.
//

#ifndef EVESTANDALONE_CDC_H
#define EVESTANDALONE_CDC_H

#include <CDC/DCDCHit.h>

class CDC
{
public:

    static tao::json::value Add_CDCHits(vector<const DCDCHit*> CDCHits)
    {

        auto arr = tao::json::value::array({});

        for(uint i=0;i<CDCHits.size();i++)
        {
            //std::cout<<i<<","<< CDCHits[i]->ring<<","<< CDCHits[i]->straw<<","<< CDCHits[i]->q<<","<< CDCHits[i]->t<<","<< CDCHits[i]->d<<","<< CDCHits[i]->itrack<<","<< CDCHits[i]->ptype<<std::endl;
            arr.emplace_back(WriteHitJSON(i, CDCHits[i]->ring, CDCHits[i]->straw, CDCHits[i]->q, CDCHits[i]->t, CDCHits[i]->d, CDCHits[i]->itrack, CDCHits[i]->ptype));
        }

        return arr;
    }

    static tao::json::value WriteHitJSON(int id, int ring, int straw, float q, float t, float d, int itrack, int ptype)
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
