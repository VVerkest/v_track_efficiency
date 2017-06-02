//  Veronica Verkest
//  May 3, 2017

#include "Pythia8/Pythia.h"
#include <fastjet/PseudoJet.hh>
#include <fastjet/ClusterSequence.hh>
#include <fastjet/Selector.hh>
#include "fastjet/tools/Filter.hh"
#include "fastjet/tools/Pruner.hh"
#include <iostream>
#include <sstream>
#include <TROOT.h>
#include <TFile.h>
#include <TTree.h>
#include <TH1.h>
#include <TF1.h>
#include <TH2.h>
#include <TH3.h>
#include <TClonesArray.h>
#include <TLatex.h>
#include <TMathText.h>
#include <random>
#include <chrono>

using namespace fastjet;
using namespace Pythia8;
using namespace std;

int main() {

  string chargeBias = "charged";   //  OPTIONS: "charged" or "all"
  string ptCutStatus = "ptCut";   //  OPTIONS: "ptCut" or "ptUncut"
  string etaCutStatus = "etaCut";   //  OPTIONS: "etaCut" or "etaUncut"
  bool useEfficiency = true;   //  80% efficiency of charged particles

  string outFileName = "out/pythia8TrackEfficiency_";
  string particleSettings = ( chargeBias + "_" + ptCutStatus + "_" + etaCutStatus ).c_str();
  if (useEfficiency == true) { particleSettings += "_efficiency"; }
  outFileName += particleSettings;
  outFileName += ".root";
    
  TH1::SetDefaultSumw2();  // error bars on histos
  TH2::SetDefaultSumw2();
  TH3::SetDefaultSumw2();

  double pi = 3.14159265358979;
  double efficiency = 0.8;

  double R = 0.4;     //  JET DEFINITION
  JetDefinition jet_def(antikt_algorithm, R);

  //seeding pseudorandom number generator
  auto begin = std::chrono::high_resolution_clock::now();

  //generating the distribution
  std::random_device rd;
  std::mt19937 g(rd());
  auto end = std::chrono::high_resolution_clock::now();
  g.seed(std::chrono::duration_cast<std::chrono::nanoseconds>(end-begin).count());
  std::uniform_real_distribution<> dis(0.0, 1.0);
  
  Pythia pythia;
  pythia.readString("Beams:eCM = 200.");      //  200 GeV beam
  pythia.readString("HardQCD:all = on");
  pythia.readString("PhaseSpace:pTHatMin = 25.0");      // min 25.0 GeV
  pythia.readString("PhaseSpace:pTHatMax = 35.0");      // max 35.0 GeV

  //turn off decays
  pythia.readString("111:mayDecay     = off");    //pi0
  pythia.readString("211:mayDecay     = off");    //pi+
  pythia.readString("-211:mayDecay    = off");    //pi-
  pythia.readString("221:mayDecay     = off");    //eta
  pythia.readString("321:mayDecay     = off");    //K+
  pythia.readString("310:mayDecay     = off");    //Kshort
  pythia.readString("130:mayDecay     = off");    //Klong
  pythia.readString("3122:mayDecay    = off");    //Lambda0
  pythia.readString("3212:mayDecay    = off");    //Sigma0
  pythia.readString("3112:mayDecay    = off");    //Sigma-
  pythia.readString("3222:mayDecay    = off");    //Sigma+
  pythia.readString("3312:mayDecay    = off");    //Xi-
  pythia.readString("3322:mayDecay    = off");    //Xi0
  pythia.readString("3334:mayDecay    = off");    //Omega-
  
  pythia.init();

  TFile *fout = new TFile( (outFileName).c_str() ,"RECREATE");  // Create ONE output file
  TTree *Pythia8Jets = new TTree("Pythia8Jets","Pythia8Jets"); // Create a raw jet output tree
  
  // TH3D* h_all_EtaPhiPt = new TH3D("all_EtaPhiPt", ";#eta;#phi;p_{T}", 27, -1, 1, 27, -pi, pi, 120, 0, 80 );
  // TH3D* h_allcons_EtaPhiPt = new TH3D("allcons_EtaPhiPt", ";#eta;#phi;p_{T}", 27, -1, 1, 27, -pi, pi, 120, 0, 80 );
  // TH3D* h_lead_EtaPhiPt = new TH3D("lead_EtaPhiPt", ";#eta;#phi;p_{T}", 27, -1, 1, 27, -pi, pi, 120, 0, 80 );
  // TH3D* h_leadcons_EtaPhiPt = new TH3D("leadcons_EtaPhiPt", ";#eta;#phi;p_{T}", 27, -1, 1, 27, -pi, pi, 120, 0, 80 );
  TH3D* h_all_EtaPhiPt = new TH3D("all_EtaPhiPt", ";#eta;#phi;p_{T}", 27, -pi, pi, 27, -pi, pi, 120, 0, 80 );
  TH3D* h_allcons_EtaPhiPt = new TH3D("allcons_EtaPhiPt", ";#eta;#phi;p_{T}", 27, -pi, pi, 27, -pi, pi, 120, 0, 80 );
  TH3D* h_lead_EtaPhiPt = new TH3D("lead_EtaPhiPt", ";#eta;#phi;p_{T}", 27, -pi, pi, 27, -pi, pi, 120, 0, 80 );
  TH3D* h_leadcons_EtaPhiPt = new TH3D("leadcons_EtaPhiPt", ";#eta;#phi;p_{T}", 27, -pi, pi, 27, -pi, pi, 120, 0, 80 );
  // Pythia8Jets->Branch("h_all_EtaPhiPt", &h_all_EtaPhiPt);
  // Pythia8Jets->Branch("h_lead_EtaPhiPt", &h_lead_EtaPhiPt);
  // Pythia8Jets->Branch("h_leadcons_EtaPhiPt", &h_leadcons_EtaPhiPt);
  // Pythia8Jets->Branch("h_allcons_EtaPhiPt", &h_allcons_EtaPhiPt);
  vector<PseudoJet> all_rawJets;
  vector<PseudoJet> lead_rawJets;

  for (unsigned iEvent = 0; iEvent < 100000; ++iEvent) {    //  BEGIN EVENT LOOP
    if ( iEvent == 50 ) { cout<< "Writing to:  " << outFileName <<endl; }
    if (!pythia.next()) continue;
    
    all_rawJets.clear(); lead_rawJets.clear();   //  clear all containers

    for (int h = 0; h < pythia.event.size(); ++h) {    // loop over all FINAL particles
      
      if (pythia.event[h].isFinal() && pythia.event[h].isVisible()) {      //only select final state particles within particle-level eta cut
	double effic_num = dis(g);  // generate random number
	PseudoJet current = PseudoJet(pythia.event[h].px(),pythia.event[h].py(),pythia.event[h].pz(),pythia.event[h].e());	//save particle charge info for later in fastjet
	current.set_user_index(pythia.event[h].isCharged());

	if ( chargeBias == "all") {
	  if( ptCutStatus == "ptCut" && pythia.event[h].pT() >= 0.2) {
	    if( etaCutStatus == "etaCut" && abs(pythia.event[h].eta()) <= 1.0 ) {
	      if ( useEfficiency == true && effic_num < 0.8 && pythia.event[h].isCharged() ) {
		all_rawJets.push_back(current);      //   all, ptCut, etaCut, & efficiency
	      } else if ( useEfficiency == false ) {
		all_rawJets.push_back(current);      //   all, ptCut, & etaCut
	      }
	    } else if( etaCutStatus == "etaUncut" ) {
	      if ( useEfficiency == true && effic_num < 0.8 && pythia.event[h].isCharged() ) {
		all_rawJets.push_back(current);      //   all, ptCut, etaCut, & efficiency
	      } else if ( useEfficiency == false ) {
		all_rawJets.push_back(current);      //   all with ptCut
	      }
	    }
	  } else if ( ptCutStatus == "ptUncut" ) {
	    if( etaCutStatus == "etaCut" && abs(pythia.event[h].eta()) <= 1.0 ) {
	      if ( useEfficiency == true && effic_num < 0.8 && pythia.event[h].isCharged() ) {
		all_rawJets.push_back(current);      //   all, etaCut, & efficiency
	      } else if ( useEfficiency == false ) {
		all_rawJets.push_back(current);      //   all with etaCut
	      }
	    } else if( etaCutStatus == "etaUncut" ) {
	      if ( useEfficiency == true && effic_num < 0.8 && pythia.event[h].isCharged() ) {
		all_rawJets.push_back(current);      //   all, etaCut, & efficiency
	      } else if ( useEfficiency == false ) {
		all_rawJets.push_back(current);      //   all with ptCut
	      }
	    }
	  }
	} else if ( chargeBias == "charged" && pythia.event[h].isCharged() ) {
	  if( ptCutStatus == "ptCut" && pythia.event[h].pT() >= 0.2) {
	    if( etaCutStatus == "etaCut" && abs(pythia.event[h].eta()) <= 1.0 ) {
	      if ( useEfficiency == true && effic_num < 0.8 && pythia.event[h].isCharged() ) {
		all_rawJets.push_back(current);      //   charged, ptCut, etaCut, & efficiency
	      } else if ( useEfficiency == false ) {
		all_rawJets.push_back(current);      //   charged, ptCut, & etaCut
	      }
	    } else if( etaCutStatus == "etaUncut" ) {
	      if ( useEfficiency == true && effic_num < 0.8 && pythia.event[h].isCharged() ) {
		all_rawJets.push_back(current);      //   charged, ptCut, etaCut, & efficiency
	      } else if ( useEfficiency == false ) {
		all_rawJets.push_back(current);      //   charged with ptCut
	      }
	    }
	  } else if ( ptCutStatus == "ptUncut" ) {
	    if( etaCutStatus == "etaCut" && abs(pythia.event[h].eta()) <= 1.0 ) {
	      if ( useEfficiency == true && effic_num < 0.8 && pythia.event[h].isCharged() ) {
		all_rawJets.push_back(current);      //   charged, etaCut, & efficiency
	      } else if ( useEfficiency == false ) {
		all_rawJets.push_back(current);      //   charged with etaCut
	      }
	    } else if( etaCutStatus == "etaUncut" ) {
	      if ( useEfficiency == true && effic_num < 0.8 && pythia.event[h].isCharged() ) {
		all_rawJets.push_back(current);      //   charged, etaCut, & efficiency
	      } else if ( useEfficiency == false ) {
		all_rawJets.push_back(current);      //   charged with ptCut
	      }
	    }
	  }
	}
	
      }
    }    //  end particle loop
    
    ClusterSequence c_all_rawJets(all_rawJets, jet_def);          //  CLUSTER ALL JETS
    vector<PseudoJet> allRawJets = sorted_by_pt(c_all_rawJets.inclusive_jets());     // EXTRACT CLUSTERED JETS


    if (allRawJets.size() != 0) {                     //  Leading Jet
      if ( abs( allRawJets[0].eta() )<=(1.0-R) ) {
	h_lead_EtaPhiPt->Fill(allRawJets[0].eta(), allRawJets[0].phi_std(), allRawJets[0].pt());
	h_lead_EtaPhiPt->SetTitle("Lead Raw Jets");
      
	vector<PseudoJet> rawleadjetcons = allRawJets[0].constituents();     //  Leading Jet Constituents
	for (int j = 0; j < rawleadjetcons.size(); ++ j) {
	  if ( abs( rawleadjetcons[j].eta() )<=(1.0) ) {
	    h_leadcons_EtaPhiPt->Fill(rawleadjetcons[j].eta(), rawleadjetcons[j].phi_std(), rawleadjetcons[j].pt());
	    h_leadcons_EtaPhiPt->SetTitle("All Raw Lead Jet Constituents");
	  }
	}

	for (int i=0; i<allRawJets.size(); ++i) {
	  h_all_EtaPhiPt->Fill(allRawJets[i].eta(), allRawJets[i].phi_std(), allRawJets[i].pt());
	  h_all_EtaPhiPt->SetTitle("All Raw Jets");
	  vector<PseudoJet> allrawjetcons = allRawJets[i].constituents();     //  Leading Jet Constituents
	  for (int j = 0; j < allrawjetcons.size(); ++ j) {
	    if ( abs( allrawjetcons[j].eta() )<=(1.0) ) {
	      h_allcons_EtaPhiPt->Fill(allrawjetcons[j].eta(), allrawjetcons[j].phi_std(), allrawjetcons[j].pt());
	      h_allcons_EtaPhiPt->SetTitle("All Raw Jet Constituents");
	    }
	  }
	}
      }
    }
  }    //  END EVENT LOOP

  pythia.stat();

  //  Pythia8Jets->Write();
  fout->Write();
  //  fout->Close();
  
  return 0;
}
