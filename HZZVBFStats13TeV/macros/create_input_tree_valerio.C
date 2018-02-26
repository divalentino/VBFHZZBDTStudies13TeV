#include "Math/ProbFuncMathCore.h"
#include "Math/QuantFuncMathCore.h"
#include "TString.h"
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
#include "macros/ComputedHistoSTUncertainty.C"

using namespace std;

void create_input_tree_valerio(TString Input_file = "VBF125_BDT_Tree.root",
			       TString TTree_input = "BDT_Tree",
			       TString Output_file = "vbf_nominal.root",
			       TString Histo_Output_Name="vbf_BDTG",
			       float   lower_cut = -100,
			       float   upper_cut = 100,
			       float   Luminosity_fb = 300.,
			       TString discriminant = "BDTscore",
			       int     pt_bin_threshold = -1,
			       TString category_tag = "1")

{  

  cout << "running on " << Input_file << endl;

  /* HISTO */
  TH1F *histo_output = new TH1F (Histo_Output_Name,Histo_Output_Name,1, 0, 1);
  TFile *f = new TFile(Input_file);

  TTree *tree_input = (TTree*)f->Get(TTree_input);

  float BDTscore;
  float mH_constrained;
  float Mjj, jet1_pt, jet2_pt;
  float sf_weight(1.0);
  float weight;
  float lumi_weight(1.0);

  float jet_pt_cut(30.);  //GeV
  float Mjj_cut   (130.); //GeV

  if (tree_input->GetBranch(discriminant)) {
    tree_input->SetBranchAddress(discriminant, &BDTscore);
  } else {
    std::cout << "Couldn't get branch for discriminant " << discriminant << "!" << std::endl;
    return;
  }

  // tree_input->SetBranchAddress("total_weight",   &weight);
  // tree_input->SetBranchAddress("mH_constrained", &mH_constrained);
  // tree_input->SetBranchAddress("Mjj",            &Mjj);
  // tree_input->SetBranchAddress("jet1_pt",        &jet1_pt);
  // tree_input->SetBranchAddress("jet2_pt",        &jet2_pt);


  if (tree_input->GetBranch("eo_weight")) {
    tree_input->SetBranchAddress("eo_weight",             &weight);
    tree_input->SetBranchAddress("eo_w_lumi",             &lumi_weight);
    tree_input->SetBranchAddress("eo_m4l_constrained",    &mH_constrained);
    tree_input->SetBranchAddress("eo_dijet_invmass",      &Mjj);
    tree_input->SetBranchAddress("eo_leading_jet_pt",     &jet1_pt);
    tree_input->SetBranchAddress("eo_subleading_jet_pt",  &jet2_pt);
  } else {
    tree_input->SetBranchAddress("weight",             &weight);
    tree_input->SetBranchAddress("w_lumi",             &lumi_weight);
    tree_input->SetBranchAddress("m4l_constrained",    &mH_constrained);
    tree_input->SetBranchAddress("dijet_invmass",      &Mjj);
    tree_input->SetBranchAddress("leading_jet_pt",     &jet1_pt);
    tree_input->SetBranchAddress("subleading_jet_pt",  &jet2_pt);
  }

  std::cout << "Using lower cut: " << lower_cut << ", upper cut: " << upper_cut << std::endl;

  Int_t tree_input_nentries = tree_input->GetEntries();

  for (Int_t i=0; i<tree_input_nentries; i++) {
    tree_input->GetEntry(i);

    //Cut on Higgs mass.
    if (mH_constrained < 115 || mH_constrained > 130)
      continue;

    //Cut on jet Pt.
    if (jet1_pt < jet_pt_cut || jet2_pt < jet_pt_cut)
      continue;
    
    //Cut on Mjj.
    if (Mjj < Mjj_cut) continue;

    //Cut on MVA discriminant.
    if(BDTscore>lower_cut && BDTscore<upper_cut) {
      histo_output->Fill(0.5, Luminosity_fb*(weight/lumi_weight));
    }
    
  }

  std::cout << "Final # of events in category: " << histo_output->Integral() << std::endl;

  //Use dynamically generated ST cut from usage of Pt(H+jj).
  //Note that we do this only for the nominal sample, where the
  //QCD scale variation applies.
  if (pt_bin_threshold>0 && Input_file.Contains("gg") && TTree_input == "BDT_Tree") {

    //Calculate ST uncertainty for this BDT region.
    float st_uncert(ComputedHistoSTUncertainty(Input_file,TTree_input,pt_bin_threshold,lower_cut,upper_cut));

    //Write it to file for later usage.
    TString out_st_file = TString("xml/st_uncert_cat_") + category_tag + ".txt";
    ofstream *st_file = new ofstream;
    st_file->open(out_st_file.Data());
    *st_file << st_uncert << endl;
    st_file->close();

  }

  TFile *fout = new TFile(Output_file, "RECREATE");
  histo_output->Write();
  fout->Close();

}
