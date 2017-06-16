//
// Created by romanov on 6/15/17.
//

#ifndef EVESTANDALONE_GEOMETRYCONVERTER_H
#define EVESTANDALONE_GEOMETRYCONVERTER_H

#include <thread>

#include <TFile.h>
#include <TApplication.h>

#include <TEveCalo.h>
#include <TEveWindow.h>
#include <TEveManager.h>
#include <TEveBrowser.h>
#include <TEveScene.h>
#include <TEveViewer.h>
#include <TGeoManager.h>
#include <TEveGeoNode.h>
#include <TEveGedEditor.h>
#include <TGTab.h>
#include <iostream>
#include <TRint.h>
#include "TGDMLWrite.h"
#include "StringUtils.h"


namespace hdvis{

class GeometryConverter {
public:



////////////////////////////////////////////////////////////////////////////////
/// Chooses the object and method that should be used for processing object

void ProcessShape(TGeoShape *geoShape) {
    using namespace std;
    string clsname(geoShape->ClassName());


    //process different sh apes
    if (clsname == "TGeoBBox") {
        CreateBoxN((TGeoBBox *) geoShape);
    } /*else if (strcmp(clsname, "TGeoParaboloid") == 0) {
        solidN = CreateParaboloidN((TGeoParaboloid*) geoShape);
    } else if (strcmp(clsname, "TGeoSphere") == 0) {
        solidN = CreateSphereN((TGeoSphere*) geoShape);
    } else if (strcmp(clsname, "TGeoArb8") == 0) {
        solidN = CreateArb8N((TGeoArb8*) geoShape);
    } else if (strcmp(clsname, "TGeoConeSeg") == 0) {
        solidN = CreateConeN((TGeoConeSeg*) geoShape);
    } else if (strcmp(clsname, "TGeoCone") == 0) {
        solidN = CreateConeN((TGeoCone*) geoShape);
    } else if (strcmp(clsname, "TGeoPara") == 0) {
        solidN = CreateParaN((TGeoPara*) geoShape);
    } else if (strcmp(clsname, "TGeoTrap") == 0) {
        solidN = CreateTrapN((TGeoTrap*) geoShape);
    } else if (strcmp(clsname, "TGeoGtra") == 0) {
        solidN = CreateTwistedTrapN((TGeoGtra*) geoShape);
    } else if (strcmp(clsname, "TGeoTrd1") == 0) {
        solidN = CreateTrdN((TGeoTrd1*) geoShape);
    } else if (strcmp(clsname, "TGeoTrd2") == 0) {
        solidN = CreateTrdN((TGeoTrd2*) geoShape);
    } else if (strcmp(clsname, "TGeoTubeSeg") == 0) {
        solidN = CreateTubeN((TGeoTubeSeg*) geoShape);
    } else if (strcmp(clsname, "TGeoCtub") == 0) {
        solidN = CreateCutTubeN((TGeoCtub*) geoShape);
    } */else if (clsname == "TGeoTube") {
        CreateTubeN((TGeoTube*) geoShape);
    } /*else if (strcmp(clsname, "TGeoPcon") == 0) {
        solidN = CreatePolyconeN((TGeoPcon*) geoShape);
    } else if (strcmp(clsname, "TGeoTorus") == 0) {
        solidN = CreateTorusN((TGeoTorus*) geoShape);
    } else if (strcmp(clsname, "TGeoPgon") == 0) {
        solidN = CreatePolyhedraN((TGeoPgon*) geoShape);
    } else if (strcmp(clsname, "TGeoEltu") == 0) {
        solidN = CreateEltubeN((TGeoEltu*) geoShape);
    } else if (strcmp(clsname, "TGeoHype") == 0) {
        solidN = CreateHypeN((TGeoHype*) geoShape);
    } else if (strcmp(clsname, "TGeoXtru") == 0) {
        solidN = CreateXtrusionN((TGeoXtru*) geoShape);
    } else if (strcmp(clsname, "TGeoScaledShape") == 0) {
        TGeoScaledShape * geoscale = (TGeoScaledShape *) geoShape;
        TString scaleObjClsName = geoscale->GetShape()->ClassName();
        if (scaleObjClsName == "TGeoCone") {
            solidN = CreateElConeN((TGeoScaledShape*) geoShape);
        } else {
            Info("ChooseObject",
                 "ERROR! TGeoScaledShape object is not possible to process correctly. %s object is processed without scale",
                 scaleObjClsName.Data());
            solidN = ChooseObject(geoscale->GetShape());
            //Name has to be propagated to geoscale level pointer
            fNameList->fLst[TString::Format("%p", geoscale)] =
                    fNameList->fLst[TString::Format("%p", geoscale->GetShape())];
        }
    } else if (strcmp(clsname, "TGeoCompositeShape") == 0) {
        solidN = CreateCommonBoolN((TGeoCompositeShape*) geoShape);
    } else if (strcmp(clsname, "TGeoUnion") == 0) {
        solidN = CreateCommonBoolN((TGeoCompositeShape*) geoShape);
    } else if (strcmp(clsname, "TGeoIntersection") == 0) {
        solidN = CreateCommonBoolN((TGeoCompositeShape*) geoShape);
    } else if (strcmp(clsname, "TGeoSubtraction") == 0) {
        solidN = CreateCommonBoolN((TGeoCompositeShape*) geoShape);
    } else {
        Info("ChooseObject", "ERROR! %s Solid CANNOT be processed, solid is NOT supported",
             clsname);
        solidN = NULL;
    }
    if (solidN == NULL) {
        if (fNameList->fLst[TString::Format("%p", geoShape)] == "") {
            TString missingName = geoShape->GetName();
            StringUtils::EscapeJsonString("missing_" + missingName, TString::Format("%p", geoShape));
        } else {
            fNameList->fLst[TString::Format("%p", geoShape)] = "missing_" + fNameList->fLst[TString::Format("%p", geoShape)];
        }
    }

    return solidN;*/
}


