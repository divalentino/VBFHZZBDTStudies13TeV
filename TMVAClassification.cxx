
#include <cstdlib>
#include <iostream>
#include <map>
#include <string>
#include <vector>

#include "TChain.h"
#include "TFile.h"
#include "TTree.h"
#include "TString.h"
#include "TObjString.h"
#include "TSystem.h"
#include "TROOT.h"

#include "TMVA/Factory.h"
#include "TMVA/Tools.h"
#include "TMVA/Ranking.h"
#include "TMVA/IMethod.h"
#include "../utils/Utils.h"


using namespace std;

// read input data file with ascii format (otherwise ROOT) ?
Bool_t ReadDataFromAsciiIFormat = kFALSE;

int main( int argc, char** argv )
{
  
  std::vector<TString> InFiles;
  TString ofn  = argc>1?argv[1]:"TMVA.root"; 
  InFiles.push_back( argc>2?argv[2]:"");
  InFiles.push_back( argc>3?argv[3]:"");
  InFiles.push_back( argc>4?argv[4]:"");
  InFiles.push_back( argc>5?argv[5]:"");
  InFiles.push_back( argc>6?argv[6]:"");
  
  // default settings ... can be changed by specifying command line arguments
  TString SplitMode = "Random";
//   TString NumTrees  = "850";
//   TString MinEvents = "150";

  TString NumTrees  = "525";
  TString MinEvents = "150";

  TString weightFN  = "TMVAClassification";
  TString tree_name = "EvtTree";

  for (int i=0;i<=argc;++i) {
    TString arg=argv[i];
    if (arg[0]=='-') {
      if      ( arg=="--splitMode" )       SplitMode = argv[++i];      
      else if ( arg=="--numTree" )         NumTrees  = argv[++i];
      else if ( arg=="--minEvt" )          MinEvents = argv[++i];
      else if ( arg=="--WFN" )             weightFN  = argv[++i];
      else if ( arg=="--treeName" )        tree_name = argv[++i];
      else error("Don't know about argument "+arg);
    }
  }



  printf("================\n");
  printf("  Output file name: %s\n",ofn.Data());
  printf("  input file 1 : %s\n",InFiles[0].Data());
  printf("  input file 2 : %s\n",InFiles[1].Data());
  printf("  input file 3 : %s\n",InFiles[2].Data());
  printf("  input file 4 : %s\n",InFiles[3].Data());
  printf("  input file 5 : %s\n",InFiles[4].Data());
  
  StrV BDTVars = put_into_Str_vector( InFiles[4].Data() );
  
  //StrV BDTVars =   put_into_Str_vector( "../BDT_Vars_jets.txt" );
  for(int i=0;i<BDTVars.size(); i++) cout<<"BDTVars["<<i<<"] = "<<BDTVars[i]<<endl;

  //---------------------------------------------------------------
  // Default MVA methods to be trained + tested
  std::map<std::string,int> Use;
  
  // --- Boosted Decision Trees
  Use["BDT"]             = 0; // uses Adaptive Boost
  Use["BDTG"]            = 1; // uses Gradient Boost
  Use["BDTB"]            = 0; // uses Bagging
  Use["BDTD"]            = 0; // decorrelation + Adaptive Boost
  Use["BDTF"]            = 0; // allow usage of fisher discriminant for node splitting 
  
  
  std::cout << std::endl << "==> Start TMVAClassification" << std::endl;

  bool batchMode(false);
  bool useDefaultMethods(true);

  // --------------------------------------------------------------------------------------------------
  // Create a ROOT output file where TMVA will store ntuples, histograms, etc.

  TString outfileName( ofn );
  TFile* outputFile = TFile::Open( outfileName, "RECREATE" );

  // All TMVA output can be suppressed by removing the "!" (not) in
  // front of the "Silent" argument in the option string
  //TMVA::Factory *factory = new TMVA::Factory( "TMVAClassification", outputFile,
  //                                        "!V:!Silent:Color:DrawProgressBar:Transformations=I;D;P;G,D:AnalysisType=Classification" );
  TMVA::Factory *factory = new TMVA::Factory( weightFN, outputFile,
					      "!V:!Silent:Color:DrawProgressBar:Transformations=I;P;G:AnalysisType=Classification" );

  // If you wish to modify default settings
  // (please check "src/Config.h" to see all available global options)
  //    (TMVA::gConfig().GetVariablePlotting()).fTimesRMS = 8.0;
  //    (TMVA::gConfig().GetIONames()).fWeightFileDir = "myWeightDirectory";


  for(int ivar=0; ivar<BDTVars.size(); ivar++)    factory->AddVariable( BDTVars[ivar],'F' );
 
   
  if(InFiles[3]==""){
   
    TFile *inSig      = TFile::Open( InFiles[0] );
    TFile *inBkg      = TFile::Open( InFiles[1] );
    TTree *signal     = (TTree*)inSig->Get(tree_name);
    TTree *background = (TTree*)inBkg->Get(tree_name);

    if (!signal || !background) {
      tree_name  = "EvtTree";
      signal     = (TTree*)inSig->Get(tree_name);
      background = (TTree*)inBkg->Get(tree_name);
    }
   
    // global event weights per tree (see below for setting event-wise weights)
    Double_t signalWeight     = 1.0;
    Double_t backgroundWeight = 1.0;
     
    // You can add an arbitrary number of signal or background trees
    factory->SetSignalWeightExpression("weight");    
    factory->SetBackgroundWeightExpression("weight");    
    factory->AddSignalTree    ( signal,     signalWeight     ); 
    factory->AddBackgroundTree( background, backgroundWeight );
 
  }else{
     

    TFile *inSigTrain  = TFile::Open( InFiles[0] );
    TFile *inSigTest   = TFile::Open( InFiles[1] );     
    TFile *inBkgTrain  = TFile::Open( InFiles[2] );
    TFile *inBkgTest   = TFile::Open( InFiles[3] );
     
    // --- Register the training and test trees
     
    TTree *signalTrain      = (TTree*)inSigTrain->Get(tree_name);
    TTree *signalTest       = (TTree*)inSigTest->Get(tree_name);
    TTree *backgroundTrain  = (TTree*)inBkgTrain->Get(tree_name);
    TTree *backgroundTest   = (TTree*)inBkgTest->Get(tree_name);
     
    // global event weights per tree (see below for setting event-wise weights)
    Double_t signalTrainWeight  = 1.0;
    Double_t signalTestWeight   = 1.0;
    Double_t bkgTrainWeight     = 1.0;
    Double_t bkgTestWeight      = 1.0;
   
    // You can add an arbitrary number of signal or background trees 
  
    factory->SetSignalWeightExpression("weight");    
    factory->SetBackgroundWeightExpression("weight");    
     
    factory->AddSignalTree( signalTrain, signalTrainWeight, "Training" );
    factory->AddSignalTree( signalTest,  signalTestWeight,  "Test" ); 
    factory->AddBackgroundTree( backgroundTrain, bkgTrainWeight, "Training" );
    factory->AddBackgroundTree( backgroundTest,  bkgTestWeight,  "Test" );

    } 
   

  // Use the following code instead of the above two or four lines to add signal and background
  // training and test events "by hand"
  // NOTE that in this case one should not give expressions (such as "var1+var2") in the input
  //      variable definition, but simply compute the expression before adding the event
  //
  //     // --- begin ----------------------------------------------------------
  //     std::vector<Double_t> vars( 4 ); // vector has size of number of input variables
  //     Float_t  treevars[4], weight;
  //     
  //     // Signal
  //     for (UInt_t ivar=0; ivar<4; ivar++) signal->SetBranchAddress( Form( "var%i", ivar+1 ), &(treevars[ivar]) );
  //     for (UInt_t i=0; i<signal->GetEntries(); i++) {
  //        signal->GetEntry(i);
  //        for (UInt_t ivar=0; ivar<4; ivar++) vars[ivar] = treevars[ivar];
  //        // add training and test events; here: first half is training, second is testing
  //        // note that the weight can also be event-wise
  //        if (i < signal->GetEntries()/2.0) factory->AddSignalTrainingEvent( vars, signalWeight );
  //        else                              factory->AddSignalTestEvent    ( vars, signalWeight );
  //     }
  //   
  //     // Background (has event weights)
  //     background->SetBranchAddress( "weight", &weight );
  //     for (UInt_t ivar=0; ivar<4; ivar++) background->SetBranchAddress( Form( "var%i", ivar+1 ), &(treevars[ivar]) );
  //     for (UInt_t i=0; i<background->GetEntries(); i++) {
  //        background->GetEntry(i);
  //        for (UInt_t ivar=0; ivar<4; ivar++) vars[ivar] = treevars[ivar];
  //        // add training and test events; here: first half is training, second is testing
  //        // note that the weight can also be event-wise
  //        if (i < background->GetEntries()/2) factory->AddBackgroundTrainingEvent( vars, backgroundWeight*weight );
  //        else                                factory->AddBackgroundTestEvent    ( vars, backgroundWeight*weight );
  //     }
  // --- end ------------------------------------------------------------
   
  // Set individual event weights (the variables must exist in the original TTree)
  //    for signal    : factory->SetSignalWeightExpression    ("weight1*weight2");
  //    for background: factory->SetBackgroundWeightExpression("weight1*weight2");
  //factory->SetBackgroundWeightExpression("weight");

  // Apply additional cuts on the signal and background samples (can be different)


  //DEFAULT CASE: NO CUTS.
  TCut mycuts = "";
  TCut mycutb = ""; 

  //mycuts = "jet1_eta*jet2_eta<0";
  //mycutb = "jet1_eta*jet2_eta<0";

  // Tell the factory how to use the training and testing events
  if(InFiles[3]=="")
    factory->PrepareTrainingAndTestTree( mycuts, mycutb,"nTrain_Signal=0:nTrain_Background=0:SplitMode="+SplitMode+":NormMode=NumEvents:!V" );
  else
    factory->PrepareTrainingAndTestTree( mycuts, mycutb ,"");
   
  cout<<"here"<<endl;
  
  
  // Book BDTs

  if (Use["BDT"])  // Adaptive Boost
    factory->BookMethod( TMVA::Types::kBDT, "BDT",
			 "!H:!V:NTrees="+NumTrees+":nEventsMin="+MinEvents+":MaxDepth=3:BoostType=AdaBoost:AdaBoostBeta=0.5:SeparationType=GiniIndex:nCuts=20:PruneMethod=NoPruning" );

  if (Use["BDTG"]) // Gradient Boost
    //Official HSG2 settings.
//     factory->BookMethod(TMVA::Types::kBDT, "BDTG",
// 			"!H:!V:NTrees=50:BoostType=Grad:Shrinkage=0.275:GradBaggingFraction=0.6:nCuts=20:NNodesMax=100000:MaxDepth=5");

    factory->BookMethod(TMVA::Types::kBDT, "BDTG",
			"!H:!V:NTrees=50:BoostType=Grad:Shrinkage=0.275:UseBaggedBoost:GradBaggingFraction=0.6:nCuts=20:NNodesMax=100000:MaxDepth=5");


  if (Use["BDTB"]) // Bagging
    factory->BookMethod( TMVA::Types::kBDT, "BDTB",
			 "!H:!V:NTrees=400:BoostType=Bagging:SeparationType=GiniIndex:nCuts=20:PruneMethod=NoPruning" );

  if (Use["BDTD"]) // Decorrelation + Adaptive Boost
    factory->BookMethod( TMVA::Types::kBDT, "BDTD",
			 "!H:!V:NTrees=400:nEventsMin=400:MaxDepth=3:BoostType=AdaBoost:SeparationType=GiniIndex:nCuts=20:PruneMethod=NoPruning:VarTransform=Decorrelate" );

  if (Use["BDTF"])  // Allow Using Fisher discriminant in node splitting for (strong) linearly correlated variables
    factory->BookMethod( TMVA::Types::kBDT, "BDTMitFisher",
			 "!H:!V:NTrees=50:nEventsMin=150:UseFisherCuts:MaxDepth=3:BoostType=AdaBoost:AdaBoostBeta=0.5:SeparationType=GiniIndex:nCuts=20:PruneMethod=NoPruning" );


  // --------------------------------------------------------------------------------------------------
  // ---- Train, test, and evaluate the MVAs

  // Train BDT using the set of training events
  factory->TrainAllMethods();

  /*
  TMVA::Ranking* CreateRanking();
  TMVA::IMethod *method = factory->GetMethod("BDT");
  const TMVA::Ranking *ranking = method->CreateRanking();
  ranking->Print();
  */

  // ---- Evaluate BDT using the set of test events
  factory->TestAllMethods();
  // ----- Evaluate and compare performance of BDT
  factory->EvaluateAllMethods();



  // --------------------------------------------------------------

  // Save the output
  outputFile->Close();

  std::cout << "==> Wrote root file: " << outputFile->GetName() << std::endl
	    << "==> TMVAClassification is done!" << std::endl
	    << std::endl
	    << "==> To view the results, launch the GUI: \"root -l ./TMVAGui.C\"" << std::endl
	    << std::endl;

  // Clean up
  delete factory;
}

