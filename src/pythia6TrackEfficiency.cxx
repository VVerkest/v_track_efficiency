//  Veronica Verkest
//  May 3, 2017

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

#include "TStopwatch.h"

// TStarJetPico
#include "TStarJetPicoReader.h"
#include "TStarJetPicoEvent.h"
#include "TStarJetPicoEventHeader.h"
#include "TStarJetPicoEventCuts.h"
#include "TStarJetPicoPrimaryTrack.h"
#include "TStarJetPicoTower.h"
#include "TStarJetPicoTrackCuts.h"
#include "TStarJetPicoTowerCuts.h"
#include "TStarJetVectorContainer.h"
#include "TStarJetVector.h"
#include "TStarJetPicoTriggerInfo.h"
#include "TStarJetPicoUtils.h"

using namespace fastjet;
using namespace std;

int main() {

  string chargeBias = "charged";   //  OPTIONS: "charged" or "all"
  string ptCutStatus = "ptCut";   //  OPTIONS: "ptCut" or "ptUncut"
  string etaCutStatus = "etaCut";   //  OPTIONS: "etaCut" or "etaUncut"
  bool useEfficiency = false;   //  80% efficiency of charged particles

  string outFileName = "out/pythia6TrackEfficiency_";
  string particleSettings = ( chargeBias + "_" + ptCutStatus + "_" + etaCutStatus ).c_str();
  if (useEfficiency == true) { particleSettings += "_efficiency"; }
  outFileName += particleSettings;    outFileName += ".root";
  
  TStopwatch TimeKeeper;  //Start a timer
  TimeKeeper.Start( );
  
  TH1::SetDefaultSumw2( );  // Histograms will calculate gaussian errors
  TH2::SetDefaultSumw2( );
  TH3::SetDefaultSumw2( );

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

  TFile *fout = new TFile( (outFileName).c_str() ,"RECREATE");  // Create ONE output file
  TTree *aout = new TTree("Pythia6Jets","Pythia6Jets"); // Create a raw jet output tree
  TChain* chain = new TChain( "JetTree" );
  chain->Add( "AddedGeantPythia/picoDst_25_35*" );

  string collisionType = "pp";
  TStarJetPicoReader reader;
  transform(collisionType.begin(), collisionType.end(), collisionType.begin(), ::tolower);
    
  reader.SetInputChain( chain );    // set the chain
  // apply hadronic correction - subtract 100% of charged track energy from towers
  reader.SetApplyFractionHadronicCorrection( false );
  //   reader.SetFractionHadronicCorrection( hadronicCorrectionFraction );
  reader.SetRejectTowerElectrons( kFALSE );
    
  TStarJetPicoEventCuts* evCuts = reader.GetEventCuts();

  evCuts->SetRefMultCut( 0 );
    
  // Tracks cuts
  TStarJetPicoTrackCuts* trackCuts = reader.GetTrackCuts();
  trackCuts->SetDCACut( 100 );
  trackCuts->SetMinNFitPointsCut( -1 );
  trackCuts->SetFitOverMaxPointsCut( -1 );
  trackCuts->SetMaxPtCut ( 9999.0 );
    
  std::cout << "Using these track cuts:" << std::endl;
  std::cout << " dca : " << trackCuts->GetDCACut(  ) << std::endl;
  std::cout << " nfit : " <<   trackCuts->GetMinNFitPointsCut( ) << std::endl;
  std::cout << " nfitratio : " <<   trackCuts->GetFitOverMaxPointsCut( ) << std::endl;
  std::cout << " maxpt : " << trackCuts->GetMaxPtCut (  ) << std::endl;
    
  // Towers
  TStarJetPicoTowerCuts* towerCuts = reader.GetTowerCuts();
  towerCuts->SetMaxEtCut( 9999.0 );
  towerCuts->AddBadTowers( "src/Combined_y7_PP_Nick.txt" );

  std::cout << "Using these tower cuts:" << std::endl;
  std::cout << "  GetMaxEtCut = " << towerCuts->GetMaxEtCut() << std::endl;
  std::cout << "  Gety8PythiaCut = " << towerCuts->Gety8PythiaCut() << std::endl;
    
  reader.SetProcessV0s(false);
  reader.Init( -1 ); //runs through all events with -1
  
  TH3D* h_all_EtaPhiPt = new TH3D("all_EtaPhiPt", ";#eta;#phi;p_{T}", 27, -1, 1, 27, -pi, pi, 120, 0, 80 );
  TH3D* h_lead_EtaPhiPt = new TH3D("lead_EtaPhiPt", ";#eta;#phi;p_{T}", 27, -1, 1, 27, -pi, pi, 120, 0, 80 );
  TH3D* h_leadcons_EtaPhiPt = new TH3D("leadcons_EtaPhiPt", ";#eta;#phi;p_{T}", 27, -1, 1, 27, -pi, pi, 120, 0, 80 );
  TH3D* h_allcons_EtaPhiPt = new TH3D("allcons_EtaPhiPt", ";#eta;#phi;p_{T}", 27, -1, 1, 27, -pi, pi, 120, 0, 80 );
  aout->Branch("h_all_EtaPhiPt", &h_all_EtaPhiPt);
  aout->Branch("h_lead_EtaPhiPt", &h_lead_EtaPhiPt);
  aout->Branch("h_leadcons_EtaPhiPt", &h_leadcons_EtaPhiPt);
  aout->Branch("h_allcons_EtaPhiPt", &h_allcons_EtaPhiPt);

  // Data classes
  TStarJetVectorContainer<TStarJetVector>* container;
  TStarJetVector* sv;
  TStarJetPicoEventHeader* header;
  TStarJetPicoEvent* event;
  TClonesArray* towers;

  vector<PseudoJet> all_rawJets;
  int nEvents = 0;
  int nHardJets = 0;
  int nMatchedHard = 0;
  

  while ( reader.NextEvent() ) {       //  BEGIN EVENT LOOP!
    all_rawJets.clear();   //  clear all containers

    nEvents++;
    reader.PrintStatus(10);      // Print out reader status every 10 seconds

    event = reader.GetEvent();           // Get the event header and event
    header = event->GetHeader();

    container = reader.GetOutputContainer();      // Get the output container from the reader

    double vertexZ = header->GetPrimaryVertexZ();      // Find vertex Z bin
    if ( vertexZ > 30 || vertexZ <= -30 ) { continue; }      //    |Vz| <= 30.0 cm

    TStarJetVector* sv;
    for ( int i=0; i < container->GetEntries() ; ++i ) {
      sv = container->Get(i);
      PseudoJet current = PseudoJet( *sv );
      current.set_user_index( sv->GetCharge() );
      double effic_num = dis(g);  // generate random number

      if ( chargeBias == "all") {
	if( ptCutStatus == "ptCut" && current.pt() >= 0.2) {
	  if( etaCutStatus == "etaCut" && abs(current.eta()) <= 1.0 ) {
	    if ( useEfficiency == true && effic_num < 0.8 && sv->IsCharged() == true ) {
	      all_rawJets.push_back(current);      //   all, ptCut, etaCut, & efficiency
	    } else if ( useEfficiency == false ) {
	      all_rawJets.push_back(current);      //   all, ptCut, & etaCut
	    }
	  } else if( etaCutStatus == "etaUncut" ) {
	    if ( useEfficiency == true && effic_num < 0.8 && sv->IsCharged() == true ) {
	      all_rawJets.push_back(current);      //   all, ptCut, etaCut, & efficiency
	    } else if ( useEfficiency == false ) {
	      all_rawJets.push_back(current);      //   all with ptCut
	    }
	  }
	} else if ( ptCutStatus == "ptUncut" ) {
	  if( etaCutStatus == "etaCut" && abs(current.eta()) <= 1.0 ) {
	    if ( useEfficiency == true && effic_num < 0.8 && sv->IsCharged() == true ) {
	      all_rawJets.push_back(current);      //   all, etaCut, & efficiency
	    } else if ( useEfficiency == false ) {
	      all_rawJets.push_back(current);      //   all with etaCut
	    }
	  } else if( etaCutStatus == "etaUncut" ) {
	    if ( useEfficiency == true && effic_num < 0.8 && sv->IsCharged() == true ) {
	      all_rawJets.push_back(current);      //   all, etaCut, & efficiency
	    } else if ( useEfficiency == false ) {
	      all_rawJets.push_back(current);      //   all with ptCut
	    }
	  }
	}
      } else if ( chargeBias == "charged" && sv->IsCharged() == true ) {
	if( ptCutStatus == "ptCut" && current.pt() >= 0.2) {
	  if( etaCutStatus == "etaCut" && abs(current.eta()) <= 1.0 ) {
	    if ( useEfficiency == true && effic_num < 0.8 && sv->IsCharged() == true ) {
	      all_rawJets.push_back(current);      //   charged, ptCut, etaCut, & efficiency
	    } else if ( useEfficiency == false ) {
	      all_rawJets.push_back(current);      //   charged, ptCut, & etaCut
	    }
	  } else if( etaCutStatus == "etaUncut" ) {
	    if ( useEfficiency == true && effic_num < 0.8 && sv->IsCharged() == true ) {
	      all_rawJets.push_back(current);      //   charged, ptCut, etaCut, & efficiency
	    } else if ( useEfficiency == false ) {
	      all_rawJets.push_back(current);      //   charged with ptCut
	    }
	  }
	} else if ( ptCutStatus == "ptUncut" ) {
	  if( etaCutStatus == "etaCut" && abs(current.eta()) <= 1.0 ) {
	    if ( useEfficiency == true && effic_num < 0.8 && sv->IsCharged() == true ) {
	      all_rawJets.push_back(current);      //   charged, etaCut, & efficiency
	    } else if ( useEfficiency == false ) {
	      all_rawJets.push_back(current);      //   charged with etaCut
	    }
	  } else if( etaCutStatus == "etaUncut" ) {
	    if ( useEfficiency == true && effic_num < 0.8 && sv->IsCharged() == true ) {
	      all_rawJets.push_back(current);      //   charged, etaCut, & efficiency
	    } else if ( useEfficiency == false ) {
	      all_rawJets.push_back(current);      //   charged with ptCut
	    }
	  }
	}
      }
    }
    
    ClusterSequence c_all_rawJets(all_rawJets, jet_def);          //  CLUSTER ALL JETS
    vector<PseudoJet> allRawJets = sorted_by_pt(c_all_rawJets.inclusive_jets());     // EXTRACT CLUSTERED JETS


    if (allRawJets.size() != 0) {                     //  Leading Jet
      if ( abs( allRawJets[0].eta() )<=(1.0-R) ) {
	h_lead_EtaPhiPt->Fill(allRawJets[0].eta(), allRawJets[0].phi_std(), allRawJets[0].pt());
	h_lead_EtaPhiPt->SetTitle("Lead Raw Jets");
      
	vector<PseudoJet> rawleadjetcons = allRawJets[0].constituents();     //  Leading Jet Constituents
	for (int j = 0; j < rawleadjetcons.size(); ++ j) {
	  h_leadcons_EtaPhiPt->Fill(rawleadjetcons[j].eta(), rawleadjetcons[j].phi_std(), rawleadjetcons[j].pt());
	  h_leadcons_EtaPhiPt->SetTitle("All Raw Lead Jet Constituents");
	}

	for (int i=0; i<allRawJets.size(); ++i) {
	  h_all_EtaPhiPt->Fill(allRawJets[i].eta(), allRawJets[i].phi_std(), allRawJets[i].pt());
	  h_all_EtaPhiPt->SetTitle("All Raw Jets");
	  vector<PseudoJet> allrawjetcons = allRawJets[i].constituents();     //  Leading Jet Constituents
	  for (int j = 0; j < allrawjetcons.size(); ++ j) {
	    h_allcons_EtaPhiPt->Fill(allrawjetcons[j].eta(), allrawjetcons[j].phi_std(), allrawjetcons[j].pt());
	    h_allcons_EtaPhiPt->SetTitle("All Raw Jet Constituents");
	  }
	}
      }
    }
  }    //  END EVENT LOOP

  aout->Write();
  fout->Write();
  fout->Close();
  
  return 0;
}
