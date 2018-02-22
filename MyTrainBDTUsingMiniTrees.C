#include <cstdlib>
#include <iostream>
#include <map>
#include <string>

#include "TChain.h"
#include "TFile.h"
#include "TTree.h"
#include "TString.h"
#include "TObjString.h"
#include "TSystem.h"
#include "TROOT.h"

//#include "TMVAGui.C"

#if not defined(__CINT__) || defined(__MAKECINT__)
// needs to be included when makecint runs (ACLIC)
#include "TMVA/Factory.h"
#include "TMVA/Tools.h"
#endif

#define LUMI 20.3
#define do_vbfSgn true
#define do_vhSgn  false
#define doOptimization false

using namespace std;

TString ntup_path = "../files/HiggsTree/13-09-15/";

struct MultiSampleStruct 
{
  MultiSampleStruct() {};
  TFile *file;
  map <TString,TTree*> trees;
};

void MyTrainBDTUsingMiniTrees( TString OutputName   = "MyTest",
			       int     pt_threshold = -1)
{
   cout << OutputName << endl;
   // --------------------------------------------------------------------------------------------------
   // --- This loads the library
   // --------------------------------------------------------------------------------------------------

   TString myMethodList = "BDTG";

   TMVA::Tools::Instance();
   cout << endl << "==> Start TMVAClassification" << endl;

   map<string,int> Use;
   if (myMethodList != "") {
      for (map<string,int>::iterator it = Use.begin(); it != Use.end(); it++) it->second = 0;

      vector<TString> mlist = TMVA::gTools().SplitString( myMethodList, ',' );
      for (UInt_t i=0; i<mlist.size(); i++) {
         string regMethod(mlist[i]);
         Use[regMethod] = 1;
      }
   }

   // --------------------------------------------------------------------------------------------------
   // --- Here the preparation phase begins
   // --------------------------------------------------------------------------------------------------

   // Create a ROOT output file where TMVA will store ntuples, histograms, etc.
   TString outfileName( "rootfiles/" + OutputName + ".root" );
   TFile* outputFile = TFile::Open( outfileName, "RECREATE" );

   vector<TString> v_SGN_name;
   vector<TString> v_BKG_name;

   v_BKG_name.push_back("higgs_signal_mc_training_files/mc15_13TeV.341504.PowhegPythia8EvtGen_CT10_AZNLOCTEQ6L1_ggH124_ZZ4lep_noTau.1fb-1.bin.PtHjj.HiggsTree.root");
   v_BKG_name.push_back("higgs_signal_mc_training_files/mc15_13TeV.341506.PowhegPythia8EvtGen_CT10_AZNLOCTEQ6L1_ggH126_ZZ4lep_noTau.1fb-1.bin.PtHjj.HiggsTree.root");
   v_BKG_name.push_back("higgs_signal_mc_training_files/mc15_13TeV.341508.PowhegPythia8EvtGen_CT10_AZNLOCTEQ6L1_ggH128_ZZ4lep_noTau.1fb-1.bin.PtHjj.HiggsTree.root");

   v_SGN_name.push_back("higgs_signal_mc_training_files/mc15_13TeV.341517.PowhegPythia8EvtGen_CT10_AZNLOCTEQ6L1_VBFH124_ZZ4lep_noTau.1fb-1.bin.PtHjj.HiggsTree.root");
   v_SGN_name.push_back("higgs_signal_mc_training_files/mc15_13TeV.341519.PowhegPythia8EvtGen_CT10_AZNLOCTEQ6L1_VBFH126_ZZ4lep_noTau.1fb-1.bin.PtHjj.HiggsTree.root");
   v_SGN_name.push_back("higgs_signal_mc_training_files/mc15_13TeV.341521.PowhegPythia8EvtGen_CT10_AZNLOCTEQ6L1_VBFH128_ZZ4lep_noTau.1fb-1.bin.PtHjj.HiggsTree.root");
   
   // Create the factory object. Later you can choose the methods whose performance you'd like to investigate. 
   TMVA::Factory *factory = new TMVA::Factory( OutputName, outputFile,
                                               "!V:!Silent:Color:DrawProgressBar:AnalysisType=Classification:Transformations=I" );

   // Apply additional cuts on the signal and background samples
   TCut mySelection = "jet1_pt>30 && jet2_pt>30 && Mjj>130. && higgs_m > 115. && higgs_m < 135."; // Custom one (high mass)
   
   // Variables used
   factory->AddVariable( "Mjj","m_{jj} [MeV]","",'F' );  
   factory->AddVariable( "DelEta_jj","#Delta#eta","",'F' ); 
   factory->AddVariable( "jet1_pt","lead jet p_{T} [MeV]", "",'F' );
   factory->AddVariable( "jet2_pt","sub jet p_{T} [MeV]", "",'F' ); 

   if (OutputName.Contains("Default")) {
     factory->AddVariable( "jet1_eta","jet1_eta","",'F');
   }
   else if (OutputName.Contains("Precut")) {

     //Gives about +2% to Z0.
     mySelection += "jet1_eta*jet2_eta<0";

     //Doesn't seem to help.
     //mySelection += "((higgs_eta < jet1_eta && higgs_eta > jet2_eta) || (higgs_eta > jet1_eta && higgs_eta < jet2_eta))";

     //Gives another 1.something% if around Mjj > 200.
     mySelection += "Mjj >= 200";
     
     factory->AddVariable( "pT_Hjj","pT_Hjj", "",'F' );
     factory->AddVariable( "minDelR_jZ","minDelR_jZ", "",'F' );
     factory->AddVariable( "ZeppetaZZ","ZeppetaZZ", "",'F' );
     
   }
   else if (OutputName.Contains("Bin")) {
     factory->AddVariable( Form("pT_Hjj_bin_%i", pt_threshold), Form("pT_Hjj_bin_%i", pt_threshold), "",'F' );
     factory->AddVariable( "minDelR_jZ","minDelR_jZ", "",'F' );
     factory->AddVariable( "ZeppetaZZ","ZeppetaZZ", "",'F' );
   }
   else if (OutputName.Contains("Test")) {

     factory->AddVariable( "pT_Hjj","pT_Hjj", "",'F' );
     factory->AddVariable( "minDelR_jZ","minDelR_jZ", "",'F' );
     factory->AddVariable( "ZeppetaZZ","ZeppetaZZ", "",'F' );
     
   }
   else if (OutputName.Contains("Third")) {
     mySelection += "jet3_pt>0";
     factory->AddVariable( "pT_Hjj","pT_Hjj", "",'F' );
     factory->AddVariable( "minDelR_jZ","minDelR_jZ", "",'F' );
     factory->AddVariable( "ZeppetaZZ","ZeppetaZZ", "",'F' );
     factory->AddVariable( "Zeppetaj3","Zeppetaj3", "",'F' );
     factory->AddVariable( "jet3_eta", "jet3_eta", "",'F' );
   }

   // Signal individual event weights                                                         
   factory->SetSignalWeightExpression( "weight" );

   // Background individual event weights
   factory->SetBackgroundWeightExpression( "weight" );

   //-------------------------------------------------------------------------  
   // --- Read training and test data 
   //-------------------------------------------------------------------------  

   std::vector<TString> channels;
   channels.push_back("HiggsTree");

   // Adding signal :
   vector<MultiSampleStruct> v_SGN_sample;
   for(unsigned int i=0; i<v_SGN_name.size(); i++) {
     v_SGN_sample.push_back( MultiSampleStruct() );
     v_SGN_sample[i].file  = TFile::Open( v_SGN_name[i] );
     cout << "--- TMVAClassification       : Using input file (SGN): " << v_SGN_sample[i].file->GetName() << endl;
     for (unsigned int j=0; j< channels.size(); j++) {
       v_SGN_sample[i].trees[channels[j]] = (TTree*) v_SGN_sample[i].file->Get( channels[j] );
	 factory->AddSignalTree( v_SGN_sample[i].trees[channels[j]]);
     }
   }
   
   // Adding background :
   vector<MultiSampleStruct> v_BKG_sample;
   for(unsigned int i=0; i<v_BKG_name.size(); i++) {
     v_BKG_sample.push_back( MultiSampleStruct() );
     v_BKG_sample[i].file  = TFile::Open( v_BKG_name[i] );
     cout << "--- TMVAClassification       : Using input file (BKG): " << v_BKG_sample[i].file->GetName() << endl;
     for (unsigned int j=0; j< channels.size(); j++) {
       v_BKG_sample[i].trees[channels[j]] = (TTree*) v_BKG_sample[i].file->Get( channels[j] );
       factory->AddBackgroundTree( v_BKG_sample[i].trees[channels[j]]);
     }
   }

   // ---- Prepare training and test samples
   factory->PrepareTrainingAndTestTree( mySelection, mySelection,
     "nTrain_Signal=0:nTrain_Background=0:nTest_Signal=0:nTest_Background=0:SplitMode=Random:V" );

   // --- VBF :: Boosted Decision Trees 
   if ( Use["BDTG"] ) // Gradient Boost
     factory->BookMethod( TMVA::Types::kBDT, "BDTG",
     			  "!H:!V:NTrees=50:BoostType=Grad:Shrinkage=0.275:nEventsMin=1000:NNodesMax=100000:MaxDepth=5:UseBaggedGrad:UseBaggedBoost:GradBaggingFraction=0.6:nCuts=20:VarTransform=Normalize:PruneMethod=CostComplexity");

   // ---- Optimize the setting (configuration) of the MVAs using the set of training events
   if(doOptimization) factory->OptimizeAllMethodsForClassification();

   // --- Train MVAs using the set of training events
   factory->TrainAllMethods();

   // --- Evaluate all MVAs using the set of test events
   factory->TestAllMethods();

   // ---- Evaluate and compare performance of all configured MVAs
   factory->EvaluateAllMethods();
   // --------------------------------------------------------------

   // --- Save the output
   outputFile->Close();

   cout << "==> Wrote root file: " << outputFile->GetName() << endl;
   cout << "==> TMVAClassification is done!" << endl;

   delete factory;
   
}
