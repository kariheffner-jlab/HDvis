//
// Created by tbritton on 5/31/17.
//

#ifndef EVESTANDALONE_FCAL_H
#define EVESTANDALONE_FCAL_H

#include <TEveBoxSet.h>
#include <TEveManager.h>
#include <TEveJetCone.h>
#include <TRandom3.h>

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
    void Add_FCALShowers(vector<const DFCALShower *> FCALShowers)
    {

        vector<TEveJetCone*> FCAL_showers;

        for(uint i=0;i<FCALShowers.size();i++)
        {
            cout<<"totes shower"<<endl;
            cout<<float(FCALShowers[i]->getPosition().X())<<","<<float(FCALShowers[i]->getPosition().Y())<<","<<float(FCALShowers[i]->getPosition().Z())<<endl;
            auto FCAL_shower=new TEveJetCone(Form("FCAL Shower %d", i), "");
            TEveVector show_pos;
            show_pos.fX=float(FCALShowers[i]->getPosition().X());
            show_pos.fY=float(FCALShowers[i]->getPosition().Y());
            show_pos.fZ=float(FCALShowers[i]->getPosition().Z());//float(500 + 173.9);
            //FCAL_shower->SetCylinder(129 - 10, 268.36 - 10);
            //FCAL_shower->AddEllipticCone(0,  0, 6, 6);
            FCAL_shower->SetApex(show_pos);
            //FCAL_shower->SetCylinder(1,1);
            //TRandom3 r(0);


            FCAL_shower->AddCone(1.4, 1.2, 10., 26.5);
            FCAL_shower->SetMainColorRGB(float(255), 255, 0);
            FCAL_shower->SetMainAlpha(.75);

            FCAL_showers.push_back(FCAL_shower);
        }

        for(uint i=0;i<FCAL_showers.size();i++)
        {
            gEve->AddElement(FCAL_showers[i]);
        }
    }
private:

};
#endif //EVESTANDALONE_FCAL_H