    ////////////////////////////////////////////////////////////////////////////////
    /// Check for null parameter to skip the NULL objects

    bool IsNullParam(Double_t parValue, const std::string& parName, const std::string&  objName)
    {
        using namespace std;

        if (parValue == 0.) {
            std::cout<<"IsNullParam. ERROR! "<<objName<<" is NULL due to "<<parName<<" = "<<parValue<<", Volume based on this shape will be skipped"<<std::endl;
            return true;
        }
        return false;
    }


////////////////////////////////////////////////////////////////////////////////
/// Creates "box" node for GDML

    void CreateBoxN(TGeoBBox * geoShape)
    {
        using namespace std;

        //XMLNodePointer_t mainN = fGdmlE->NewChild(0, 0, "box", 0);

        string lname = geoShape->GetName();
        lname = StringUtils::EscapeJsonString(lname);

        if (IsNullParam(geoShape->GetDX(), "DX", lname) ||
            IsNullParam(geoShape->GetDY(), "DY", lname) ||
            IsNullParam(geoShape->GetDZ(), "DZ", lname)) {
            return;
        }

        cout<<""
                "        object = new THREE.Mesh( new THREE.BoxGeometry( "
                ""<<geoShape->GetDX()<<", "
                ""<<geoShape->GetDY()<<", "
                ""<<geoShape->GetDZ()<<", 4, 4, 4 ), material );\n"<<endl;



//        cout<<"x = "<<geoShape->GetDX()<<" y = "<<geoShape->GetDY()<<" z = "<<geoShape->GetDZ()<<endl;

    }


////////////////////////////////////////////////////////////////////////////////
/// Creates "tube" node for GDML  from  object TGeoTubeSeg

        XMLNodePointer_t CreateTubeN(TGeoTube * geoShape)
        {
//            XMLNodePointer_t mainN = fGdmlE->NewChild(0, 0, "tube", 0);
//            TString lname = GenName(geoShape->GetName(), TString::Format("%p", geoShape));
//            fGdmlE->NewAttr(mainN, 0, "name", lname);
//            if (IsNullParam(geoShape->GetRmax(), "Rmax", lname) ||
//                IsNullParam(geoShape->GetDz(), "Dz", lname)) {
//                return NULL;
//            }
//
//            fGdmlE->NewAttr(mainN, 0, "rmin", TString::Format("%.12g", geoShape->GetRmin()));
//            fGdmlE->NewAttr(mainN, 0, "rmax", TString::Format("%.12g", geoShape->GetRmax()));
//            fGdmlE->NewAttr(mainN, 0, "z", TString::Format("%.12g",  2 * geoShape->GetDz()));
//            fGdmlE->NewAttr(mainN, 0, "startphi", TString::Format("%.12g", geoShape->GetPhi1()));
//            fGdmlE->NewAttr(mainN, 0, "deltaphi", TString::Format("%.12g", geoShape->GetPhi2() - geoShape->GetPhi1()));
//
//            fGdmlE->NewAttr(mainN, 0, "aunit", "deg");
//            fGdmlE->NewAttr(mainN, 0, "lunit", "cm");
//            return mainN;
        }


