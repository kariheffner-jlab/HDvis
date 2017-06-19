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

#include <tao/json.hpp>
#include <tao/json/from_string.hpp>
#include <tao/json/value.hpp>

namespace hdvis{

class GeometryConverter {
public:

    GeometryConverter()
    {
        //initialize list of accepted patterns for divisions (in ExtractVolumes)
        _acceptedPatterns["TGeoPatternX"] =      true;
        _acceptedPatterns["TGeoPatternY"] =      true;
        _acceptedPatterns["TGeoPatternZ"] =      true;
        _acceptedPatterns["TGeoPatternCylR"] =   true;
        _acceptedPatterns["TGeoPatternCylPhi"] = true;
        //========================

        //initialize list of rejected shapes for divisions (in ExtractVolumes)
        //this shapes are rejected because, it is not possible to divide trd2
        //in Y axis and while only trd2 object is imported from GDML
        //it causes a problem when TGeoTrd1 is divided in Y axis
        _rejectedShapes["TGeoTrd1"] = true;
        _rejectedShapes["TGeoTrd2"] = true;

    }


////////////////////////////////////////////////////////////////////////////////
/// Chooses the object and method that should be used for processing object

tao::json::value ProcessShape(TGeoShape *geoShape) {
    using namespace std;
    string clsname(geoShape->ClassName());

    tao::json::value solidN;
    //process different sh apes
    if (clsname == "TGeoBBox") {
        solidN = CreateBoxN((TGeoBBox *) geoShape);
    } else if (clsname == "TGeoParaboloid") {
        solidN.append({"error","TGeoParaboloid is not implemented"});
        //solidN = CreateParaboloidN((TGeoParaboloid*) geoShape);
    } else if (clsname == "TGeoSphere") {
        solidN.append({"error","TGeoSphere is not implemented"});
        //solidN = CreateSphereN((TGeoSphere*) geoShape);
    } else if (clsname == "TGeoArb8") {
        solidN.append({"error","TGeoArb8 is not implemented"});
        //solidN = CreateArb8N((TGeoArb8*) geoShape);
    } else if (clsname == "TGeoConeSeg") {
        solidN.append({"error","TGeoConeSeg is not implemented"});
        //solidN = CreateConeN((TGeoConeSeg*) geoShape);
    } else if (clsname == "TGeoCone") {
        solidN.append({"error","TGeoCone is not implemented"});
        //solidN = CreateConeN((TGeoCone*) geoShape);
    } else if (clsname == "TGeoPara") {
        solidN.append({"error","TGeoPara is not implemented"});
        //solidN = CreateParaN((TGeoPara*) geoShape);
    } else if (clsname == "TGeoTrap") {
        solidN.append({"error","TGeoTrap is not implemented"});
        //solidN = CreateTrapN((TGeoTrap*) geoShape);
    } else if (clsname == "TGeoGtra") {
        solidN.append({"error","TGeoGtra is not implemented"});
        //solidN = CreateTwistedTrapN((TGeoGtra*) geoShape);
    } else if (clsname == "TGeoTrd1") {
        solidN.append({"error","TGeoTrd1 is not implemented"});
        //solidN = CreateTrdN((TGeoTrd1*) geoShape);
    } else if (clsname == "TGeoTrd2") {
        solidN.append({"error","TGeoTrd2 is not implemented"});
        //solidN = CreateTrdN((TGeoTrd2*) geoShape);
    } else if (clsname == "TGeoTubeSeg") {
        solidN = CreateTubeN((TGeoTubeSeg*) geoShape);
    } else if (clsname == "TGeoCtub") {
        solidN.append({"error","TGeoCtub is not implemented"});
        //solidN = CreateCutTubeN((TGeoCtub*) geoShape);
    } else if (clsname == "TGeoTube") {
        solidN = CreateTubeN((TGeoTube*) geoShape);
    } else if (clsname == "TGeoPcon") {
        //solidN.append({"error","TGeoPcon is not implemented"});
        solidN = CreatePolyconeN((TGeoPcon*) geoShape);
    } else if (clsname == "TGeoTorus") {
        solidN.append({"error","TGeoTorus is not implemented"});
        //solidN = CreateTorusN((TGeoTorus*) geoShape);
    } else if (clsname == "TGeoPgon") {
        solidN.append({"error","TGeoPgon is not implemented"});
        //solidN = CreatePolyhedraN((TGeoPgon*) geoShape);
    } else if (clsname == "TGeoEltu") {
        solidN.append({"error","TGeoEltu is not implemented"});
        //solidN = CreateEltubeN((TGeoEltu*) geoShape);
    } else if (clsname == "TGeoHype") {
        solidN.append({"error","TGeoHype is not implemented"});
        //solidN = CreateHypeN((TGeoHype*) geoShape);
    } else if (clsname == "TGeoXtru") {
        solidN.append({"error","TGeoXtru is not implemented"});
        //solidN = CreateXtrusionN((TGeoXtru*) geoShape);
    } else if (clsname == "TGeoScaledShape") {
        solidN.append({"error","TGeoScaledShape is not implemented"});

        /*
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
        }*/
    } else if (clsname == "TGeoCompositeShape") {
        solidN.append({"error","TGeoCompositeShape is not implemented"});
        //solidN = CreateCommonBoolN((TGeoCompositeShape*) geoShape);
    } else if (clsname == "TGeoUnion") {
        solidN.append({"error","TGeoUnion is not implemented"});
        //solidN = CreateCommonBoolN((TGeoCompositeShape*) geoShape);
    } else if (clsname == "TGeoIntersection") {
        solidN.append({"error","TGeoSubtraction is not implemented"});
        //solidN = CreateCommonBoolN((TGeoCompositeShape*) geoShape);
    } else if (clsname == "TGeoSubtraction") {
        solidN.append({"error","CreateCommonBoolN is not implemented"});
        //solidN = CreateCommonBoolN((TGeoCompositeShape*) geoShape);
    } else {
        cerr<<"ChooseObject ERROR! %s Solid CANNOT be processed, solid is NOT supported"<<endl;
    }
    if (solidN.is_unsigned()) {
        cerr<<"solidN.is_unsigned() shape:"<<endl;
    }

    return solidN;
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
    tao::json::value CreateBoxN(TGeoBBox * geoShape)
    {
        tao::json::value result({
                              {"shape", "box"},
                              {"root_name", geoShape->GetName()},
                              {"x", geoShape->GetDX()},
                              {"y", geoShape->GetDY()},
                              {"z", geoShape->GetDZ()},
                              {"lunit", "cm"}
                      });
        return result;
    }


        ////////////////////////////////////////////////////////////////////////////////
        /// Creates "tube" node for GDML  from  object TGeoTubeSeg
        tao::json::value CreateTubeN(TGeoTube * geoShape)
        {
            tao::json::value result({
                                  {"shape", "tube"},
                                  {"root_name", geoShape->GetName()},
                                  {"rmin",      geoShape->GetRmin()},
                                  {"rmax",      geoShape->GetRmax()},
                                  {"z",          2 * geoShape->GetDz()},
                                  {"startphi",  0},
                                  {"deltaphi",  360},
                                  {"aunit", "deg"},
                                  {"lunit", "cm"}
                          });
            return result;
        }


////////////////////////////////////////////////////////////////////////////////
/// Creates "tube" node for GDML  from  object TGeoTubeSeg

        tao::json::value CreateTubeN(TGeoTubeSeg * geoShape)
        {tao::json::value result({
                                         {"shape", "tube"},
                                         {"root_name", geoShape->GetName()},
                                         {"rmin",      geoShape->GetRmin()},
                                         {"rmax",      geoShape->GetRmax()},
                                         {"z",          2 * geoShape->GetDz()},
                                         {"startphi",  geoShape->GetPhi1()},
                                         {"deltaphi",  geoShape->GetPhi2() - geoShape->GetPhi1()},
                                         {"aunit", "deg"},
                                         {"lunit", "cm"}
                                 });

            return result;
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
        /// Method that retrieves axis and unit along which object is divided

        TString GetPattAxis(Int_t divAxis, const char * pattName, TString& unit)
        {
            TString resaxis;
            unit = "cm";
            switch (divAxis) {
                case 1:
                    if (strcmp(pattName, "TGeoPatternX") == 0) {
                        return "kXAxis";
                    } else if (strcmp(pattName, "TGeoPatternCylR") == 0) {
                        return "kRho";
                    }
                    break;
                case 2:
                    if (strcmp(pattName, "TGeoPatternY") == 0) {
                        return "kYAxis";
                    } else if (strcmp(pattName, "TGeoPatternCylPhi") == 0) {
                        unit = "deg";
                        return "kPhi";
                    }
                    break;
                case 3:
                    if (strcmp(pattName, "TGeoPatternZ") == 0) {
                        return "kZAxis";
                    }
                    break;
                default:
                    return "kUndefined";
                    break;
            }
            return "kUndefined";
        }


        ////////////////////////////////////////////////////////////////////////////////
        /// Creates "polycone" node for GDML

        tao::json::value CreatePolyconeN(TGeoPcon * geoShape)
        {
            Int_t nZPlns = geoShape->GetNz();
            std::vector<tao::json::value> zplanes;
            for (Int_t it = 0; it < nZPlns; it++) {
                //add zplane child node

                zplanes.push_back(tao::json::value(
                        {
                                {"z", geoShape->GetZ(it)},
                                {"rmin", geoShape->GetRmin(it)},
                                {"rmax", geoShape->GetRmax(it)}
                        }
                ));
            }

            tao::json::value zplanesJson(zplanes);

            auto result = tao::json::value({
                    {"shape", "polycone"},
                    {"startphi", geoShape->GetPhi1()},
                    {"deltaphi", geoShape->GetDphi()},
                    {"aunit", "deg"},
                    {"lunit", "cm"},
                    {"zplanes", zplanesJson}
                                           });

            return result;
        }


        ////////////////////////////////////////////////////////////////////////////////
        /// Creates "divisionvol" node for GDML

        tao::json::value CreateDivisionN(Double_t offset, Double_t width, Int_t number,
                                         const char * axis, const char * unit, const char * volref,
                                         bool fgG4Compatibility=false)
        {
            using namespace tao::json;

            if (fgG4Compatibility  == kTRUE) {
                //if eg. full length is 20 and width * number = 20,0001 problem in geant4
                //unit is either in cm or degrees nothing else
                width = (floor(width * 1E4)) * 1E-4;
                if ((offset >= 0.) && (strcmp(axis, "kPhi") == 0)) {
                    Int_t offsetI = (Int_t) offset;
                    Double_t decimals = offset - offsetI;
                    //put to range from 0 to 360 add decimals and then put to range 0 -> -360
                    offset = (offsetI % 360) + decimals - 360;
                }
            }

            auto result = tao::json::value({
                    {"gdml_name", "divisionvol"},
                    {"axis", axis},
                    {"number", number},
                    {"width", width},
                    {"offset", offset},
                    {"unit", unit},
                    {"volumeref", volref}
                                    });
            return result;


        }

////////////////////////////////////////////////////////////////////////////////
/// Method extracting geometry structure recursively

        tao::json::value ExtractVolumesNew(TGeoNode *node, TGeoVolume* volume, int level, std::string parentPath)
        {
            using namespace std;
            using namespace tao;

            string padding = "";
            for(size_t i=0; i<level; i++) padding+="   ";

            auto volumeName = StringUtils::EscapeJsonString(volume->GetName());
            auto materialName = volume->GetMaterial()->GetName();
            auto shapeClassName = volume->GetShape()->ClassName();
            string path = parentPath + "/" + volumeName;
            _volumesByPath[volumeName] = volume;

            json::value volumeJson({
                                      {"name", volumeName},
                                      {"path", path },
                                      {"material", materialName}
                              });



            //divisionvol can't be in assembly

            bool isPattern;
            auto pattFinder = volume->GetFinder();
            //if found pattern
            if (pattFinder) {
                string pattClsName = pattFinder->ClassName();
                //if pattern in accepted pattern list and not in shape rejected list
                if ((_acceptedPatterns[pattClsName] == true) &&
                    (_rejectedShapes[shapeClassName] != kTRUE)) {
                    isPattern = kTRUE;
                    //cout<<"(!) Is PATTERN volume="<< volumeName <<" shape="<<shapeClassName<<" mat="<<materialName<<" pattName="<<pattClsName<<endl;

                    TString axis, unit;
                    int ndiv = pattFinder->GetNdiv();
                    double width = pattFinder->GetStep();

                    int divaxis = pattFinder->GetDivAxis();

                    // Get xlo and xhi
                    Double_t xlo, xhi;
                    volume->GetShape()->GetAxisRange(divaxis, xlo, xhi);

                    //compute relative start (not positional)
                    double offset = pattFinder->GetStart() - xlo;
                    axis = GetPattAxis(divaxis, pattClsName.c_str(), unit);

                    string nodeVolNameBak = volume->GetNode(0)->GetVolume()->GetName();

                    //create division node
                    auto v = CreateDivisionN(offset, width, ndiv, axis.Data(), unit.Data(), nodeVolNameBak.c_str());
                    volumeJson+={{"division", v}};
                    //cout<<json::to_string(v)<<endl;
                }
            }

            // Get shape parametrisation
            auto shapeJson = ProcessShape(volume->GetShape());
            volumeJson += {{"shape", shapeJson}};
            node->GetMatrix()->GetRotationMatrix();

            // Position and rotation

            //position
            const Double_t * pos = node->GetMatrix()->GetTranslation();

            // pos[0] - x, pos[1] - y, pos[2] - z
            volumeJson += {{"position", json::value::array({pos[0], pos[1], pos[2]})}};


            //Deal with reflection
            XMLNodePointer_t scaleN = NULL;
            Double_t lx, ly, lz;
            Double_t xangle = 0;
            Double_t zangle = 0;
            lx = node->GetMatrix()->GetRotationMatrix()[0];
            ly = node->GetMatrix()->GetRotationMatrix()[4];
            lz = node->GetMatrix()->GetRotationMatrix()[8];
            if (node->GetMatrix()->IsReflection() && TMath::Abs(lx) == 1 &&  TMath::Abs(ly) == 1 && TMath::Abs(lz) == 1) {
                //experimentally found out, that rotation should be updated like this
                if (lx == -1) {
                    zangle = 180;
                }
                if (lz == -1) {
                    xangle = 180;
                }
            }

            //rotation
            Xyz lxyz = GetXYZangles(node->GetMatrix()->GetRotationMatrix());
            lxyz.x -= xangle;
            lxyz.z -= zangle;
            volumeJson += {{"rotation", json::value::array({lxyz.x, lxyz.y, lxyz.z})}};

            json::value childrenJson = json::empty_array;

            //get all nodes in volume
            TObjArray *nodeLst = volume->GetNodes();
            TIter next(nodeLst);
            TGeoNode *childNode;

            //loop through all nodes
            while ((childNode = (TGeoNode *) next()))
            {
                if(childNode && childNode->GetVolume())
                {
                    auto v = ExtractVolumesNew(childNode, childNode->GetVolume(), level+1, path);
                    childrenJson.append({v});

                }
            }

            //volumeJson += {{"children", childrenJson}};

            _jsonByPath[path] = volumeJson;
            return volumeJson;

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


        std::map<std::string, tao::json::value> &GetJsonByPath() {return _jsonByPath;};

    private:
        std::map<std::string, bool> _acceptedPatterns;
        std::map<std::string, bool> _rejectedShapes;
        std::map<std::string, TGeoVolume*> _volumesByPath;
        std::map<std::string, tao::json::value> _jsonByPath;


};
}

#endif //EVESTANDALONE_GEOMETRYCONVERTER_H
