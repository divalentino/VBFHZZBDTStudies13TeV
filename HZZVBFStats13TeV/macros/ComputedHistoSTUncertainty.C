//#include "Utils.h"

#include "TFile.h"
#include "TTree.h"
#include "TH1F.h"
#include "TCanvas.h"
#include "TStyle.h"

#include <map>
#include <iostream>
#include <cmath>
#include <fstream>

bool useDebug(false);

// typedef std::map<float,float> FFMap;

// void DumpMapContents(FFMap m) {
//   FFMap::iterator mIter = m.begin();
//   std::cout << "Contents of map: " << std::endl;
//   for (; mIter != m.end(); mIter++)
//     std::cout << (*mIter).first << " " << (*mIter).second << std::endl;
// }

float ComputedHistoSTUncertainty(TString file_name="ggF125_BDT_Tree.test.bin.PtHjj.60.root",
				 TString tree_name="BDT_Tree",
				 int pt_thres=60,
				 float bdt_low = 0.3,
				 float bdt_high = 1.)
{

  //SetAtlasStyle();
  
  //FFMap ptAndUncert;

  std::vector<float> pt_values, st_uncerts;
  
  //Step 1: Load in relative ST uncertainties into map.
  float pt_cut(0.), st_read(1.0), st_uncert(-1);
  std::ifstream infile("st_uncerts.txt");
  while (infile >> pt_cut >> st_read) {
    //ptAndUncert[pt_cut] = st_read;
    pt_values.push_back(pt_cut);
    st_uncerts.push_back(st_read);
  }

  //DumpMapContents(ptAndUncert);

  //Step 2: Get histogram of pT(H+jj) in the desired BDT region.
  TFile *in_file = new TFile(file_name,"READ");
  TTree *tree    = (TTree*)in_file->Get(tree_name);
  
  float BDTscore(0.), pT_Hjj(0.), weight(0.);
  // if (pt_thres>0)
  //   tree->SetBranchAddress(TString("pT_Hjj_bin_")+Form("%i",pt_thres), &pT_Hjj);
  // else
  tree->SetBranchAddress("pT_Hjj",   &pT_Hjj);
  tree->SetBranchAddress("BDTscore", &BDTscore);
  tree->SetBranchAddress("weight",   &weight);

  std::cout << "Looping over entries: " << tree->GetEntries() << std::endl;
  
  TH1F *histo = new TH1F("", "", pt_values.size()-1, &pt_values[0]);

  for (int i(0); i < tree->GetEntries(); i++) {
    tree->GetEntry(i);
    if (BDTscore < bdt_low || BDTscore > bdt_high) continue;
    //float pt = (pT_Hjj < float(pt_thres)) ? float(pt_thres) : pT_Hjj;

    float pt = float(pt_thres);
    histo->Fill(pt, weight);

  }

  //Step 3: Compute the uncertainty, ignoring covariances between cross section
  //bins for now, using:
  // \Delta\sigma^2 = \sum (\Delta\sigma_i / \sigma_i)^2 * (Bin_i)^2
  float tot_var(0.), histo_integral(0.);
  for (int i(1); i <= histo->GetNbinsX(); i++) {

    tot_var        += pow(histo->GetBinContent(i),2) * pow(st_uncerts[i-1],2);
    histo_integral += histo->GetBinContent(i);

    histo->SetBinError(i, st_uncerts[i-1] * histo->GetBinContent(i));

    if (useDebug) {
      std::cout << "Bin center: " << histo->GetBinCenter(i) << ", content: " << histo->GetBinContent(i) << ", uncert: " << st_uncerts[i-1] << std::endl;
      std::cout << "Term:       " << pow(histo->GetBinContent(i),2) * pow(st_uncerts[i-1],2) << std::endl;
      std::cout << "Sum:        " << tot_var << std::endl;
    }

  }

  float tot_uncert(sqrt(tot_var) / histo_integral);
  
  std::cout << "Total uncertainty (ignoring cov. terms) in %: " << 100*tot_uncert << std::endl;

  //float tot_uncert(sqrt(tot_var) / histo->Integral());
  if (histo_integral <= 0.) {
    std::cout << "Nonsensical uncertainty - returning 0 ..." << std::endl;
    return 0;
  }

  // TCanvas *c1 = new TCanvas("c1","c1");
  // histo->SetLineColor(kBlack);
  // FormatHistograms(histo,"","p_{THjj} [GeV]","Weighted events");
  // histo->Draw("hist e");
  // //c1->SetLogy();
  // c1->Print("pt.pdf");

  return tot_uncert;

}
