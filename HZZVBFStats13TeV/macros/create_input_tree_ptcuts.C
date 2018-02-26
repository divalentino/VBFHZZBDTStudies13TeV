#include "Math/ProbFuncMathCore.h"
#include "Math/QuantFuncMathCore.h"
#include "TFile.h"
#include "TTree.h"
#include "TROOT.h"
#include "TH1F.h"
#include "TF1.h"
#include "TCanvas.h"
#include "TStopwatch.h"
#include <vector>
#include <iostream>
#include <sstream>
#include <iomanip>
#include <TMath.h>
#include <algorithm>

void create_input_tree_ptcuts(TString Input_file = "VBF125_BDT_Tree.root",
			      TString TTree_input = "BDT_Tree",
			      TString Output_file = "vbf_nominal.root",
			      TString Histo_Output_Name="vbf_BDTG",
			      float lower_cut = 0.55,
			      float upper_cut = 100 ,
			      bool  reqPassPtCut = true,
			      float ptCut        = 40.,
			      float Luminosity_fb = 300.,
			      TString Tag_on_category = "1")

{

  //TString Histo_Input_Name="ZZ4l_BDTG"
  cout << "running on " << Input_file << endl;
  float value_for_output = -100;

  //Declare output histogram.
  TH1F *histo_output = new TH1F (Histo_Output_Name,Histo_Output_Name,1, 0, 1);

  ////////////////////////////////////////////////////////////////////////////////

  TFile *f = new TFile(Input_file);

  //TH1F *histo_input = (TH1F*)f->Get(Histo_Input_Name);
  TTree *tree_input = (TTree*)f->Get(TTree_input);

  float BDTscore;
  float sf_weight;
  float weight;
  float pT_Hjj;
  
  tree_input->SetBranchAddress("BDTscore",&BDTscore);
  tree_input->SetBranchAddress("sf_weight",&sf_weight);
  tree_input->SetBranchAddress("weight",&weight);
  tree_input->SetBranchAddress("pT_Hjj",&pT_Hjj);

  float tree_input_nentries = (float)tree_input->GetEntries();
  cout << "# of entries total = " << tree_input_nentries << endl;

  int nBDTEvents(0);
  for (Int_t i=0; i<tree_input_nentries; i++) {
    tree_input->GetEntry(i);
    
    bool ptCategory((reqPassPtCut && pT_Hjj < ptCut) || (!reqPassPtCut && pT_Hjj > ptCut));
    
    if(BDTscore > lower_cut && BDTscore < upper_cut) {
      nBDTEvents++;
      if (ptCategory)
	histo_output->Fill(0.5,Luminosity_fb*sf_weight*weight);
    }
    
  }

  std::cout << "# of events passing BDT cut: " << nBDTEvents << std::endl;
  std::cout << "# of events in histogram:    " << histo_output->GetEntries() << std::endl;
  
  //histo_output->Fill(0.5,value_for_output);

  TFile *fout = new TFile(Output_file, "RECREATE");
  histo_output->Write();
  fout->Close();
  
}
