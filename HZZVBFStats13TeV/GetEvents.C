void GetEvents(TString filename = "bdt_skims/mc15_13TeV.341505.PowhegPythia8EvtGen_CT10_AZNLOCTEQ6L1_ggH125_ZZ4lep_noTau.1fb-1.BDT_Tree.root",
	       TString cuts     = "jet1_pt>25 && jet2_pt>25 && Mjj>120",
	       float   int_lumi = 500.)
{

  TFile *file = new TFile(filename, "READ");
  TTree *tree = (TTree*)file->Get("BDT_Tree");

  tree->Draw("1>>g",Form("total_weight*%f*(%s)",int_lumi,cuts.Data()));

  TH1F *histo = (TH1F*)gDirectory->Get("g");

  std::cout << "# of events: " << histo->Integral() << std::endl;

}
