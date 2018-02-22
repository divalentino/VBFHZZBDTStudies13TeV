// AUTHOR: VALERIO BORTOLOTTO 8-April-2015
#include "TFile.h"
#include "TROOT.h"
#include "TH1F.h"
#include "TF1.h"
#include "TCanvas.h"
#include "TStopwatch.h"
#include "RooWorkspace.h"
#include "RooAbsData.h"
#include "RooRandom.h"
#include "RooRealSumPdf.h"
#include "RooStats/ModelConfig.h"
#include "RooStats/ToyMCImportanceSampler.h"
#include "RooStats/SamplingDistribution.h"
#include "RooStats/RatioOfProfiledLikelihoodsTestStat.h"
#include "RooStats/AsymptoticCalculator.h"
#include "RooStats/FrequentistCalculator.h"
#include <vector>
#include "RooNLLVar.h"
#include "RooRealVar.h"
#include "RooDataSet.h"
#include "RooMinimizer.h"
#include "RooProfileLL.h"
#include "RooStats/RooStatsUtils.h"
#include <iostream>
#include <sstream>
#include <iomanip>
#include <TMath.h>
#include <set>
#include <sstream>
#include <fstream>
#include <utility>
#include <map>
#include <string>

using namespace RooFit;
using namespace RooStats;

std::pair<float,float> p0_calculation(TString wsInput = "output_combined_datastat_model.root" , TString workspace="combined" , TString model="ModelConfig" , TString dataName="asimovData" /*"obsData" or "asimovData"*/, bool asymmetric_error = false){

  float error_up = 0.;
  float error_down = 0.;
  float LIKELIHOOD_cond = -100000.;
  float LIKELIHOOD_null = 100000.;
  float status_nll_SM_hypo = 12.;
  float status_nll_null_hypo = 12.;
  float significance = 120.;
  float test_statistic = 100.;

  TFile* f = new TFile(wsInput);

  RooWorkspace* w = (RooWorkspace*) f->Get(workspace);
  ModelConfig* mc = (ModelConfig*) w->obj(model);
  RooAbsPdf* pdf = mc->GetPdf();
  RooArgSet* NP =(RooArgSet*)  mc->GetGlobalObservables();
  RooArgSet* pois = (RooArgSet*) mc->GetParametersOfInterest();
  RooDataSet* data = (RooDataSet*) w->data(dataName);
  RooRealVar* mu_vbf = (RooRealVar*)mc->GetParametersOfInterest()->first();

  const RooArgSet* nuis =  mc->GetNuisanceParameters();
  //    RooArgSet * Par_Original= (RooArgSet*) mc->GetNuisanceParameters();
  //    Par_Original->add(*mu_vbf);
  RooArgSet nuisAndPOI_Original(*mc->GetNuisanceParameters(),*mu_vbf);

  RooRealVar* lumi  = NULL;
  lumi = w->var("Lumi");
  lumi->setVal(1);
  lumi->setConstant(true);

  w->saveSnapshot("SM_hypo",nuisAndPOI_Original);

  /* Starting fitting nll_cond */
  w->loadSnapshot("SM_hypo");
  RooAbsReal* nll_cond = mc->GetPdf()->createNLL(*data, RooFit::Constrain( *mc->GetNuisanceParameters() ), RooFit::GlobalObservables(*mc->GetGlobalObservables()), RooFit::Offset(true) );
  if(!asymmetric_error){
    RooMinimizer SM_HYPO (*nll_cond);
    SM_HYPO.setPrintLevel(2);
    SM_HYPO.setStrategy(1);
    status_nll_SM_hypo = SM_HYPO.minimize("Minuit2");
    //   LIKELIHOOD_cond = nll_cond->getVal();
  }
  if(asymmetric_error){
    RooMinuit m(*nll_cond) ;
    m.setVerbose(kFALSE);
    m.setStrategy(2);
    // m.migrad();
    // m.hesse();
    m.minos() ;
  }

  LIKELIHOOD_cond = nll_cond->getVal();

  error_up = mu_vbf->getErrorHi();
  error_down = mu_vbf->getErrorLo();

  w->loadSnapshot("SM_hypo");
  mu_vbf->setVal(0);
  mu_vbf->setConstant(true);

  RooAbsReal* nll_null = mc->GetPdf()->createNLL(*data, RooFit::Constrain( *mc->GetNuisanceParameters() ), RooFit::GlobalObservables(*mc->GetGlobalObservables()), RooFit::Offset(true) );
  if(!asymmetric_error){
    RooMinimizer NULL_HYPO (*nll_null);
    NULL_HYPO.setPrintLevel(2);
    NULL_HYPO.setStrategy(1);
    status_nll_null_hypo = NULL_HYPO.minimize("Minuit2");
  }

  if(asymmetric_error){
    RooMinuit m_null(*nll_null) ;
    m_null.setVerbose(kFALSE);
    m_null.setStrategy(2);
    // m.migrad();
    // // m.hesse();
    m_null.minos() ;
  }

  LIKELIHOOD_null = nll_null->getVal();


  cout << "result is calculated using the dataset: " << dataName << endl;

  test_statistic =2*(LIKELIHOOD_null - LIKELIHOOD_cond);
  if(test_statistic < 0) test_statistic = -1*test_statistic;
  significance = sqrt(test_statistic);

  w->Print();

  cout << " significance in sigma " << significance << endl;

  cout << "errors on mu : High ="  << error_up << "   Down" << error_down << endl;

  cout << "LaTeX formatted error:" << endl;
  cout << Form("$ ^{+%3.3f}_{%3.3f} $",error_up,error_down) << endl;

  float symm_err(0.5*(fabs(error_up)+fabs(error_down)));
  
  return std::make_pair(significance, symm_err);
 
}
