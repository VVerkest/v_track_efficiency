

int main() {

  string chargeBias = "all";   //  OPTIONS: "charged" or "all"
  string ptCutStatus = "ptCut";   //  OPTIONS: "ptCut" or "ptUncut"
  string etaCutStatus = "etaCut";   //  OPTIONS: "etaCut" or "etaUncut"
  bool useEfficiency = true;   //  80% efficiency of charged particles

  if ( chargeBias != "all" && chargeBias != "charged" ) {cerr<<"incorrect chargeBias setting: "<<chargeBias<<"  (OPTIONS: ''charged'' or ''all'')"<<endl; return -1;}
  if ( ptCutStatus != "ptCut" && ptCutStatus != "ptUncut" ) {cerr<<"incorrect ptCutStatus setting: "<<ptCutStatus<<"  (OPTIONS: ''ptCut'' or ''ptUncut'')"<<endl; return -1;}
  if ( etaCutStatus != "etaCut" && etaCutStatus != "etaUncut" ) {cerr<<"incorrect etaCutStatus setting: "<<etaCutStatus<<"  (OPTIONS: ''etaCut'' or ''etaUncut'')"<<endl; return -1;}
  if ( ptCutStatus != "ptCut" && ptCutStatus != "ptUncut" ) {cerr<<"incorrect ptCutStatus setting: "<<ptCutStatus<<"  (OPTIONS: ''ptCut'' or ''ptUncut'')"<<endl; return -1;}

  //  All particles
  if ( chargeBias == "charged" && sv->IsCharged() == false )  { continue; }  // removes neutral particles
  if ( ptCutStatus == "ptCut" && current.pt() < 0.2)                { continue; }  // removes particles below 0.2 GeV
  if ( etaCutStatus == "etaCut" && abs(current.eta()) > 1.0 )   { continue; }  // removes particles with eta>|1|

  if ( useEfficiency == false ) { all_rawJets.push_back(current); }
  else if ( useEfficiency == true ) {                                                 // keep all uncharged particles if chargeBias == "all"
    if ( chargeBias == "all" && sv ->IsCharged() == false ) { all_rawJets.push_back(current); }  // keep all neutral particles
    if ( effic_num < 0.8 && sv->IsCharged() == true ) { all_rawJets.push_back(current); }  // keep 80% of charged particles
  }

  return 0;
}
