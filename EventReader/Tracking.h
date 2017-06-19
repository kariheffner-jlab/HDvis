//
// Created by tbritton on 5/31/17.
//

#include <TEvePointSet.h>
#include <TRACKING/DReferenceTrajectory.h>
#include <PID/DChargedTrack.h>
#include <PID/DNeutralParticle.h>
#include <TEveManager.h>
#include <fstream>

#ifndef EVESTANDALONE_TRACKING_H
#define EVESTANDALONE_TRACKING_H



class Tracking
{
public:

    Tracking(DMagneticFieldMap* Bfield, DRootGeom* RootGeom)
    {
        int RMAX_INTERIOR=65;
        int RMAX_EXTERIOR=89;
        rt=new DReferenceTrajectory(Bfield);
        rt->Rsqmax_interior = RMAX_INTERIOR*RMAX_INTERIOR;//innerBCAL radius
        rt->Rsqmax_exterior = RMAX_EXTERIOR*RMAX_EXTERIOR;

        rt->SetDRootGeom(RootGeom);
        rt->SetDGeometry(NULL);
    };


    void Add_DChargedTracks(vector<const DChargedTrack*> ChargedTracks)
    {
        ofstream event_out;
        event_out.open("../js/eventw.json", ios::app);//JSON
        event_out<<"\"charged_tracks\": "<<"[\n";//JSON

        vector<TEvePointSet*> Track_points;
        for(int i=0;i<ChargedTracks.size()/*TrackCandidates.size()*/;i++)
        {
            event_out<<"{"<<endl;
            event_out<<"\"id\": "<<i<<","<<endl; //JSON

            string PID_name=ParticleType(ChargedTracks[i]->Get_BestFOM()->PID());
            string name=PID_name + Form(" Track Points %i", i);
            cout<<name<<endl;
            rt->Reset();
            auto Track_ps = new TEvePointSet();

            rt->SetMass(ChargedTracks[i]->Get_BestFOM()->mass());
            //rt.SetMass(TrackCandidates[i]->mass());

            auto position = ChargedTracks[i]->Get_BestFOM()->position();
            auto momentum = ChargedTracks[i]->Get_BestFOM()->momentum();
            auto charge = ChargedTracks[i]->Get_BestFOM()->charge();

            event_out<<"\"charge\": "<<charge<<","<<endl; //JSON

            rt->Swim(position, momentum, charge);
            //rt.Swim(TrackCandidates[i]->position(), TrackCandidates[i]->momentum(), TrackCandidates[i]->charge());
            DReferenceTrajectory::swim_step_t* steps =rt->swim_steps;

            event_out<<"\"points\": "<<"["<<endl; //JSON

            for(int j=0; j<rt->Nswim_steps; j++)
            {
                DVector3 step_loc=steps[j].origin;

                if(j!=rt->Nswim_steps-1 && step_loc.Z()<=625 ) {
                    //cout << "with comma: " << j << "|" << rt->Nswim_steps << std::endl;
                    event_out << "[" << step_loc.X() << "," << step_loc.Y() << "," << step_loc.Z() << "],"
                              << endl; //JSON
                }
                else
                    event_out<<"["<<step_loc.X()<<","<<step_loc.Y()<<","<<step_loc.Z()<<"]"<<endl; //JSON

                //cout<<i<<"|"<<step_loc.X()<<","<<step_loc.Y()<<","<<step_loc.Z()<<endl;
                if(step_loc.Z()>625 )
                    break;

                if(step_loc.Z()<=0)
                    continue;

                Track_ps->SetNextPoint(step_loc.X(), step_loc.Y(),step_loc.Z()); //FCAL alignment is 150.501,-349.986,147.406
                //FCAL_ps->SetNextPoint(FCALHits[i]->x+150.501, FCALHits[i]->y-349.986, 26.5+147.406); //FCAL alignment is 150.501,-349.986,147.406

                //if(TrackCandidates[i]->charge()==1)
                if(ChargedTracks[i]->Get_BestFOM()->charge()==-1)
                    Track_ps->SetMainColorRGB(0,float(250.), 0.);
                else
                    Track_ps->SetMainColorRGB(float(250.),0, 0.);


                Track_ps->SetPointId(new TNamed(Form(" Track Points %i", i), ""));
                Track_ps->SetMarkerSize(1);
                Track_ps->SetMarkerStyle(4);
                Track_ps->SetElementName(name.c_str());

            }
            event_out<<"]"<<endl; //JSON
            if(i!=ChargedTracks.size()-1)
                event_out<<"},"<<endl;
            else
                event_out<<"}"<<endl;

            Track_points.push_back(Track_ps);
        }
        event_out<<"]"<<endl;
        event_out.close();
        for (int i = 0; i < Track_points.size(); i++) {
            gEve->AddElement(Track_points[i]);

        }

    }

    void Add_DNeutralParticles(vector<const DNeutralParticle*> NeutralTracks)
    {
        vector<TEvePointSet*> NeutTrack_points;
        for(int i=0;i<NeutralTracks.size()/*TrackCandidates.size()*/;i++)
        {

            string PID_name=ParticleType(NeutralTracks[i]->Get_BestFOM()->PID());
            string name=PID_name + Form(" Track Points %i", i);
            //cout<<name<<endl;
            rt->Reset();
            auto Track_ps = new TEvePointSet();


            rt->SetMass(NeutralTracks[i]->Get_BestFOM()->mass());
            //rt.SetMass(TrackCandidates[i]->mass());

            rt->Swim(NeutralTracks[i]->Get_BestFOM()->position(), NeutralTracks[i]->Get_BestFOM()->momentum(), NeutralTracks[i]->Get_BestFOM()->charge());
            //rt.Swim(TrackCandidates[i]->position(), TrackCandidates[i]->momentum(), TrackCandidates[i]->charge());
            DReferenceTrajectory::swim_step_t* steps =rt->swim_steps;

            for(int j=0; j<rt->Nswim_steps; j++)
            {
                DVector3 step_loc=steps[j].origin;
                //cout<<i<<"|"<<step_loc.X()<<","<<step_loc.Y()<<","<<step_loc.Z()<<endl;
                if(step_loc.Z()>625 )
                    break;

                if(step_loc.Z()<=0)
                    continue;

                Track_ps->SetNextPoint(step_loc.X(), step_loc.Y(),step_loc.Z()); //FCAL alignment is 150.501,-349.986,147.406
                //FCAL_ps->SetNextPoint(FCALHits[i]->x+150.501, FCALHits[i]->y-349.986, 26.5+147.406); //FCAL alignment is 150.501,-349.986,147.406


                Track_ps->SetMainColorRGB(float(255.),float(255.), 0.);


                Track_ps->SetPointId(new TNamed(Form(" Track Points %i", i), ""));
                Track_ps->SetMarkerSize(1);
                Track_ps->SetMarkerStyle(4);
                Track_ps->SetElementName(name.c_str());

            }
            NeutTrack_points.push_back(Track_ps);
        }
        for (int i = 0; i < NeutTrack_points.size(); i++) {
            gEve->AddElement(NeutTrack_points[i]);
        }

    }
private:
    DReferenceTrajectory* rt;
};

#endif //EVESTANDALONE_TRACKING_H