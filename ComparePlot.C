void ComparePlot(){
  
  const float pi = 3.14159265358979;  

  //  FIRST INPUT FILE SETTINGS
  string dataType1     = "pythia6";   //  OPTIONS: "pythia6", "p6Geant", or "pythia8"
  string chargeBias1   = "charged";   //  OPTIONS: "charged" or "all"
  string ptCutStatus1  = "ptCut";     //  OPTIONS: "ptCut" or "ptUncut"
  string etaCutStatus1 = "etaCut";    //  OPTIONS: "etaCut" or "etaUncut"
  bool useEfficiency1  =  false;      //  80% efficiency of charged particles

  //  SECOND INPUT FILE SETTINGS
  string dataType2     = "p6Geant";   //  OPTIONS: "pythia6", "p6Geant", or "pythia8"
  string chargeBias2   = "charged";   //  OPTIONS: "charged" or "all"
  string ptCutStatus2  = "ptCut";     //  OPTIONS: "ptCut" or "ptUncut"
  string etaCutStatus2 = "etaCut";    //  OPTIONS: "etaCut" or "etaUncut"
  bool useEfficiency2  =  false;      //  80% efficiency of charged particles

  
  string fileSaveName = "";            // file to be written and saved
  //  name it yourself, lazyass.

  
  string fileSettings1 = ( chargeBias1 + "_" + ptCutStatus1 + "_" + etaCutStatus1 ).c_str();
  if (useEfficiency1 == true) { fileSettings1 += "_efficiency"; }                               // Create file1 import name
  string inFile1 = ( "out/" + dataType1 + "TrackEfficiency_" + fileSettings1 + ".root" ).c_str();
  string fileSettings2 = ( chargeBias2 + "_" + ptCutStatus2 + "_" + etaCutStatus2 ).c_str();
  if (useEfficiency2 == true) { fileSettings2 += "_efficiency"; }                               // Create file2 import name
  string inFile2 = ( "out/" + dataType2 + "TrackEfficiency_" + fileSettings2 + ".root" ).c_str();

  //  gather histograms from first file  
  TFile* CompareFile1 = new TFile( (inFile1).c_str() , "READ" );     // Read in data file 1

  TH3D* h1_all_EtaPhiPt = (TH3D*) CompareFile1->Get("all_EtaPhiPt");
  TH3D* h1_allcons_EtaPhiPt = (TH3D*) CompareFile1->Get("allcons_EtaPhiPt");
  TH3D* h1_lead_EtaPhiPt = (TH3D*) CompareFile1->Get("lead_EtaPhiPt");
  TH3D* h1_leadcons_EtaPhiPt = (TH3D*) CompareFile1->Get("leadcons_EtaPhiPt");

  
  //  gather histograms from second file  
  TFile* CompareFile2 = new TFile( (inFile2).c_str() , "READ" );     // Read in data file 2

  TH3D* h2_all_EtaPhiPt = (TH3D*) CompareFile2->Get("all_EtaPhiPt");
  TH3D* h2_allcons_EtaPhiPt = (TH3D*) CompareFile2->Get("allcons_EtaPhiPt");
  TH3D* h2_lead_EtaPhiPt = (TH3D*) CompareFile2->Get("lead_EtaPhiPt");
  TH3D* h2_leadcons_EtaPhiPt = (TH3D*) CompareFile2->Get("leadcons_EtaPhiPt"); 

  
  //  Project histograms
  TH2D* h1_all_EtaPt = (TH2D*) h1_all_EtaPhiPt->Project3D("ZX");            TH1D* h1_all_Pt = (TH1D*) h1_all_EtaPt->ProjectionY();
  TH2D* h1_allcons_EtaPt = (TH2D*) h1_allcons_EtaPhiPt->Project3D("ZX");    TH1D* h1_allcons_Pt = (TH1D*) h1_allcons_EtaPt->ProjectionY();
  TH2D* h1_lead_EtaPt = (TH2D*) h1_lead_EtaPhiPt->Project3D("ZX");          TH1D* h1_lead_Pt = (TH1D*) h1_lead_EtaPt->ProjectionY();
  TH2D* h1_leadcons_EtaPt = (TH2D*) h1_leadcons_EtaPhiPt->Project3D("ZX");  TH1D* h1_leadcons_Pt = (TH1D*) h1_leadcons_EtaPt->ProjectionY();

  TH2D* h2_all_EtaPt = (TH2D*) h2_all_EtaPhiPt->Project3D("ZX");            TH1D* h2_all_Pt = (TH1D*) h2_all_EtaPt->ProjectionY();
  TH2D* h2_allcons_EtaPt = (TH2D*) h2_allcons_EtaPhiPt->Project3D("ZX");    TH1D* h2_allcons_Pt = (TH1D*) h2_allcons_EtaPt->ProjectionY();
  TH2D* h2_lead_EtaPt = (TH2D*) h2_lead_EtaPhiPt->Project3D("ZX");          TH1D* h2_lead_Pt = (TH1D*) h2_lead_EtaPt->ProjectionY();
  TH2D* h2_leadcons_EtaPt = (TH2D*) h2_leadcons_EtaPhiPt->Project3D("ZX");  TH1D* h2_leadcons_Pt = (TH1D*) h2_leadcons_EtaPt->ProjectionY();










  



  TFile *fout = new TFile( (fileSaveName).c_str() ,"RECREATE");    // create a new Root file

  
  // / / / / / / / / / / / / / / / / / / / / / / / / / / / / / / / / / / / / / / / / / / / / / / / / / / / / / / / / / / / / 


  string pdfSaveName_BASE = ( "histos/pdfs/p6_" + particleSettings + "_" ).c_str();
  string pdfSaveName;
  TCanvas *c1 = new TCanvas("c1", "c1", 800, 600);

  pdfSaveName = (pdfSaveName_BASE + "leadcons_EtaPt.pdf" ).c_str();
  if ( norm == true ) { h_leadcons_EtaPt->DrawNormalized( "COLZ" ); }
  else { h_leadcons_EtaPt->Draw( "COLZ" ); }
  c1->SaveAs( (pdfSaveName).c_str() );

  
  c1->SetLogy();

  
  pdfSaveName = (pdfSaveName_BASE + "all_Pt.pdf" ).c_str();
  if ( norm == true ) { h_all_Pt->DrawNormalized(); }
  else { h_all_Pt->Draw(); }
  c1->SaveAs( (pdfSaveName).c_str() );


  fout->Write();  
  fout->Close();  

}
