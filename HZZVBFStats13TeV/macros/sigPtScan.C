#include "p0_calculation.C"

void sigPtScan()
{

  float best_cut(-1), max_Z0(0.);

  for (float cut(0.); cut <= 1.0; cut += 0.1) {
  
    gSystem->Exec(TString("cd ../; ./scripts/run_hzz_stats_ptcut.sh 1 ")+Form("%f",cut));
  
    float value = (p0_calculation("output_combined_datastat_model.root", "combined", "ModelConfig", "asimovData",true)).first;
    if (value > max_Z0) {
      best_cut = cut;
      max_Z0   = value;
    }
  }

  std::cout << "//////////////////////////////////////////" << std::endl;
  std::cout << "Best cut is: " << best_cut << std::endl;
  std::cout << "Best Z0  is: " << max_Z0   << std::endl;
  std::cout << "//////////////////////////////////////////" << std::endl;

}