        static const UInt_t fgkProcBit    = BIT(14);    //14th bit is set when solid is processed
    static const UInt_t fgkProcBitVol = BIT(19);    //19th bit is set when volume is processed
    static const UInt_t fgkMaxNameErr = 5;          //maximum number of errors for naming

    std::vector<std::string> fNameList;

    struct Xyz {
        Double_t x;
        Double_t y;
        Double_t z;
    };


////////////////////////////////////////////////////////////////////////////////
/// Retrieves X Y Z angles from rotation matrix

    Xyz GetXYZangles(const Double_t * rotationMatrix)
    {
        Xyz lxyz;
        Double_t a, b, c;
        Double_t rad = 180.0 / TMath::ACos(-1.0);
        const Double_t *r = rotationMatrix;
        Double_t cosb = TMath::Sqrt(r[0] * r[0] + r[1] * r[1]);
        if (cosb > 0.00001) {
            a = TMath::ATan2(r[5], r[8]) * rad;
            b = TMath::ATan2(-r[2], cosb) * rad;
            c = TMath::ATan2(r[1], r[0]) * rad;
        } else {
            a = TMath::ATan2(-r[7], r[4]) * rad;
            b = TMath::ATan2(-r[2], cosb) * rad;
            c = 0;
        }
        lxyz.x = a;
        lxyz.y = b;
        lxyz.z = c;
        return lxyz;
    }


////////////////////////////////////////////////////////////////////////////////
/// Method extracting geometry structure recursively

        void ExtractVolumesNew(TGeoNode *node, TGeoVolume* volume, int level=0)
        {
            using namespace std;
            string padding = "";
            for(size_t i=0; i<level; i++) padding+="   ";

            auto volumeName = StringUtils::EscapeJsonString(volume->GetName());
            auto materialName = volume->GetMaterial()->GetName();
            auto shapeClassName = volume->GetShape()->ClassName();

            if(/*volumeName=="IYDN" || volumeName == "BCSD" ||*/ volumeName.find("xd") == 0)
            {
                //cout<<padding<<"volume="<< volumeName <<" shape="<<shapeClassName<<" mat="<<materialName<<endl;
                ProcessShape(volume->GetShape());
                node->GetMatrix()->GetRotationMatrix();

                //position
                const Double_t * pos = node->GetMatrix()->GetTranslation();
                Xyz nodPos;
                nodPos.x = pos[0];
                nodPos.y = pos[1];
                nodPos.z = pos[2];

                cout<<pos[0]<<endl;
                cout<<pos[1]<<endl;
                cout<<pos[2]<<endl;
                cout<<pos[3]<<endl;
                cout<<pos[4]<<endl;
                cout<<pos[5]<<endl;
                cout<<pos[6]<<endl;
                cout<<pos[7]<<endl;
                cout<<pos[8]<<endl;
                cout<<"    object.position.set( "<<nodPos.x<<", "<<nodPos.y<<", "<<nodPos.z<<" );\n"
                        "        scene.add( object );"<<endl;

                //rotation
                //Xyz lxyz = GetXYZangles(node->GetMatrix()->GetRotationMatrix());
                //cout<<"   rotation  lxyz.x="<<lxyz.x<<"  lxyz.y="<<lxyz.y<<"  lxyz.z="<<lxyz.z<<endl;


            }



            //get all nodes in volume
            TObjArray *nodeLst = volume->GetNodes();
            TIter next(nodeLst);
            TGeoNode *childNode;

            //loop through all nodes
            while ((childNode = (TGeoNode *) next()))
            {
                if(childNode && childNode->GetVolume())
                {
                    ExtractVolumesNew(childNode, childNode->GetVolume(), level+1);
                }

            }
        }


////////////////////////////////////////////////////////////////////////////////
/// Method extracting geometry structure recursively

