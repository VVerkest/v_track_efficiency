//  Veronica Verkest
//  April 23, 2017

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
  pythia.readString("PhaseSpace:pTHatMin = 20.");      // min 20.0 GeV
  pythia.readString("PhaseSpace:pTHatMax = 30.");      // max 30.0 GeV

  pythia.readString("111:mayDecay = off");    pythia.readString("211:mayDecay = off");  // turn off decays
  pythia.readString("221:mayDecay = off");  pythia.readString("321:mayDecay = off");  
  pythia.readString("310:mayDecay = off");    pythia.readString("130:mayDecay = off");  
  pythia.readString("3122:mayDecay = off");    pythia.readString("3212:mayDecay = off");  
  pythia.readString("3112:mayDecay = off");    pythia.readString("3222:mayDecay = off");  
  pythia.readString("3312:mayDecay = off");    pythia.readString("3322:mayDecay = off");  
  pythia.readString("3334:mayDecay = off");  
  
  pythia.init();

  TFile *fout = new TFile("out/vtrackefficiency.root","RECREATE");  // Create ONE output file

  TTree *rout = new TTree("phythia8tree","phythia8tree"); // Create a raw output tree
  //create branches on the tree
  double rleadpt, rleadE, rleadetaphi, rpartpt, rpartE, rpartetaphi;
  rout->Branch("rleadpt", &rleadpt);                // LEAD JET PT
  rout->Branch("rleadE", &rleadE);                   // LEAD JET E
  rout->Branch("rleadetaphi", &rleadetaphi);   // LEAD JET ETA-PHI
  rout->Branch("rpartpt", &rpartpt);                // PARTICLE PT
  rout->Branch("rpartE", &rpartE);                   // PARTICLE E
  rout->Branch("rpartetaphi", &rpartetaphi);   // PARTICLE ETA-PHI
  double epleadpt, epleadE, epleadetaphi, eppartpt, eppartE, eppartetaphi;
  rout->Branch("epleadpt", &epleadpt);
  rout->Branch("epleadE", &epleadE);
  rout->Branch("epleadetaphi", &epleadetaphi);
  rout->Branch("eppartpt", &eppartpt);
  rout->Branch("eppartE", &eppartE);
  rout->Branch("eppartetaphi", &eppartetaphi);
  double cutleadpt, cutleadE, cutleadetaphi, cutpartpt, cutpartE, cutpartetaphi;
  rout->Branch("cutleadpt", &cutleadpt);
  rout->Branch("cutleadE", &cutleadE);
  rout->Branch("cutleadetaphi", &cutleadetaphi);
  rout->Branch("cutpartpt", &cutpartpt);
  rout->Branch("cutpartE", &cutpartE);
  rout->Branch("cutpartetaphi", &cutpartetaphi);

  // RAW JETS (NO ETA/PHI REQ. OR EFFICIENCY)
  TH1D* rLeadJetPt = new TH1D( "Rleadjetpt", "Lead Jet p_{T} (no cuts/efficiency)", 100, 0, 50 );
  TH1D* rLeadJetE = new TH1D( "RleadjetE", "Lead Jet E (no cuts/efficiency)", 101, -1, 100 );
  TH2D* rLeadEtaPhi = new TH2D( "Rleadjetetaphi", "Lead Jet #eta #phi (no cuts/efficiency)", 120, -pi, pi, 100, -pi, pi );
  TH1D* rPartPt = new TH1D("Rpartpt", "Particles p_{T} (no cuts/efficiency)", 100, 0, 50 );
  TH1D* rPartE = new TH1D("RpartE", "Particles E (no cuts/efficiency)", 101, -1, 100 );
  TH2D* rPartEtaPhi= new TH2D("Rpartetaphi", "Particles #eta #phi (no cuts/efficiency)", 120, -pi, pi, 100, -pi, pi );

  // JETS MEETING ETA/PT REQUIREMENTS
  TH1D* epLeadJetPt = new TH1D( "EPleadjetpt", "Lead Jet p_{T} (jets meeting #eta and p_{T} req.)", 100, 0, 50 );
  TH1D* epLeadJetE = new TH1D( "EPleadjetE", "Lead Jet E (jets meeting #eta and p_{T} req.)", 80, 0, 80 );
  TH2D* epLeadEtaPhi = new TH2D( "EPleadjetetaphi", "Lead Jet #eta #phi (jets meeting #eta and p_{T} req.)", 40, -pi, pi, 40, -pi, pi );
  TH1D* epPartPt = new TH1D("EPpartpt", "Particles p_{T} (jets meeting #eta and p_{T} req.)", 100, 0, 50 );
  TH1D* epPartE = new TH1D("EPpartE", "Particles E (jets meeting #eta and p_{T} req.)", 80, 0, 60 );
  TH2D* epPartEtaPhi= new TH2D("EPpartetaphi", "Particles #eta #phi (jets meeting #eta and p_{T} req.)", 40, -pi, pi, 40, -pi, pi );

  // ETA-PHI JETS WITH EFFICIENCY CORRECTIONS
  TH1D* cutLeadJetPt = new TH1D( "CUTleadjetpt", "Lead Jet p_{T} (jets meeting #eta and p_{T} req. with efficiency)", 100, 0, 50 );
  TH1D* cutLeadJetE = new TH1D( "CUTleadjetE", "Lead Jet E (jets meeting #eta and p_{T} req. with efficiency)", 80, 0, 80 );
  TH2D* cutLeadEtaPhi = new TH2D( "CUTleadjetetaphi", "Lead Jet #eta #phi (jets meeting #eta and p_{T} req. with efficiency)", 40, -pi, pi, 40, -pi, pi );
  TH1D* cutPartPt = new TH1D("CUTpartpt", "Particles p_{T} (jets meeting #eta and p_{T} req. with efficiency)", 100, 0, 50 );
  TH1D* cutPartE = new TH1D("CUTpartE", "Particles E (jets meeting #eta and p_{T} req. with efficiency)", 80, 0, 60 );
  TH2D* cutPartEtaPhi= new TH2D("CUTpartetaphi", "Particles #eta #phi (jets meeting #eta and p_{T} req. with efficiency)", 40, -pi, pi, 40, -pi, pi );
  
  vector<PseudoJet> vparticles;               //  all particles
  vector<PseudoJet> vparticlesetapt;     //  particles meeting pt/eta requirements
  vector<PseudoJet> vcutparticles;          //  particles meeting pt/eta requirements with efficiency cuts applied

  for (unsigned iEvent = 0; iEvent < 2000; ++iEvent) {    //  BEGIN EVENT LOOP
    
    vparticles.clear(); vparticlesetapt.clear(); vcutparticles.clear();       // clear all containers
    
    if (!pythia.next()) continue;
    
    for (int h = 0; h < pythia.event.size(); ++h) {    // Find all FINAL particles
      
      if (pythia.event[h].isFinal() && pythia.event[h].isVisible()) {      //only select final state particles within particle-level eta cut

	double effic_num = dis(g);  // generate random number
	
	PseudoJet current = PseudoJet(pythia.event[h].px(),pythia.event[h].py(),pythia.event[h].pz(),pythia.event[h].e());	//save particle charge info for later in fastjet
	current.set_user_index(pythia.event[h].isCharged());  
	vparticles.push_back(current);              // save all jets
	
	if (abs(pythia.event[h].eta()) < 1.0 && pythia.event[h].pT() >= 0.2) {
	  vparticlesetapt.push_back(current);
	  
	  if (effic_num < 0.8 && pythia.event[h].isCharged()) {               // efficiency of at least 0.8
	    vcutparticles.push_back(current);
	  }
	}
      }
    }

      ClusterSequence rawjets(vparticles, jet_def);          //  CLUSTER ALL JETS
      vector<PseudoJet> vrawjets = sorted_by_pt(rawjets.inclusive_jets());     // EXTRACT CLUSTERED JETS
      ClusterSequence etaptjets(vparticlesetapt, jet_def);          //  CLUSTER JETS MEETING ETA-pT REQ.
      vector<PseudoJet> vetaptjets = sorted_by_pt(etaptjets.inclusive_jets());     // EXTRACT CLUSTERED JETS  
      ClusterSequence cutjets(vcutparticles, jet_def);          //  CLUSTER JETS MEETING ETA-pT REQ. with efficiency
      vector<PseudoJet> vcutjets = sorted_by_pt(cutjets.inclusive_jets());     // EXTRACT CLUSTERED JETS


      if (vrawjets.size() != 0) {
	cout << "vrawjets.size() :  " << vrawjets.size()<< endl;
	for (int i =0; i< vrawjets.size() ; ++i) {
	  rLeadJetPt->Fill(vrawjets[i].pt());
	  rLeadJetE->Fill(vrawjets[i].e());
	  rLeadEtaPhi->Fill(vrawjets[i].eta(), vrawjets[i].phi_std());

	  vector<PseudoJet> vrawjetcons = vrawjets[i].constituents();
      
	  for (int j = 0; j < vrawjetcons.size(); ++ j) {
	    rPartPt->Fill(vrawjetcons[j].pt());
	    rPartE->Fill(vrawjetcons[j].e());
	    rPartEtaPhi->Fill(vrawjetcons[j].eta(), vrawjetcons[j].phi_std());
	  }
	}
      }
	    
      if (vetaptjets.size() != 0) {
	for ( int i=0; i < vetaptjets.size(); i++) {
	  if( abs(vetaptjets[i].eta()) < (1.0-R) && vetaptjets[i].pt()>=3.0) {

	    epLeadJetPt->Fill(vetaptjets[i].pt());
	    epLeadJetE->Fill(vetaptjets[i].e());
	    epLeadEtaPhi->Fill(vetaptjets[i].eta(), vetaptjets[i].phi_std());
	  
	    vector<PseudoJet> vetaptjetcons = vetaptjets[i].constituents();
	  
	    for (int j = 0; j < vetaptjetcons.size(); ++ j) {
	      epPartPt->Fill(vetaptjetcons[j].pt());
	      epPartE->Fill(vetaptjetcons[j].e());
	      epPartEtaPhi->Fill(vetaptjetcons[j].eta(), vetaptjetcons[j].phi_std());
	    }
	  }
	}
      }

    

      if (vcutjets.size() != 0) {
	for (int i=0; i<vcutjets.size(); ++i) {
	  if( abs(vcutjets[i].eta()) < (1.0-R) && vcutjets[i].pt() >= 3.0) {
	    cutLeadJetPt->Fill(vcutjets[i].pt());
	    cutLeadJetE->Fill(vcutjets[i].e());
	    cutLeadEtaPhi->Fill(vcutjets[i].eta(), vcutjets[i].phi_std());
	  
	    vector<PseudoJet> vcutjetcons = vcutjets[i].constituents();
	  
	    for (int j = 0; j < vcutjetcons.size(); ++ j) {
	      cutPartPt->Fill(vcutjetcons[j].pt());
	      cutPartE->Fill(vcutjetcons[j].e());
	      cutPartEtaPhi->Fill(vcutjetcons[j].eta(), vcutjetcons[j].phi_std());
	    }
	  }
	}
      }
    
  } //end event loop
  
  pythia.stat();

  //save histograms to output root file
  rout->Write();
  fout->Write();
  fout->Close();

  return 0;
}
