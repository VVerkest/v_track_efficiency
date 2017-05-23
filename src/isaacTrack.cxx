//Program modified from main01.cc in pythia8219 examples folder
//Runs events in a narrow pT hard bin, clusters the final state
//particles with anti-kT, applies some non-stringent cuts,
//and plots histograms/fills trees with & without a tracking
//efficiency selection cut.
//Isaac Mooney - WSU, March 2017 

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
//#include "hist.h"

using namespace fastjet;
using namespace Pythia8;
using namespace std;

int main() {
  // Generator. Process selection. STAR initialization. Histograms.
  
  TH1::SetDefaultSumw2();
  TH2::SetDefaultSumw2();
  TH3::SetDefaultSumw2();

  //seeding pseudorandom number generator
  auto begin = std::chrono::high_resolution_clock::now();

  //generating the distribution
  std::random_device rd;
  std::mt19937 g(rd());
  auto end = std::chrono::high_resolution_clock::now();
  g.seed(std::chrono::duration_cast<std::chrono::nanoseconds>(end-begin).count());
  std::uniform_real_distribution<> dis(0.0, 1.0);

  Pythia pythia;
  pythia.readString("Beams:eCM = 200.");
  pythia.readString("HardQCD:all = on");
  pythia.readString("PhaseSpace:pTHatMin = 20.");
  pythia.readString("PhaseSpace:pTHatMax = 30.");
  pythia.init();

  TFile *fout = new TFile("trackeffic.root","RECREATE");  // Create output file
  TTree *tout = new TTree("pseudojets","pseudojets"); // Create an output tree

  //create branches on the tree
  double px, py, pz, E, phi, eta, pt, lead_pt;
  tout->Branch("px", &px);
  tout->Branch("py", &py);
  tout->Branch("pz", &pz);
  tout->Branch("E", &E);
  tout->Branch("phi", &phi);
  tout->Branch("eta", &eta);
  tout->Branch("lead_pt", &lead_pt);
  tout->Branch("pt", &pt);

  /*
  //create histograms
  histo::histograms* testo = new histo::histograms();
  testo->Init();
  testo->FillKinematics(5);
  TFile* testhist = new TFile("testhist", "RECREATE");
  testhist->cd();
  testo->Write();
  testhist->Close();
  */

    //without cuts (jet level)
  TH1D* uncut_px = new TH1D("u_px","raw jet p_{x}",101,-3,3);
  TH1D* uncut_py = new TH1D("u_py","raw jet p_{y}",101,-3,3);
  TH1D* uncut_pz = new TH1D("u_pz","raw jet p_{z}",101,-3,3);
  TH1D* uncut_E = new TH1D("u_E","raw jet energy",101,-1,10);
  TH1D* uncut_phi = new TH1D("u_phi", "raw jet #phi", 100, 0, 6.3);
  TH1D* uncut_eta = new TH1D("u_eta", "raw jet #eta", 120, -3, 3);
  TH1D* uncut_leadPt = new TH1D("u_leadPt", "raw jet p_{T} lead", 100, 0, 50);
  TH1D* uncut_Pt = new TH1D("u_Pt", "raw jet p_{T}", 100, 0, 40);

  //charged, without cuts (jet level)                                                                                                                                                                
  TH1D* c_uncut_px = new TH1D("c_u_px","charged raw jet p_{x}",101,-3,3);
  TH1D* c_uncut_py = new TH1D("c_u_py","charged raw jet p_{y}",101,-3,3);
  TH1D* c_uncut_pz = new TH1D("c_u_pz","charged raw jet p_{z}",101,-3,3);
  TH1D* c_uncut_E = new TH1D("c_u_E","charged raw jet energy",101,-1,10);
  TH1D* c_uncut_phi = new TH1D("c_u_phi", "charged raw jet #phi", 100, 0, 6.3);
  TH1D* c_uncut_eta = new TH1D("c_u_eta", "charged raw jet #eta", 120, -3, 3);
  TH1D* c_uncut_leadPt = new TH1D("c_u_leadPt", "charged raw jet p_{T} lead", 100, 0, 50);
  TH1D* c_uncut_Pt = new TH1D("c_u_Pt", "charged raw jet p_{T}", 100, 0, 40);
  
  //without cuts (constituent level)                                                                                                                                      
  TH1D* uncut_cons_px = new TH1D("u_cons_px","raw constituent p_{x}",101,-3,3);
  TH1D* uncut_cons_py = new TH1D("u_cons_py","raw constituent p_{y}",101,-3,3);
  TH1D* uncut_cons_pz = new TH1D("u_cons_pz","raw constituent p_{z}",101,-3,3);
  TH1D* uncut_cons_E = new TH1D("u_cons_E","raw constituent energy",101,-1,10);
  TH1D* uncut_cons_phi = new TH1D("u_cons_phi", "raw constituent #phi", 100, 0, 6.3);
  TH1D* uncut_cons_eta = new TH1D("u_cons_eta", "raw constituent #eta", 120, -3, 3);
  TH1D* uncut_cons_Pt = new TH1D("u_cons_Pt", "raw constituent p_{T}", 100, 0, 40);

  //charged, without cuts (constituent level)                                                                                                                                                          
  TH1D* c_uncut_cons_px = new TH1D("c_u_cons_px","charged raw constituent p_{x}",101,-3,3);
  TH1D* c_uncut_cons_py = new TH1D("c_u_cons_py","charged raw constituent p_{y}",101,-3,3);
  TH1D* c_uncut_cons_pz = new TH1D("c_u_cons_pz","charged raw constituent p_{z}",101,-3,3);
  TH1D* c_uncut_cons_E = new TH1D("c_u_cons_E","charged raw constituent energy",101,-1,10);
  TH1D* c_uncut_cons_phi = new TH1D("c_u_cons_phi", "charged raw constituent #phi", 100, 0, 6.3);
  TH1D* c_uncut_cons_eta = new TH1D("c_u_cons_eta", "charged raw constituent #eta", 120, -3, 3);
  TH1D* c_uncut_cons_Pt = new TH1D("c_u_cons_Pt", "charged raw constituent p_{T}", 100, 0, 40);
  
    //with particle cuts
    TH1D* cut_px = new TH1D("cut_px","jet p_{x} (w/ particle #eta cut)",101,-3,3);
    TH1D* cut_py = new TH1D("cut_py","jet p_{y} (w/ particle #eta cut)",101,-3,3);
    TH1D* cut_pz = new TH1D("cut_pz","jet p_{z} (w/ particle #eta cut)",101,-3,3);
    TH1D* cut_E = new TH1D("cut_E","jet energy (w/ particle #eta cut)",101,-1,10);
    TH1D* cut_phi = new TH1D("cut_phi", "jet #phi (w/ particle #eta cut)", 100, 0, 6.3);
    TH1D* cut_eta = new TH1D("cut_eta", "jet #eta (w/ particle #eta cut)", 120, -3, 3);
    TH1D* cut_leadPt = new TH1D("cut_leadPt", "jet lead p_{T} (w/ particle #eta cut)", 100, 0, 50);
    TH1D* cut_Pt = new TH1D("cut_Pt", "jet p_{T} (w/ particle #eta cut)", 100, 0, 40);

    //with particle cuts                                                                                                         
    TH1D* c_cut_px = new TH1D("c_cut_px","charged jet p_{x} (w/ particle #eta cut)",101,-3,3);
    TH1D* c_cut_py = new TH1D("c_cut_py","charged jet p_{y} (w/ particle #eta cut)",101,-3,3);
    TH1D* c_cut_pz = new TH1D("c_cut_pz","charged jet p_{z} (w/ particle #eta cut)",101,-3,3);
    TH1D* c_cut_E = new TH1D("c_cut_E","charged jet energy (w/ particle #eta cut)",101,-1,10);
    TH1D* c_cut_phi = new TH1D("c_cut_phi", "charged jet #phi (w/ particle #eta cut)", 100, 0, 6.3);
    TH1D* c_cut_eta = new TH1D("c_cut_eta", "charged jet #eta (w/ particle #eta cut)", 120, -3, 3);
    TH1D* c_cut_leadPt = new TH1D("c_cut_leadPt", "charged jet lead p_{T} (w/ particle #eta cut)", 100, 0, 50);
    TH1D* c_cut_Pt = new TH1D("c_cut_Pt", "charged jet p_{T} (w/ particle #eta cut)", 100, 0, 40);

    //with jet cuts too
    TH1D* cut2_px = new TH1D("c2_px","jet p_{x} (w/ both #eta cuts)",101,-3,3);
    TH1D* cut2_py = new TH1D("c2_py","jet p_{y} (w/ both #eta cuts)",101,-3,3);
    TH1D* cut2_pz = new TH1D("c2_pz","jet p_{z} (w/ both #eta cuts)",101,-3,3);
    TH1D* cut2_E = new TH1D("c2_E","jet energy (w/ both #eta cuts)",101,-1,10);
    TH1D* cut2_phi = new TH1D("c2_phi", "jet #phi (w/ both #eta cuts)", 100, 0, 6.3);
    TH1D* cut2_eta = new TH1D("c2_eta", "jet #eta (w/ both #eta cuts)", 120, -3, 3);
    TH1D* cut2_leadPt = new TH1D("c2_leadPt", "jet lead p_{T} (w/ both #eta cuts)", 100, 0, 50);
    TH1D* cut2_Pt = new TH1D("c2_Pt", "jet p_{T} (w/ both #eta cuts)", 100, 0, 40);

    //with jet cuts too (charged)
    TH1D* c_cut2_px = new TH1D("c_c2_px","charged jet p_{x} (w/ both #eta cuts)",101,-3,3);
    TH1D* c_cut2_py = new TH1D("c_c2_py","charged jet p_{y} (w/ both #eta cuts)",101,-3,3);
    TH1D* c_cut2_pz = new TH1D("c_c2_pz","charged jet p_{z} (w/ both #eta cuts)",101,-3,3);
    TH1D* c_cut2_E = new TH1D("c_c2_E","charged jet energy (w/ both #eta cuts)",101,-1,10);
    TH1D* c_cut2_phi = new TH1D("c_c2_phi", "charged jet #phi (w/ both #eta cuts)", 100, 0, 6.3);
    TH1D* c_cut2_eta = new TH1D("c_c2_eta", "charged jet #eta (w/ both #eta cuts)", 120, -3, 3);
    TH1D* c_cut2_leadPt = new TH1D("c_c2_leadPt", "charged jet lead p_{T} (w/ both #eta cuts)", 100, 0, 50);
    TH1D* c_cut2_Pt = new TH1D("c_c2_Pt", "charged jet p_{T} (w/ both #eta cuts)", 100, 0, 40);

    //with tracking efficiency selection applied on top of cuts - jet level
    TH1D* effic_px = new TH1D("e_px","jet p_{x} w/ cuts + efficiency",101,-3,3);
    TH1D* effic_py = new TH1D("e_py","jet p_{y} w/ cuts + efficiency",101,-3,3);
    TH1D* effic_pz = new TH1D("e_pz","jet p_{z} w/ cuts + efficiency",101,-3,3);
    TH1D* effic_E = new TH1D("e_E","jet energy w/ cuts + efficiency",101,-1,10);
    TH1D* effic_phi = new TH1D("e_phi", "jet #phi w/ cuts + efficiency", 100, 0, 6.3);
    TH1D* effic_eta = new TH1D("e_eta", "jet #eta w/ cuts + efficiency", 120, -3, 3);
    TH1D* effic_leadPt = new TH1D("e_leadPt", "jet lead p_{T} w/ cuts + efficiency", 100, 0, 50);
    TH1D* effic_Pt = new TH1D("e_Pt", "jet p_{T} w/ cuts + efficiency", 100, 0, 40);
    TH1D* test_ptdiff = new TH1D("e_ptdiff", "jet p_{T} efficiency applied - p_{T} no efficiency", 100, -50, 50);
    TH1D* test_without_leadsublead = new TH1D("e_leadsublead", "jet p_{T} without leading&subleading jets w/ cuts +efficiency", 100, 0, 40);
    
    //charged (denoted c_) fraction only, with tracking efficiency selection applied on top of cuts - jet level
    TH1D* c_effic_px = new TH1D("c_e_px","charged jet p_{x} w/ cuts + efficiency",101,-3,3);
    TH1D* c_effic_py = new TH1D("c_e_py","charged jet p_{y} w/ cuts + efficiency",101,-3,3);
    TH1D* c_effic_pz = new TH1D("c_e_pz","charged jet p_{z} w/ cuts + efficiency",101,-3,3);
    TH1D* c_effic_E = new TH1D("c_e_E","charged jet energy w/ cuts + efficiency",101,-1,10);
    TH1D* c_effic_phi = new TH1D("c_e_phi", "charged jet #phi w/ cuts + efficiency", 100, 0, 6.3);
    TH1D* c_effic_eta = new TH1D("c_e_eta", "charged jet #eta w/ cuts + efficiency", 120, -3, 3);
    TH1D* c_effic_leadPt = new TH1D("c_e_leadPt", "charged jet lead p_{T} w/ cuts + efficiency", 100, 0, 50);
    TH1D* c_effic_Pt = new TH1D("c_e_Pt", "charged jet p_{T} w/ cuts + efficiency", 100, 0, 40);

    TH1D* num_diff_raw = new TH1D("num_diff_raw","number difference of jets raw - with efficiency",100,0,100);
    TH1D* c_num_diff_raw = new TH1D("c_num_diff_raw","number difference of charged jets raw - with efficiency",100,0,100);
    TH1D* num_diff = new TH1D("num_diff","number difference of jets without - with efficiency",100,0,100);
    TH1D* c_num_diff = new TH1D("c_num_diff","number difference of charged jets without - with efficiency",100,0,100);
    
    //HAVE TO CREATE SEPARATE PSEUDOJET VECTORS FOR CHARGED STUFF, SINCE IT WILL LATER BE CLUSTERED SEPARATELY  
    //an event worth of pseudojets
    vector<PseudoJet> uncut_part;
    vector<PseudoJet> cut_part;
    vector<PseudoJet> cut2_part;
    vector<PseudoJet> effic_part;

    //same but for charged pseudojets
    vector<PseudoJet> c_uncut_part;
    vector<PseudoJet> c_cut_part;
    vector<PseudoJet> c_cut2_part;
    vector<PseudoJet> c_effic_part;
  
    //Hist mult("charged multiplicity", 100, -0.5, 799.5);
  
    // choose a jet definition
    double R = 0.4;
    JetDefinition jet_def(antikt_algorithm, R);

    //efficiency (constant for now)
    //double efficiency = 0.8;
  
    // Begin event loop. Generate event. Skip if error. List first one.
    
    for (unsigned iEvent = 0; iEvent < 100000; ++iEvent) {
      uncut_part.clear(); cut_part.clear();
      cut2_part.clear(); effic_part.clear();
      c_uncut_part.clear(); c_cut_part.clear();
      c_cut2_part.clear(); c_effic_part.clear();
      if (!pythia.next()) continue;
      //int nCharged = 0;
      // Find all final particles and fill histograms
      for (unsigned i = 0; i < pythia.event.size(); ++i) {
     	//only select final state particles within particle-level eta cut
        if (pythia.event[i].isFinal() && pythia.event[i].isVisible()) {
	  //save particle charge info for later in fastjet
	  PseudoJet current = PseudoJet(pythia.event[i].px(),pythia.event[i].py(),pythia.event[i].pz(),pythia.event[i].e());
          current.set_user_index(pythia.event[i].isCharged());  
	  uncut_part.push_back(current);
	  if (pythia.event[i].isCharged()) {
	    c_uncut_part.push_back(current);
	  }
	  if (abs(pythia.event[i].eta()) < 1 && pythia.event[i].pT() >= 0.2) {
                //++nCharged;
	      double effic_num = dis(g);
	      if (effic_num > 0.8 && pythia.event[i].isCharged()) {
                    //fill the original jet, skip filling the tracking efficiency applied jet
                    cut_part.push_back(current);
                    cut2_part.push_back(current);
		    c_cut_part.push_back(current);
                    c_cut2_part.push_back(current);
                    continue;
	      }
              else {
		//fill both
		cut_part.push_back(current);
		cut2_part.push_back(current);
		effic_part.push_back(current);
		if (pythia.event[i].isCharged()) {
		  c_cut_part.push_back(current);
		  c_cut2_part.push_back(current);
		  c_effic_part.push_back(current);
		}
	      }
            }
        }
      }
      //HAVE TO CLUSTER CHARGED SEPARATELY

      // run the clustering, extract the jets
      ClusterSequence uncut(uncut_part, jet_def);
      ClusterSequence cut(cut_part, jet_def);
      ClusterSequence cut2(cut2_part, jet_def);
      ClusterSequence effic(effic_part, jet_def);
      vector<PseudoJet> uncut_jets = sorted_by_pt(uncut.inclusive_jets());
      vector<PseudoJet> cut_jets = sorted_by_pt(cut.inclusive_jets());
      vector<PseudoJet> cut2_jets = sorted_by_pt(cut2.inclusive_jets());
      vector<PseudoJet> effic_jets = sorted_by_pt(effic.inclusive_jets());

      ClusterSequence c_uncut(c_uncut_part, jet_def);
      ClusterSequence c_cut(c_cut_part, jet_def);
      ClusterSequence c_cut2(c_cut2_part, jet_def);
      ClusterSequence c_effic(c_effic_part, jet_def);
      vector<PseudoJet> c_uncut_jets = sorted_by_pt(c_uncut.inclusive_jets());
      vector<PseudoJet> c_cut_jets = sorted_by_pt(c_cut.inclusive_jets());
      vector<PseudoJet> c_cut2_jets = sorted_by_pt(c_cut2.inclusive_jets());
      vector<PseudoJet> c_effic_jets = sorted_by_pt(c_effic.inclusive_jets());

      //cout << "Clustering with " << jet_def.description() << '\n';

      //leading jet pt spectra (a) without cuts, (b) with particle cuts, (c) with jet cuts, and (d) with cuts + efficiency

      //(a):
      if (uncut_jets.size() != 0) {
	uncut_leadPt->Fill(uncut_jets[0].pt());
      }

      if (c_uncut_jets.size() != 0) {
	c_uncut_leadPt->Fill(c_uncut_jets[0].pt());
      }

      //HAVE TO DO THESE SEPARATELY ^^

      //(b):
      if (cut_jets.size() != 0) {
	cut_leadPt->Fill(cut_jets[0].pt());
      }

      if (c_cut_jets.size() != 0) {
	c_cut_leadPt->Fill(c_cut_jets[0].pt());
      }

      //(c):
      if(cut2_jets.size() != 0) {
	if (abs(cut2_jets[0].eta()) < (1 - R)) {
	  //leading jet pt spectrum
	  lead_pt = cut2_jets[0].pt();
	  cut2_leadPt->Fill(cut2_jets[0].pt());
	  tout->Fill();
	}
      }

      if (c_cut2_jets.size() != 0 && abs(c_cut2_jets[0].eta()) < (1 - R)) {
	c_cut2_leadPt->Fill(c_cut2_jets[0].pt());
      }

      //make sure not indexing differently
      //(d):
      if (effic_jets.size() != 0) {
	if (abs(effic_jets[0].eta()) < (1 - R)) {
	  effic_leadPt->Fill(effic_jets[0].pt());
	}
      }

      if (c_effic_jets.size() != 0 && (abs(c_effic_jets[0].eta()) < (1 - R))) {
	  c_effic_leadPt->Fill(c_effic_jets[0].pt());
      }

      //pseudojets without cuts
      for (unsigned i = 0; i < uncut_jets.size(); ++ i) {
        uncut_px->Fill(uncut_jets[i].px());
        uncut_py->Fill(uncut_jets[i].py());
        uncut_pz->Fill(uncut_jets[i].pz());
        uncut_E->Fill(uncut_jets[i].e());
        uncut_phi->Fill(uncut_jets[i].phi());
        uncut_eta->Fill(uncut_jets[i].eta());
        uncut_Pt->Fill(uncut_jets[i].pt());
	
	vector<PseudoJet> uncut_cons = uncut_jets[i].constituents(); 
	for (unsigned j = 0; j < uncut_cons.size(); ++ j) {
	  uncut_cons_px->Fill(uncut_cons[j].px());
	  uncut_cons_py->Fill(uncut_cons[j].py());
	  uncut_cons_pz->Fill(uncut_cons[j].pz());
	  uncut_cons_E->Fill(uncut_cons[j].e());
	  uncut_cons_phi->Fill(uncut_cons[j].phi());
	  uncut_cons_eta->Fill(uncut_cons[j].eta());
	  uncut_cons_Pt->Fill(uncut_cons[j].pt());
	}
      }

      //pseudojets without cuts (charged)                                                                                                                  
      for (unsigned i = 0; i < c_uncut_jets.size(); ++ i) {
          c_uncut_px->Fill(c_uncut_jets[i].px());
          c_uncut_py->Fill(c_uncut_jets[i].py());
          c_uncut_pz->Fill(c_uncut_jets[i].pz());
          c_uncut_E->Fill(c_uncut_jets[i].e());
          c_uncut_phi->Fill(c_uncut_jets[i].phi());
          c_uncut_eta->Fill(c_uncut_jets[i].eta());
          c_uncut_Pt->Fill(c_uncut_jets[i].pt());
        vector<PseudoJet> c_uncut_cons = c_uncut_jets[i].constituents();
        for (unsigned j = 0; j < c_uncut_cons.size(); ++ j) {
            c_uncut_cons_px->Fill(c_uncut_cons[j].px());
            c_uncut_cons_py->Fill(c_uncut_cons[j].py());
            c_uncut_cons_pz->Fill(c_uncut_cons[j].pz());
            c_uncut_cons_E->Fill(c_uncut_cons[j].e());
            c_uncut_cons_phi->Fill(c_uncut_cons[j].phi());
            c_uncut_cons_eta->Fill(c_uncut_cons[j].eta());
            c_uncut_cons_Pt->Fill(c_uncut_cons[j].pt());
        }
      }

      //pseudojets with particle level cuts
      for (unsigned i = 0; i < cut_jets.size(); ++ i) {
        cut_px->Fill(cut_jets[i].px());
        cut_py->Fill(cut_jets[i].py());
        cut_pz->Fill(cut_jets[i].pz());
        cut_E->Fill(cut_jets[i].e());
        cut_phi->Fill(cut_jets[i].phi());
        cut_eta->Fill(cut_jets[i].eta());
        cut_Pt->Fill(cut_jets[i].pt());
      }

      for (unsigned i = 0; i < c_cut_jets.size(); ++ i) {
	  c_cut_px->Fill(c_cut_jets[i].px());
	  c_cut_py->Fill(c_cut_jets[i].py());
	  c_cut_pz->Fill(c_cut_jets[i].pz());
	  c_cut_E->Fill(c_cut_jets[i].e());
	  c_cut_phi->Fill(c_cut_jets[i].phi());
	  c_cut_eta->Fill(c_cut_jets[i].eta());
	  c_cut_Pt->Fill(c_cut_jets[i].pt());
      }

      //pseudojets with both sets of cuts
      for (unsigned i = 0; i < cut2_jets.size(); ++ i) {
	//pseudojet axis cut
	if (abs(cut2_jets[i].eta()) < (1 - R)) {
          px = cut2_jets[i].px();
          py = cut2_jets[i].py();
          pz = cut2_jets[i].pz();
          E = cut2_jets[i].e();
          phi = cut2_jets[i].phi();
          eta = cut2_jets[i].eta();
          pt = cut2_jets[i].pt();
          tout->Fill();
          
          cut2_px->Fill(cut2_jets[i].px());
          cut2_py->Fill(cut2_jets[i].py());
          cut2_pz->Fill(cut2_jets[i].pz());
          cut2_E->Fill(cut2_jets[i].e());
          cut2_phi->Fill(cut2_jets[i].phi());
          cut2_eta->Fill(cut2_jets[i].eta());
          cut2_Pt->Fill(cut2_jets[i].pt());
	}
      }

      for (unsigned i = 0; i < c_cut2_jets.size(); ++ i) {
	if ((abs(c_cut2_jets[i].eta()) < (1 - R))) {
	  c_cut2_px->Fill(c_cut2_jets[i].px());
	  c_cut2_py->Fill(c_cut2_jets[i].py());
	  c_cut2_pz->Fill(c_cut2_jets[i].pz());
	  c_cut2_E->Fill(c_cut2_jets[i].e());
	  c_cut2_phi->Fill(c_cut2_jets[i].phi());
	  c_cut2_eta->Fill(c_cut2_jets[i].eta());
	  c_cut2_Pt->Fill(c_cut2_jets[i].pt());
	}
      }
      
      int count_effic = 0, count_c_effic = 0; 
      //pseudojets with cuts + charged particle tracking efficiency selection
      for (unsigned i = 0; i < effic_jets.size(); ++ i) {
        if (abs(effic_jets[i].eta()) < (1 - R)) {
	  ++ count_effic;
	  effic_px->Fill(effic_jets[i].px());
	  effic_py->Fill(effic_jets[i].py());
	  effic_pz->Fill(effic_jets[i].pz());
	  effic_E->Fill(effic_jets[i].e());
	  effic_phi->Fill(effic_jets[i].phi());
	  effic_eta->Fill(effic_jets[i].eta());
	  effic_Pt->Fill(effic_jets[i].pt());
        }
      }
      num_diff_raw->Fill(uncut_jets.size() - count_effic);
      num_diff->Fill(effic_jets.size() - count_effic);
      
      for (unsigned i = 0; i < c_effic_jets.size(); ++ i) {
	if ((abs(c_effic_jets[i].eta()) < (1 - R))) {
	  ++ count_c_effic;
	  c_effic_px->Fill(c_effic_jets[i].px());
	  c_effic_py->Fill(c_effic_jets[i].py());
	  c_effic_pz->Fill(c_effic_jets[i].pz());
	  c_effic_E->Fill(c_effic_jets[i].e());
	  c_effic_phi->Fill(c_effic_jets[i].phi());
	  c_effic_eta->Fill(c_effic_jets[i].eta());
	  c_effic_Pt->Fill(c_effic_jets[i].pt());
	}
      }
      c_num_diff_raw->Fill(c_uncut_jets.size() - count_c_effic);
      c_num_diff->Fill(c_effic_jets.size() - count_c_effic);

      for (unsigned i = 0; i < effic_jets.size(); ++ i) {
	if (abs(effic_jets[i].eta()) < (1 - R)) {
	  if (i != 0 && i != 1) {
	    test_without_leadsublead->Fill(effic_jets[i].pt());
	  }
 	}
      }

      //testing!
      double mindist = 99999;
      double ptdiff = -99999;
      if (effic_jets.size() != 0 ) {
	for (unsigned i = 0; i < cut2_jets.size(); ++ i) {
	  double dist = effic_jets[0].delta_R(cut2_jets[i]);
	  //pseudojet::delta_R pass in another pseudojet to get distance between two in cylindrical (in eta phi)
	  if (dist < mindist) {
	    mindist = dist;
	    ptdiff = effic_jets[0].pt() - cut2_jets[i].pt();
	  }
	}
      }
      if (mindist != 99999) {
	test_ptdiff->Fill(ptdiff);
      }

      //mult.fill( nCharged );
    
      // End of event loop. Statistics. Histogram. Done.
    }

    pythia.stat();
    //cout << mult << '\n';
  
    cout << "Writing to " << fout->GetName() << '\n';
    //save histograms to output root file
    //tout->Print();
    tout->Write();
    fout->Write();
    fout->Close();

    return 0;
}