    void ExtractVolumes(TGeoVolume* volume)
    {
        using namespace std;

        string volname, matname, solname, pattClsName, nodeVolNameBak;
        TGeoPatternFinder *pattFinder = 0;
        Bool_t isPattern = kFALSE;

        volname = StringUtils::EscapeJsonString(volume->GetName());

        cout<<"volname ="<< volname<<endl;


        //get reference material and add solid to <solids> + get name
        matname = volume->GetMaterial()->GetName();
        //cout<<"   matname = "<<matname<<endl;

        string clsname(volume->GetShape()->ClassName());
        //cout<<"   clsname = "<<clsname<<endl;
        ProcessShape(volume->GetShape());

        //divisionvol can't be in assembly
        pattFinder = volume->GetFinder();
        //if found pattern
        if (pattFinder) {
            cout<<"  it is pattern"<<endl;
            isPattern = kTRUE;
         }


        //get all nodes in volume
        TObjArray *nodeLst = volume->GetNodes();
        TIter next(nodeLst);
        TGeoNode *geoNode;

        Int_t nCnt = 0;
        //loop through all nodes
        while ((geoNode = (TGeoNode *) next()))
        {
            geoNode->GetMatrix();
            //get volume of current node and if not processed then process it
            TGeoVolume * subvol = geoNode->GetVolume();

            if (subvol->TestAttBit(fgkProcBitVol) == kFALSE) {
                subvol->SetAttBit(fgkProcBitVol);
                ExtractVolumes(subvol);
            }


            if (isPattern == kFALSE) {
                //create name for node
                string nodename, posname, rotname;
                nodename = geoNode->GetName();
                nodename = StringUtils::EscapeJsonString(nodename);
                nodename = nodename + "in" + volname;
                cout<<"  nodename = "<<nodename<<endl;

                //create name for position and clear rotation
                posname = nodename + "pos";
                rotname = "";


                //position
                const Double_t * pos = geoNode->GetMatrix()->GetTranslation();
                Xyz nodPos;
                nodPos.x = pos[0];
                nodPos.y = pos[1];
                nodPos.z = pos[2];
                //childN = CreatePositionN(posname.Data(), nodPos);
                //fGdmlE->AddChild(fDefineNode, childN); //adding node to <define> node

                //Deal with reflection

                Double_t lx, ly, lz;
                Double_t xangle = 0;
                Double_t zangle = 0;
                lx = geoNode->GetMatrix()->GetTranslation()[0];
                ly = geoNode->GetMatrix()->GetRotationMatrix()[4];
                lz = geoNode->GetMatrix()->GetRotationMatrix()[8];


                //rotation
                Xyz lxyz = GetXYZangles(geoNode->GetMatrix()->GetRotationMatrix());
                lxyz.x -= xangle;
                lxyz.z -= zangle;
                if ((lxyz.x != 0.0) || (lxyz.y != 0.0) || (lxyz.z != 0.0)) {
                    rotname = nodename + "rot";
                    cout<<"   rotation "<<rotname<<" lxyz.x="<<lxyz.x<<" lxyz.y"<<lxyz.y<<" lxyz.z"<<lxyz.z<<endl;
                }

                //cout<<"   Add child?"<<endl;
            }
        }

        //create only one divisionvol node
        if (isPattern && pattFinder) {
            //retrieve attributes of division
            cout<<"isPattern && pattFinder"<<endl;

//        Int_t ndiv, divaxis;
//        Double_t offset, width, xlo, xhi;
//        TString axis, unit;
//
//        ndiv = pattFinder->GetNdiv();
//        width = pattFinder->GetStep();
//
//        divaxis = pattFinder->GetDivAxis();
//        volume->GetShape()->GetAxisRange(divaxis, xlo, xhi);
//
//        //compute relative start (not positional)
//        offset = pattFinder->GetStart() - xlo;
//        axis = GetPattAxis(divaxis, pattClsName, unit);
//
//        //create division node
//        childN = CreateDivisionN(offset, width, ndiv, axis.Data(), unit.Data(), nodeVolNameBak.Data());
//        fGdmlE->AddChild(volumeN, childN);
        }

        //fVolCnt++;
        //add volume/assembly node into the <structure> node
        //fGdmlE->AddChild(fStructureNode, volumeN);

    }

};
}

#endif //EVESTANDALONE_GEOMETRYCONVERTER_H
