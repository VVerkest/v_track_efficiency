void Pythia6Plot(){
  
  const float pi = 3.141592;

  string chargeBias = "all";   //  OPTIONS: "charged" or "all"
  string ptCutStatus = "ptCut";   //  OPTIONS: "ptCut" or "ptUncut"
  string etaCutStatus = "etaCut";   //  OPTIONS: "etaCut" or "etaUncut"
  bool useEfficiency = true;   //  80% efficiency of charged particles
  bool norm = true;   //  Normalize histograms by number of events

  string particleSettings = ( chargeBias + "_" + ptCutStatus + "_" + etaCutStatus ).c_str();
  if (useEfficiency == true) { particleSettings += "_efficiency"; }
  
  string inFileName = "out/pythia6TrackEfficiency_";                   // file to read in
  inFileName += particleSettings;    inFileName += ".root";
  
  string fileSaveName = "histos/p6/pythia6TrackEfficiency_";            // file to be written and saved
  //  if (norm == true) { particleSettings += "_NORM"; }
  fileSaveName += particleSettings;    fileSaveName += ".root";

  TFile* pythia6FILE = new TFile( (inFileName).c_str() , "READ" );     // Read in data file

  TFile *fout = new TFile( (fileSaveName).c_str() ,"RECREATE");    // create a new Root file

  //  gather histograms
  TH3D* h_all_EtaPhiPt = (TH3D*) pythia6FILE->Get("all_EtaPhiPt");
  TH3D* h_allcons_EtaPhiPt = (TH3D*) pythia6FILE->Get("allcons_EtaPhiPt");
  TH3D* h_lead_EtaPhiPt = (TH3D*) pythia6FILE->Get("lead_EtaPhiPt");
  TH3D* h_leadcons_EtaPhiPt = (TH3D*) pythia6FILE->Get("leadcons_EtaPhiPt"); 

  //  Project histograms
  TH2D* h_all_EtaPt = (TH2D*) h_all_EtaPhiPt->Project3D("ZX");                           h_all_EtaPt->SetName("all_EtaPt");
  TH2D* h_allcons_EtaPt = (TH2D*) h_allcons_EtaPhiPt->Project3D("ZX");             h_allcons_EtaPt->SetName("allcons_EtaPt");
  TH1D* h_all_Pt = (TH1D*) h_all_EtaPt->ProjectionY();                                         h_all_Pt->SetName("all_Pt");
  TH1D* h_allcons_Pt = (TH1D*) h_allcons_EtaPt->ProjectionY();                           h_allcons_Pt->SetName("allcons_Pt");
  
  TH2D* h_lead_EtaPt = (TH2D*) h_lead_EtaPhiPt->Project3D("ZX");                      h_lead_EtaPt->SetName("lead_EtaPt");
  TH2D* h_leadcons_EtaPt = (TH2D*) h_leadcons_EtaPhiPt->Project3D("ZX");        h_leadcons_EtaPt->SetName("leadcons_EtaPt");
  TH1D* h_lead_Pt = (TH1D*) h_lead_EtaPt->ProjectionY();                                    h_lead_Pt->SetName("lead_Pt");
  TH1D* h_leadcons_Pt = (TH1D*) h_leadcons_EtaPt->ProjectionY();                      h_leadcons_Pt->SetName("leadcons_Pt");

  string pdfSaveName_BASE = ( "histos/pdfs/" + particleSettings + "/p6_" + particleSettings + "_" ).c_str();
  string pdfSaveName;
  TCanvas *c1 = new TCanvas("c1", "c1", 800, 600);

  //  save 2D & 3D histograms as .pdf images in "histos/pdfs"
  pdfSaveName = (pdfSaveName_BASE + "all_EtaPt.pdf" ).c_str();
  if ( norm == true) { h_all_EtaPt->DrawNormalized( "COLZ" ); }
  else { h_all_EtaPt->Draw( "COLZ" ); }
  c1->SaveAs( (pdfSaveName).c_str() );
  
  pdfSaveName = (pdfSaveName_BASE + "allcons_EtaPt.pdf" ).c_str();
  if ( norm == true) { h_allcons_EtaPt->DrawNormalized( "COLZ" ); }
  else { h_allcons_EtaPt->Draw( "COLZ" ); }
  c1->SaveAs( (pdfSaveName).c_str() );
  
  pdfSaveName = (pdfSaveName_BASE + "lead_EtaPt.pdf" ).c_str();
  if ( norm == true) { h_lead_EtaPt->DrawNormalized( "COLZ" ); }
  else { h_lead_EtaPt->Draw( "COLZ" ); }
  c1->SaveAs( (pdfSaveName).c_str() );
  
  pdfSaveName = (pdfSaveName_BASE + "leadcons_EtaPt.pdf" ).c_str();
  if ( norm == true ) { h_leadcons_EtaPt->DrawNormalized( "COLZ" ); }
  else { h_leadcons_EtaPt->Draw( "COLZ" ); }
  c1->SaveAs( (pdfSaveName).c_str() );

  
  c1->SetLogy();

  
  pdfSaveName = (pdfSaveName_BASE + "all_Pt.pdf" ).c_str();
  if ( norm == true ) { h_all_Pt->DrawNormalized(); }
  else { h_all_Pt->Draw(); }
  c1->SaveAs( (pdfSaveName).c_str() );

  pdfSaveName = (pdfSaveName_BASE + "allcons_Pt.pdf" ).c_str();
  if ( norm == true ) { h_allcons_Pt->DrawNormalized(); }
  else { h_allcons_Pt->Draw(); }
  c1->SaveAs( (pdfSaveName).c_str() );
  
  pdfSaveName = (pdfSaveName_BASE + "lead_Pt.pdf" ).c_str();
  if ( norm == true ) { h_lead_Pt->DrawNormalized(); }
  else { h_lead_Pt->Draw(); }
  c1->SaveAs( (pdfSaveName).c_str() );
  
  pdfSaveName = (pdfSaveName_BASE + "leadcons_Pt.pdf" ).c_str();
  if ( norm == true ) { h_leadcons_Pt->DrawNormalized(); }
  else { h_leadcons_Pt->Draw(); }
  c1->SaveAs( (pdfSaveName).c_str() );  


  fout->Write();  
  fout->Close();  

}
