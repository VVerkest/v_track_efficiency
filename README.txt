
	v_track_efficiency
	Veronica Verkest
	May 2017

~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

AFTER CLONING DIRECTORY, PERFORM THESE COMMANDS:

      mkdir src/obj
      mkdir bin
      mkdir out
      mkdir histos
      mkdir histos/pdfs


    * Create a hard or soft link to the 'AddedGeantPythia/'
      data directory (can be copied from RHIC121 Pegasus) in the
      'v_track_efficiency' directory
      
    * 'make' will make all executables
    
    * After running pythia6TrackEfficiency or
      pythia8TrackEfficiency, run the corresponding plotting
      macro with matching settings for cuts, efficiency, etc.
      (jet-finding and particle settings)


~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

JET-FINDING & PARTICLE SETTINGS

string chargeBias = "charged";  // OPTIONS: "charged" or "all"
string ptCutStatus = "ptCut";   // OPTIONS: "ptCut" or "ptUncut"
string etaCutStatus = "etaCut"; // OPTIONS: "etaCut" or "etaUncut"
bool useEfficiency = true; // 80% efficiency of charged particles


~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

EXECUTABLES:

	pythia6TrackEfficiency
	~~~~~~~~~~~~~~~~~~~~~~
   *  read in pythia6 data files from 'AddedGeantPythia'
   *  perform jet-finding
   *  create 3D histograms (pT, eta, phi) for:
      	     - lead jet per event
	     - lead jet constituents
 	     - all jets per event
 	     - constituents of all jets
   *  output histograms as .root files (named by jet-finding &
      particle settings) into 'out/' directory


	pythia8TrackEfficiency
	~~~~~~~~~~~~~~~~~~~~~~
   *  simulate pythia8 data with decays disabled
   *  perform jet-finding
   *  create 3D histograms (pT, eta, phi) for:
      	     - lead jet per event
	     - lead jet constituents
 	     - all jets per event
 	     - constituents of all jets
   *  output histograms as .root files (named by jet-finding &
      particle settings) into 'out/' directory
      

~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
