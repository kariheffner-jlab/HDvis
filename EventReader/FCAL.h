//
// Created by tbritton on 5/31/17.
//

#ifndef EVESTANDALONE_FCAL_H
#define EVESTANDALONE_FCAL_H

#include <TEveBoxSet.h>
#include <TEveManager.h>

class FCAL
{
public:

    void Add_FCALHits(vector<const DFCALHit *> FCALHits)
    {
        auto FCAL_bs = new TEveBoxSet("FCAL_hits");
        TEveRGBAPalette* pal = new TEveRGBAPalette(-150,150);

        FCAL_bs->SetPalette(pal);
        //FCAL_bs->SetFrame(frm);
        FCAL_bs->Reset(TEveBoxSet::kBT_AABox, kTRUE, 64);

        //h2->Reset();
        for (uint i = 0; i < FCALHits.size(); i++) {


            FCAL_bs->AddBox(FCALHits[i]->x - 1, FCALHits[i]->y - 1, float_t(500 + 173.9), 2, 2,
                            FCALHits[i]->E * 100/*20*log(FCALHits[i]->E*1000)*/);

            int redness = 255;
            if (abs(FCALHits[i]->t) * 10 < 255)
                redness = int(abs(FCALHits[i]->t) * 10);

            if (FCALHits[i]->t >= 0.0)
                FCAL_bs->DigitColor(UChar_t(redness), 0, 0, 50);
                //FCAL_bs->DigitValue(FCALHits[i]->t);
            else
                FCAL_bs->DigitColor(0, UChar_t(redness), 0, 50);
            //FCAL_bs->DigitColor(0,abs(FCALHits[i]->t),0);


        }
        gEve->AddElement(FCAL_bs);
    }
private:

};
#endif //EVESTANDALONE_FCAL_H
