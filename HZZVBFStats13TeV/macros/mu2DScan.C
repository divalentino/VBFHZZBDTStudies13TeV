#include "p0_calculation.C"

void mu2DScan()
{

  float best_cut1(-1), best_cut2(-1), min_delmu(100.);

  for (float cut1(0.); cut1 < 1.0; cut1 += 0.1) {
    for (float cut2(-0.8); cut2 < cut1; cut2 += 0.1) {

      gSystem->Exec(TString("cd ../; ./scripts/run_hzz_stats.sh 1 ")+Form( "%f ",cut1)+Form(" %f ",cut2)+" -1");
  
      float value = (p0_calculation("output_combined_datastat_model.root", "combined", "ModelConfig", "asimovData", false)).second;
      if (value < min_delmu) {
      	best_cut1 = cut1;
      	best_cut2 = cut2;
      	min_delmu = value;
      }
    }
  }

  std::cout << "//////////////////////////////////////////" << std::endl;
  std::cout << "Optimal bin edges: -1 " << Form(" %f ",best_cut2) << Form(" %f ",best_cut1) << " 1" << std::endl;
  std::cout << "Best \Delta\mu  is: "   << min_delmu        << std::endl;
  std::cout << "//////////////////////////////////////////" << std::endl;

}
