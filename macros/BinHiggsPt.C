#include <vector>
#include <iostream>
#include <fstream>
#include "TH1F.h"
#include "TString.h"
#include "TTree.h"
#include "TBranch.h"
#include "TChain.h"
#include "TFile.h"
#include "TChainElement.h"


////////////////////////////////////////////////////////
// NAME:   BinHiggsPt
// INPUT:  HiggsTree file
// OUTPUT: HiggsTree file with binned pT_Hjj, based on
//         an arbitrary threshold.
////////////////////////////////////////////////////////

inline void CopyTH1FObjects(TFile *_file, TFile *_outFile) {

  if (!_file || !_outFile)
    return;

  _outFile->cd();
    
  TList *keys = _file->GetListOfKeys();
  for (int i = 0; i < keys->GetEntries(); i++) {
    TString key_name(keys->At(i)->GetName());
    if (TString(_file->Get(key_name)->ClassName()).Contains("TH1F")) {
      TH1F *clone = (TH1F*)_file->Get(key_name)->Clone();
      clone->Write();
    }
  }
}

TH1F* MergeHistograms(TChain *chain, TString hist_name, int nFiles = -1)
{

  TH1F *histo_add = 0;

  std::vector<TFile*> files;

  TObjArray *fileElements=chain->GetListOfFiles();
  TIter next(fileElements);
  TChainElement *chEl=0;
  while (( chEl=(TChainElement*)next() )) {
    TFile *f = new TFile(chEl->GetTitle());
    files.push_back(f);
  }

  int end_size((nFiles < 0) ? int(files.size()) : nFiles);

  for (int i(0); i < end_size; i++) {
    if (i==0)
      histo_add = (TH1F*)files[i]->Get(hist_name);
    else
      histo_add->Add((TH1F*)files[i]->Get(hist_name));
  }

  return histo_add;

}

void BinHiggsPt(TString filename,
		float cut_min = 20.,
		float cut_max = 60.,
		float cut_interval = 5.)
{

  if (filename=="") {
    std::cout << "Filename is null! Exiting ..." << std::endl;
    return;
  }

  if (!filename.Contains("HiggsTree")) {
    std::cout << "Filename must contain 'HiggsTree'! Exiting ..." << std::endl;
    return;
  }

  ////////////////////////////////////////////////////////////////////////////////
  // File I/O stuff.
  
  bool isFolder(filename.EndsWith(".root") ? false : true);
  
  //Decide whether it's a single file or a folder.
  TTree *EvtTree = 0;
  TFile *infile  = 0;
  if (isFolder) {

    std::cout << "Found a folder!" << std::endl;
    
    //Folder case.
    TChain *chain = new TChain("HiggsTree");
    chain->Add(filename+"/*root*");
    if (!chain->GetEntry(0)) {
      delete chain;
      chain = new TChain("HiggsTree");
      chain->Add(filename+"/*root*");
      if (!chain->GetEntry(0)) return;
    }

    EvtTree = chain;

  } else {

    std::cout << "Found a single file!" << std::endl;
    
    //Single file case.
    infile  = new TFile(filename,"READ");
    if ((TTree*)infile->Get("EvtTree"))
      EvtTree = (TTree*)infile->Get("EvtTree");
    else
      EvtTree = (TTree*)infile->Get("HiggsTree");

  }
  
  if (!EvtTree) {
    std::cout << "Tree/chain not found? Aborting ..." << std::endl;
    return;
  }

  ////////////////////////////////////////////////////////////////////////////////
  // Produce a vector with all relevant cuts.

  TString outfilename(filename);
  outfilename.ReplaceAll("HiggsTree.","bin.PtHjj.HiggsTree.");
  TFile *outfile = new TFile(outfilename,"RECREATE");
  TTree *outTree = EvtTree->CloneTree();
  
  std::cout << "Creating output file: " << outfilename.Data() << std::endl;
  
  std::vector<float>      PtCuts;
  std::vector<TString>    PtCutStrings;
  std::vector<TString>    PtBoolStrings;
  std::vector<TBranch*>   PtCutBranches;
  std::vector<TBranch*>   PtBoolBranches;
  std::map<TString,float> PtValueMap;
  
  for (float cut = cut_min; cut <= cut_max; cut += cut_interval) {
    PtCuts.push_back(cut);
    PtCutStrings .push_back(Form("pT_Hjj_bin_%i",int(cut)));
    //PtBoolStrings.push_back(Form("pT_Hjj_isLT_%i",int(cut)));

    //Initialize map component.
    PtValueMap[Form("pT_Hjj_bin_%i",int(cut))] = 0.;
    
    TBranch *bpt = outTree->Branch(Form("pT_Hjj_bin_%i",int(cut)),
				   &PtValueMap[Form("pT_Hjj_bin_%i",int(cut))],
				   Form("pT_Hjj_bin_%i/F",int(cut)));
    PtCutBranches.push_back(bpt);
    
    // TBranch *bpt_bool = outTree->Branch(Form("pT_Hjj_isLT_%i",int(cut)),
    // 					&PtValueMap[Form("pT_Hjj_isLT_%i",int(cut))],
    // 					Form("pT_Hjj_isLT_%i/F",int(cut)));
    // PtBoolBranches.push_back(bpt_bool);
    
  }
  
  ////////////////////////////////////////////////////////////////////////////////
  // Run loop over bins, and write to output file.
  
  //Duplicate events and add a new branch for the binned Pt(H+jj).
  float   pT_Hjj(0.);

  //Set branch address for unbinned pT_Hjj.
  outTree->SetBranchAddress("pT_Hjj", &pT_Hjj);

  for (int i = 0; i < outTree->GetEntries(); i++) {
    outTree->GetEntry(i);

    for (unsigned int j = 0; j < PtCuts.size(); j++) {
      PtValueMap[PtCutStrings[j]]  = pT_Hjj;
      //PtValueMap[PtBoolStrings[j]] = 0;
      
      if (PtValueMap[PtCutStrings[j]] < PtCuts[j]) {
	PtValueMap[PtCutStrings[j]]  = PtCuts[j];
	//PtValueMap[PtBoolStrings[j]] = 1;
      }
      
      PtCutBranches[j] ->Fill();
      //PtBoolBranches[j]->Fill();
      
    }
  }

  //////////////////////////////////////////
  // Need to write xsec+BR histos to file.
  //////////////////////////////////////////

  //Copy histograms from initial file.
  if (infile)
    CopyTH1FObjects(infile, outfile);
  
  //Write output TTree to file.
  outTree->Write();
  outfile->Close();

  std::cout << "Wrote output file." << std::endl;

}
