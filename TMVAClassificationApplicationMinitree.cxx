
#include <cstdlib>
#include <vector>
#include <iostream>
#include <map>
#include <string>

#include "TFile.h"
#include "TTree.h"
#include "TString.h"
#include "TSystem.h"
#include "TROOT.h"
#include "TStopwatch.h"

#include "TMVA/Reader.h"
#include "TMVA/Config.h"
#include "TMVA/Tools.h"
//#include "TMVA/MethodCuts.h"
#include "Utils/Utils.h"

using namespace std;


int main( int argc, char** argv ) {

  TString tree_name("tree_incl_all");
  
  std::vector<TString> InFiles;
  TString ofn         = argc>1?argv[1]:""; 
  TString infn        = argc>2?argv[2]:""; 
  TString histName    = argc>3?argv[3]:"";
  TString weightfile = argc>4?argv[4]:"";
  TString varfile     = argc>5?argv[5]:"";

    
  if( ofn=="" || infn=="" || weightfile=="" ) error("Must specify input & output file nomes...");
  
  
  printf("================\n");
  printf("  Output file name: %s\n",ofn.Data());
  printf("  input file    : %s\n",infn.Data());
  printf("  histogram     : %s\n",histName.Data());
  printf("  weight file   : %s\n",weightfile.Data());
  printf("  variable file : %s\n",varfile.Data());
  
  
  StrV BDTVars =   put_into_Str_vector(varfile.Data());
  
  for(int i=0;i<BDTVars.size(); i++) 
    cout<<"BDTVars["<<i<<"] = "<<BDTVars[i]<<endl;

  StrV TMVAVars = put_into_Str_vector("./configs/TMVA_vars_minitree.txt");
  for(int i=0;i<TMVAVars.size(); i++) 
    cout<<"TMVAVars["<<i<<"] = "<<TMVAVars[i]<<endl;
  
  //---------------------------------------------------------------
  // Default MVA methods to be trained + tested
  std::map<std::string,int> Use;
  
  // --- Boosted Decision Trees
  Use["BDT"]             = 0; // uses Adaptive Boost
  Use["BDTG"]            = 1; // uses Gradient Boost
  Use["BDTB"]            = 0; // uses Bagging
  Use["BDTD"]            = 0; // decorrelation + Adaptive Boost
  // 
  // ---------------------------------------------------------------
  
  std::map<std::string,int> nIdenticalResults;
  
  std::cout << std::endl;
  std::cout << "==> Start TMVAClassificationApplication" << std::endl;   // --- Create the Reader object
  
  TMVA::Reader *reader = new TMVA::Reader( "!Color:!Silent" );
  

  // Create a set of variables and declare them to the reader
  // - the variable names must corresponds in name and type to
  // those given in the weight file(s) that you use
  
  map<Str,Float_t> BDTVarMap;  map<Str,Float_t> BDTVarMap2;

  for(int ivar=0; ivar<BDTVars.size(); ivar++)
    reader->AddVariable( BDTVars[ivar], &BDTVarMap[ BDTVars[ivar] ]);
    
    
  // --- Book the MVA methods

  TString dir    = "weights/";
  TString prefix = "TMVAClassification";


  // Book method(s)
  for (std::map<std::string,int>::iterator it = Use.begin(); it != Use.end(); it++) {
    if (it->second) {
      TString methodName = TString(it->first) + TString(" method");
      reader->BookMVA( methodName, weightfile ); 
    }
  }
   

  // Book output histograms
  UInt_t nbin = 100;
  TH1F   *histNnC(0), *histNnT(0), *histBdt(0), *histBdtG(0), *histBdtD(0), *histRf(0), *histSVMG(0);
  TH1F   *histFDAMT(0), *histFDAGA(0), *histCat(0), *histPBdt(0);
  TH1F   *histNnC2(0), *histNnT2(0), *histBdt2(0), *histBdtG2(0), *histBdtD2(0), *histRf2(0), *histSVMG2(0);
  TH1F   *histFDAMT2(0), *histFDAGA2(0), *histCat2(0), *histPBdt2(0);

  if (Use["BDT"])           histBdt     = new TH1F( "MVA_BDT",         histName,      nbin, -0.8, 0.8 );
  if (Use["BDTD"])          histBdtD    = new TH1F( "MVA_BDTD",        histName,      nbin, -0.8, 0.8 );
  if (Use["BDTG"])          histBdtG    = new TH1F( "MVA_BDTG",        histName,      nbin, -1.0, 1.0 );
  if (Use["Plugin"])        histPBdt    = new TH1F( "MVA_PBDT",        histName,      nbin, -0.8, 0.8 );

 
  // Prepare input tree (this must be replaced by your data source)
  TFile *input(0);
  if (!gSystem->AccessPathName( infn ))
    input = TFile::Open( infn ); 
   
  if (!input) {
    std::cout << "ERROR: could not open data file" << std::endl;
    exit(1);
  }
  std::cout << "--- TMVAClassificationApp    : Using input file: " << input->GetName() << std::endl;
   
  // --- Event loop

  // Prepare the event tree
  // - here the variable names have to corresponds to your tree
  // - you can use the same variables as above which is slightly faster,
  //   but of course you can use different ones and copy the values inside the event loop
  //

  TFile *target  = new TFile( ofn,"RECREATE" );

  std::cout << "--- Select signal sample" << std::endl;
  TTree* theTree = (TTree*)input->Get(tree_name); 

  if (theTree==NULL) {
    theTree = (TTree*)input->Get("EvtTree");
  }
  if (theTree==NULL) {
    cout << "Input tree not found! Exiting ..." << endl;
    return 0;
  }
  TTree* theOutputTree = new TTree("tree_incl_all","tree_incl_all",-99);

  //Need to save other trees, as well.
  TH1F *xsec                = (TH1F*)input->Get("xsec");
  TH1F *nentries            = (TH1F*)input->Get("nentries");
  TH1F *cumulative_weight   = (TH1F*)input->Get("cumulative_weight");
  TH1F *inverse_xsec_weight = (TH1F*)input->Get("inverse_xsec_weight");
  TH1F *branching_ratio     = (TH1F*)input->Get("inverse_branching_ratio_weight");
  TH1F *sf_to_orig_weight   = (TH1F*)input->Get("sf_to_orig_weight");
  
  map<Str,Float_t> TMVATreeMap;
  map<Str,Int_t>   TMVATreeMap_int;
  for(int ivar=0; ivar<TMVAVars.size(); ivar++) {
    theTree->SetBranchAddress( TMVAVars[ivar], & TMVATreeMap[ TMVAVars[ivar] ]);
    theOutputTree->Branch( TMVAVars[ivar], & TMVATreeMap[ TMVAVars[ivar] ], TMVAVars[ivar]+"2/F");
  }
  
  // Test Variable for output tree - we just take the first one which corresponds to the evID
  //theOutputTree->Branch( TMVAVars[0], & TMVATreeMap[ TMVAVars[0] ], TMVAVars[0]+"2/F");

  Float_t BDTScore;    
  theOutputTree->Branch( histName, &BDTScore, histName+"/F");
   
  // Efficiency calculator for cut method
  Int_t    nSelCutsGA = 0;
  Double_t effS       = 0.9;

  std::vector<Float_t> vecVar(4); // vector for EvaluateMVA tests

  std::cout << "--- Processing: " << theTree->GetEntries() << " events" << std::endl;
  TStopwatch sw;
  sw.Start();
  Int_t nEvent = theTree->GetEntries();
  for (Long64_t ievt=0; ievt<nEvent; ievt++) {

    if (ievt%1000 == 0) std::cout << "--- ... Processing event: " << ievt << std::endl;

    theTree->GetEntry(ievt);

    for(int ivar=0; ivar<TMVAVars.size(); ivar++) {

     for(int jvar=0; jvar<BDTVars.size(); jvar++) 
        if( TMVAVars[ivar] == BDTVars[jvar] ) 
	  BDTVarMap[ BDTVars[jvar] ] = TMVATreeMap[ TMVAVars[ivar] ];
           
    } 
     
    // --- Return the MVA outputs and fill intto histograms
    if (Use["CutsGA"]) {
      // Cuts is a special case: give the desired signal efficienciy
      Bool_t passed = reader->EvaluateMVA( "CutsGA method", effS );
      if (passed) nSelCutsGA++;
    }

    //std::cout<<"eval  =    "<<reader->EvaluateMVA( "BDT method" )<<std::endl;
    if (Use["BDT"          ])   histBdt    ->Fill( reader->EvaluateMVA( "BDT method"           ) );
    if (Use["BDTD"         ])   histBdtD   ->Fill( reader->EvaluateMVA( "BDTD method"          ) );
    if (Use["BDTG"         ])   histBdtG   ->Fill( reader->EvaluateMVA( "BDTG method"          ) );
    if (Use["Plugin"       ])   histPBdt   ->Fill( reader->EvaluateMVA( "P_BDT method"         ) );
    
    if (Use["BDT"          ])   BDTScore  = reader->EvaluateMVA( "BDT method" );
    if (Use["BDTD"         ])   BDTScore  = reader->EvaluateMVA( "BDTD method" );
    if (Use["BDTG"         ])   BDTScore  = reader->EvaluateMVA( "BDTG method" );
    if (Use["Plugin"       ])   BDTScore  = reader->EvaluateMVA( "P_BDT method" );

    
    /* cout << BDTScore << endl; */
    
    theOutputTree->Fill();

  }
  

  // Get elapsed time
  sw.Stop();
  std::cout << "--- End of event loop: "; sw.Print();   // --- Write histograms

  if (Use["BDT"      ])   histBdt    ->Write();
  if (Use["BDTD"     ])   histBdtD   ->Write();
  if (Use["BDTG"     ])   histBdtG   ->Write(); 
  if (Use["Plugin"   ])   histPBdt   ->Write();

  
  theOutputTree->Write();

  //Write additional output trees.
  if (xsec)     xsec->Write();
  if (nentries) nentries->Write();
  if (cumulative_weight)
    cumulative_weight->Write();
  if (inverse_xsec_weight)
    inverse_xsec_weight->Write();
  if (branching_ratio)
    branching_ratio->Write();
  if (sf_to_orig_weight)
    sf_to_orig_weight->Write();

  target->Close();

  std::cout << "--- Created root file containing the MVA output histograms and a new tree with the BDT score" << std::endl;
  
  delete reader;
    
  std::cout << "==> TMVAClassificationApplication is done!" << std::endl << std::endl;
  
}
