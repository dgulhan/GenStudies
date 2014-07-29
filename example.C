#include <iostream>
#include <vector>
#include <algorithm>
#include "TCanvas.h"
#include "TError.h"

#include "TFile.h"
#include "TTree.h"
#include "TH1D.h"
#include "TH2D.h"
#include "TCanvas.h"
#include "TNtuple.h"
#include "ntupler/genpart.C"


void example(){ 
 TH1D::SetDefaultSumw2();
 bool doSaveTrackInfo=false;

 TString directory="";
 TString infname="QPythia2";
 
 TFile *outf= new TFile(Form("test_ntuple_%s.root",infname.Data()),"recreate");
 
 TNtuple *nt_gen = new TNtuple("nt_gen","","pt:eta:phi:pt1:pt2:dphi:pdg");

 std:string jetVars="cent:pt1:eta1:phi1:pt2:eta2:phi2:pt3:eta3:phi3:dphi:ptratio";

 TNtuple *nt_jet = new TNtuple("nt_jet","",jetVars.data());

 genjet *fjet = new genjet(Form("%s%s.root",directory.Data(),infname.Data()));
 genpart *fgen = new genpart(Form("%s%s.root",directory.Data(),infname.Data()));
 
 int nentries = fgen->GetEntriesFast();
 for(int jentry=0;jentry<nentries;jentry++){
   if((jentry%1000)==0) std::cout<<jentry<<"/"<<nentries<<std::endl;
   fjet->GetEntry(jentry);
   fgen->GetEntry(jentry);

   float  cent=fgen->b;
   cent=cent*100;
  
   float pt1=-99;
   float phi1=-99;
   float eta1=-99;
   float pt2=-99;
   float phi2=-99;
   float eta2=-99;
   float pt3=-99;
   float phi3=-99;
   float eta3=-99;
   float dphi=-99;
   float ptratio=-99; 

   std::vector<std::pair<double, std::pair<double,double > > >jets; 

  
   int njet=0;
   for(int ijet=0;ijet<fjet->nref;ijet++){
    if(fabs(fjet->jteta[ijet])>2 ||fjet->jtpt[ijet]<30) continue;
    jets.push_back(std::make_pair(fjet->jtpt[ijet],std::make_pair(fjet->jteta[ijet],fjet->jtphi[ijet])));

    njet++;
   } 
  


  std::sort(jets.begin(),jets.end());
  if(njet>0){
   pt1= jets[njet-1].first;
   eta1= jets[njet-1].second.first;
   phi1= jets[njet-1].second.second;
   if(njet>1){
    pt2=jets[njet-2].first;
    eta2=jets[njet-2].second.first;
    phi2=jets[njet-2].second.second;
    dphi=acos(cos(phi1-phi2));
    ptratio=pt2/pt1;
    if(njet>2){
     pt3=jets[njet-3].first;
     eta3=jets[njet-3].second.first;
     phi3=jets[njet-3].second.second;
    }
   }
  }

  for(int itrk=0;itrk<fgen->mult;itrk++){
   
   float eta=fgen->eta[itrk]; 
   float pt=fgen->pt[itrk];

   // if(fgen->chg[itrk]==0)continue;   //to get only charged particles
   float phi=fgen->phi[itrk];
   float pdg=fgen->pdg[itrk];
   
   
   float entry[]={pt,eta,phi,pt1,pt2,dphi,pdg};
   if(doSaveTrackInfo) nt_gen->Fill(entry);

  }
   

  jets.clear();
  float jtentry[]={cent,pt1,eta1,phi1,pt2,eta2,phi2,pt3,eta3,phi3,dphi,ptratio};
  nt_jet->Fill(jtentry);
 }
 

 
 outf->cd();
 if(doSaveTrackInfo){ 
  nt_gen->Write();
 }
 nt_jet->Write();
 
 outf->Close();
}
