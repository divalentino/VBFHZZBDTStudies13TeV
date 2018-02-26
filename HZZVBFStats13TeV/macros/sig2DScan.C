#include "p0_calculation.C"

void sig2DScan()
{

  float best_cut1(-1), best_cut2(-1), max_Z0(0.);

  for (float cut1(0.2); cut1 < 1.0; cut1 += 0.1) {
    for (float cut2(0.1); cut2 < cut1; cut2 += 0.1) {

      //gSystem->Exec(TString("cd ../; ./scripts/run_hzz_stats.sh 1 ")+Form( "%f ",cut1)+Form(" %f ",cut2)+" 0");
      gSystem->Exec(TString("cd ../; ./scripts/run_hzz_stats.sh 1 ")+Form( "%f ",cut1)+Form(" %f ",cut2)+" 0");
  
      float value = (p0_calculation("output_combined_datastat_model.root", "combined", "ModelConfig", "asimovData", false)).first;
      if (value > max_Z0) {
      	best_cut1 = cut1;
      	best_cut2 = cut2;
      	max_Z0   = value;
      }
    }
  }

  std::cout << "//////////////////////////////////////////" << std::endl;
  std::cout << "Optimal bin edges: 0 " << Form(" %f ",best_cut2) << Form(" %f ",best_cut1) << " 1" << std::endl;
  std::cout << "Best Z0  is: " << max_Z0   << std::endl;
  std::cout << "//////////////////////////////////////////" << std::endl;

}
