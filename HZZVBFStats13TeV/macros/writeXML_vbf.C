#include "TH1D.h"
#include "TFile.h"
#include "TLegend.h"
#include "THStack.h"
#include "TCanvas.h"
#include "TTree.h"
#include "TStyle.h"
#include "THStack.h"
#include <vector>
#include <string>
#include <iostream>
#include <algorithm>
#include <sstream>
#include <map>
#include "TSystem.h"
#include <cstdlib>
#include "TString.h"
#include "TLatex.h"
#include "TROOT.h"
#include <fstream>
#include <iostream>

void writeXML_vbf( TString   filepathnomass="",
		   const int systN = 1,
		   const int sampleN = 2,
		   int       regionN = 1,
		   float     mass = 125,
		   const int massesN = 1,
		   bool      add_syst = true,
		   bool      add_stat_err = false,
		   bool      Norm_factor_ggf = false,
		   bool      higgs_theo_syst = true,
		   bool      remove_general_theo_syst = false,
		   bool      use_st_uncert=false,
		   bool      use_jes=false )

{
  
  bool donot_write_syst = false;

  float var_up = 1.1;
  float var_down = 0.9;

  //Syst
  std::vector<TString> systematics;
  systematics.push_back("JET_GroupedNP_1__1");
  systematics.push_back("JET_GroupedNP_2__1");
  systematics.push_back("JET_GroupedNP_3__1");
  systematics.push_back("JET_JER_SINGLE_NP__1");
  
  //if (systN==1)
    //systematics.push_back("jes");
  //	systematics[1]="ATLAS_JES";

  //Mass
  /*
    float massPoint[massesN];
    for(int i =0 ; i<massesN ; i++){
    massPoint[i]=120+i*5;
    cout << "massPoint = " << massPoint[i]  << endl;
    }
  */
  //Regions
  std::vector<TString> region;
  for (int i(1); i <= regionN; i++)
    region.push_back(TString("BDT_category")+Form("%i",i));
  
  // if (regionN==1)
  //   region[0]="BDT_category1";
  // region[1]="BDT_category2";
  // region[2]="BDT_category3";

  //  region[2]="CR1";

  //Samples
  std::vector<TString> sample;
  sample.push_back("vbf");
  sample.push_back("ggf");
  //sample.push_back("qqzz");
  
  // sample[2]="other";


  for(int y=0; y<massesN; y++){
    /*
      std::ostringstream os;
      os << massPoint[y];
    */
    TString directoryrm = ".! rm -rf xml/*category*.xml";
    gROOT->ProcessLine(directoryrm);
    TString directory = ".! mkdir -p xml/";
    gROOT->ProcessLine(directory);
    // TString copydriver = ".! cp ./driver.xml xml/driver.xml";
    // gROOT->ProcessLine(copydriver);
    //TString copydriver_2 = ".! cp ./ChangeWorkspace.C xml/ChangeWorkspace.C";
    //gROOT->ProcessLine(copydriver_2);
    TString copyHist = ".! cp ./HistFactorySchema.dtd xml/HistFactorySchema.dtd";
    gROOT->ProcessLine(copyHist);
    TString copyrunp0_script = ".! cp ./macros/p0_calculation.C xml/p0_calculation.C";
    gROOT->ProcessLine(copyrunp0_script);
    // TString copyrootc = ".! cp root.C xml/root.C";
    // gROOT->ProcessLine(copyrootc);

  }


  for(int j = 0; j<regionN ; j++){

    ofstream * optfile = new ofstream;

    TString xmlName = "xml/"+ region[j] +"_vbf_ZZ.xml";

    cout << "file name = " << xmlName << endl;

    optfile->open(xmlName);


    TString fileName = filepathnomass + "output_"+ region[j] +"_data.root";

    TFile *f_input_data = TFile::Open(fileName);
    if(!f_input_data) continue;

    *optfile<<"<!DOCTYPE Channel SYSTEM 'HistFactorySchema.dtd'>" << endl;
    *optfile<<endl;
    *optfile<<"<Channel Name=\""<<region[j]<<"" <<"\" HistoName=\"\">"<<endl;
    *optfile<<"  <Data " <<" InputFile= \"" << fileName << "\" HistoName=\"hist_data\" HistoPath=\"\" />" << endl;
    *optfile<<" <StatErrorConfig RelErrorThreshold= \"0 \" ConstraintType=\"Poisson\" />"<<endl;
    *optfile<<endl;

    for(int k = 0; k<sampleN ; k++){
      TString histoName = /* "hist_"+*/sample[k]+"_BDTG";
      cout << "-------------------------------------sample = " << sample[k] << endl;

      TString fileName_sample = filepathnomass + "output_"+ region[j] +"_"+sample[k]+"_nominal.root";

      TFile *f_input = TFile::Open(fileName_sample);
      if(!f_input) continue;


      TH1F *h_signal = (TH1F*)f_input->Get(histoName);
      float value = h_signal->GetBinContent(1);
      cout << "value = " << value << endl;

      *optfile<<"  <Sample Name=\""<<sample[k]<<"\" "<<" InputFile= \"" << fileName_sample <<"\""  <<" HistoPath=\"\" HistoName=\""<<histoName <<"\" NormalizeByTheory=\"True\">"<<endl;

      if(add_syst && sample[k] != "qqzz"){

        for(int m = 0 ; m<systN; m++){
	  TString fileName_UP = filepathnomass + "/output_"+ region[j]+ "_"+ sample[k] + "_" + systematics[m]+"up"+".root";
	  //output_BDT_category1_vbf_jesup.root

	  TFile *f_input_UP = TFile::Open(fileName_UP);
	  if(!f_input_UP) continue;
	  TH1F *h_signal_UP = (TH1F*)f_input_UP->Get(histoName);
	  float value_UP    = h_signal_UP->GetBinContent(1);
	  float value_DOWN  = value - fabs(value - h_signal_UP->GetBinContent(1));

	  if (value_UP < value)
	    value_DOWN  = value + fabs(value - h_signal_UP->GetBinContent(1));

	  TString fileName_DOWN = filepathnomass + "/output_"+ region[j]+ "_"+ sample[k] + "_" + systematics[m]+"down"+".root";

	  TFile *f_input_DOWN = TFile::Open(fileName_DOWN);
	  if(f_input_DOWN) {
	    TH1F *h_signal_DOWN = (TH1F*)f_input_DOWN->Get(histoName);
	    value_DOWN = h_signal_DOWN->GetBinContent(1);
	    f_input_DOWN->Close();
	  }
	  f_input_UP->Close();

	  float var_up = value_UP / value;
	  float var_down = value_DOWN / value;

	  //*optfile<<"    <OverallSys Name=\"" << systematics[m] << "\" High= \""<< var_up <<"\" Low=\"" << var_down << "\"/>" << endl;
	  if(systematics[m].Contains("theo") && remove_general_theo_syst ) donot_write_syst = true;
	  if(!donot_write_syst)*optfile<<"    <OverallSys Name=\"" << systematics[m] << "\" High= \""<< var_up <<"\" Low=\"" << var_down << "\"/>" << endl;
	  
	}

      }//close syst
      if(sample[k]=="vbf"){

	// if(higgs_theo_syst)   *optfile <<"    <OverallSys Name=\"QCDscale_vbf\" High=\"1.004\" Low=\"0.995\" />" << endl;
	// if(higgs_theo_syst)   *optfile <<"    <OverallSys Name=\"pdf_Higgs_vbf\" High=\"1.033\" Low=\"0.967\" />" << endl;
	// if(higgs_theo_syst)   *optfile <<"    <OverallSys Name=\"ATLAS_BR_VV\" High=\"1.0428\" Low=\"0.9579\" />" << endl;
	
	if (use_jes) {

	  //25 GeV threshold.
	  // *optfile << "    <OverallSys Name=\"JET_GroupedNP_1__1\" High=\"1.0547\" Low=\"0.9453\" />" << endl;
	  // *optfile << "    <OverallSys Name=\"JET_GroupedNP_2__1\" High=\"1.0194\" Low=\"0.9806\" />" << endl;
	  // *optfile << "    <OverallSys Name=\"JET_GroupedNP_3__1\" High=\"1.0436\" Low=\"0.9564\" />" << endl;
	  // *optfile << "    <OverallSys Name=\"JET_JER_SINGLE_NP__1\" High=\"1.0002\" Low=\"0.9998\" />" << endl;

	  //35 GeV threshold.
	  *optfile << "    <OverallSys Name=\"JET_GroupedNP_1__1\" High=\"1.060062\" Low=\"0.939938\" />" << endl;
	  *optfile << "    <OverallSys Name=\"JET_GroupedNP_2__1\" High=\"1.022128\" Low=\"0.977872\" />" << endl;
	  *optfile << "    <OverallSys Name=\"JET_GroupedNP_3__1\" High=\"1.053123\" Low=\"0.946877\" />" << endl;
	  *optfile << "    <OverallSys Name=\"JET_JER_SINGLE_NP__1\" High=\"1.002248\" Low=\"0.997752\" />" << endl;
	  
	}
	
        *optfile <<"    <NormFactor Name=\"mu_vbf\" Val=\"1.\" Low=\"0.\" High=\"20.\" Const=\"True\" />" << endl;
	
      }
      
      if(sample[k]=="ggf"){

	//Gluon fusion theory systematics.
	if(higgs_theo_syst) {
	  if (use_st_uncert)   {

	    // float st_up(1.15), st_down(0.85);
	    // *optfile << Form("    <OverallSys Name=\"QCDscale_ggf\" High=\"%f \" Low=\"%f\" />",st_up,st_down) << endl;

	    //Need to get ST uncertainty from file.
	    float pt_cut(0.), st_read(1.0), st_uncert(-1);

	    std::cout << "****************************************************************" << std::endl;
	    TString in_filename = TString("xml/st_uncert_cat_") + Form("%i",j+1) + ".txt";
	    std::cout << "Getting ST uncertainty from: " << in_filename.Data() << std::endl;
	    std::ifstream infile(in_filename.Data());
	    while (infile >> st_read) {
	      st_uncert = st_read;
	    }
	    
	    if (st_uncert < 0) {
	      std::cout << "Didn't find ST uncertainty matching input value??? Exiting ..." << std::endl;
	      return;
	    }
	  
	    float st_up(1.+st_uncert), st_down(1.-st_uncert);

	    if (st_down < 0.)
	      st_down = 0.01;
	  
	    *optfile << Form("    <OverallSys Name=\"QCDscale_ggf\" High=\"%f \" Low=\"%f\" />",st_up,st_down) << endl;

	  } else {
	    *optfile <<"    <OverallSys Name=\"QCDscale_ggf\" High=\"1.13\" Low=\"0.87\" />" << endl;
	  }
	}
	
	// if(higgs_theo_syst)   *optfile <<"    <OverallSys Name=\"pdf_Higgs_ggf\" High=\"1.072\" Low=\"0.94\" />" << endl;
	// if(higgs_theo_syst)   *optfile <<"    <OverallSys Name=\"ATLAS_BR_VV\" High=\"1.0428\" Low=\"0.9579\" />" << endl;
	
        if(Norm_factor_ggf)
	  *optfile <<"    <NormFactor Name=\"mu_ggf\" Val=\"1.\" Low=\"0.\" High=\"20.\" Const=\"True\" />" << endl;
	
	//NEW: Luminosity uncertainty.
	*optfile <<"    <OverallSys Name=\"ATLAS_Lumi\" High=\"1.03\" Low=\"0.97\" />" << endl;
	
	if (use_jes) {

	  //25 GeV threshold.
	  // *optfile << "<OverallSys Name=\"JET_GroupedNP_1__1\" High=\"1.117428\" Low=\"0.882572\" />" << endl;
	  // *optfile << "<OverallSys Name=\"JET_GroupedNP_2__1\" High=\"1.041716\" Low=\"0.958284\" />" << endl;
	  // *optfile << "<OverallSys Name=\"JET_GroupedNP_3__1\" High=\"1.066367\" Low=\"0.933633\" />" << endl;
	  // *optfile << "<OverallSys Name=\"JET_JER_SINGLE_NP__1\" High=\"1.054224\" Low=\"0.945776\" />" << endl;

	  *optfile << "<OverallSys Name=\"JET_GroupedNP_1__1\" High=\"1.096702\" Low=\"0.903298\" />" << endl;
	  *optfile << "<OverallSys Name=\"JET_GroupedNP_2__1\" High=\"1.034112\" Low=\"0.965888\" />" << endl;
	  *optfile << "<OverallSys Name=\"JET_GroupedNP_3__1\" High=\"1.065312\" Low=\"0.934688\" />" << endl;
	  *optfile << "<OverallSys Name=\"JET_JER_SINGLE_NP__1\" High=\"1.046333\" Low=\"0.953667\" />" << endl;
	  
	}
	
      }

      if(add_stat_err) *optfile <<"    <StatError Activate=\"True\"/> " <<endl;
      if(!add_stat_err) *optfile <<"    <StatError Activate=\"False\"/> " <<endl;
      *optfile<<"  </Sample>"<<endl;
      *optfile<<endl;

    }//close sample
    *optfile<<"</Channel>"<< endl;
    optfile->close();	
    //f_input->Close();
  }//close region
  
  //f_input_data->Close();

  //  TString createws = ".! ";
  //  gROOT->ProcessLine(cdxml);
  //  system("cd xml");
  //  system("hist2workspace driver.xml");


}
