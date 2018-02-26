#include "Utils/AtlasStyle/AtlasStyle.h"
#include "Utils/AtlasStyle/AtlasStyle.C"

void DrawLuminosityPlots()
{

  SetAtlasStyle();
  
  float lumi, z0, dmu;
  //std::ifstream infile("lumi_z0_dmu.txt");

  std::ifstream infile("lumi_z0_dmu_float_ggf.txt");
  std::vector<float> lumis, z0s, dmus;

  while (infile >> lumi >> z0 >> dmu) {
    lumis.push_back(lumi);
    z0s  .push_back  (z0);
    dmus .push_back  (dmu);
  }

  TCanvas *c1 = new TCanvas("c1","c1",800,600);
  
  //Draw graphs vs. integrated luminosity of dmu
  //and z0.
  TGraph *z0_graph = new TGraph(lumis.size(), &(lumis[0]), &(z0s[0]));
  z0_graph->GetXaxis()->SetTitle("Int. luminosity [fb^{-1}]");
  z0_graph->GetYaxis()->SetTitle("Expected Z_{0}^{VBF}");
  z0_graph->GetYaxis()->SetRangeUser(0,8);

  z0_graph->Draw("ALP");

  TLine line;
  line.SetLineStyle(9);

  //3sigma point.
  line.DrawLine(0,3,550,3);
  line.DrawLine(150,0,150,8);
  
  //5sigma point.
  line.DrawLine(0,5,550,5);
  line.DrawLine(415,0,415,8);
  
  c1->Print("lumi_scan_z0.pdf");
  
  TGraph *dmu_graph = new TGraph(lumis.size(), &(lumis[0]), &(dmus[0]));
  dmu_graph->GetXaxis()->SetTitle("Int. luminosity [fb^{-1}]");
  dmu_graph->GetYaxis()->SetTitle("Expected #Delta#mu_{VBF}");

  dmu_graph->Draw("ALP");
  
  c1->Print("lumi_scan_dmu.pdf");

}
