void GetEventsMini(TString filename = "bdt_skims_valerio/mc15_ggH125_35.root",
		   TString cuts     = "eo_leading_jet_pt>25 && eo_subleading_jet_pt>25 && eo_dijet_invmass>120",
		   float   int_lumi = 500.)
{

  TFile *file = new TFile(filename, "READ");
  TTree *tree = (TTree*)file->Get("newtree");

  tree->Draw("1>>g",Form("(eo_weight/eo_w_lumi)*%f*(%s)",int_lumi,cuts.Data()));

  TH1F *histo = (TH1F*)gDirectory->Get("g");

  std::cout << "# of events: " << histo->Integral() << std::endl;

}
