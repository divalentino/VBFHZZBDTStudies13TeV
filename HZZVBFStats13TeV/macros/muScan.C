#include "p0_calculation.C"

//Scan to minimize symmetrized signal strength uncertainty \Delta\mu.
void muScan()
{

  float best_cut(-1), min_delmu(100.);

  for (float cut(0.); cut < 1.0; cut += 0.1) {
  
    gSystem->Exec(TString("cd ../; ./scripts/run_hzz_stats.sh 1 ")+Form("%f",cut));
  
    float value = (p0_calculation("output_combined_datastat_model.root", "combined", "ModelConfig", "asimovData", false)).second;
    if (value < min_delmu) {
      best_cut  = cut;
      min_delmu = value;
    }
  }

  std::cout << "//////////////////////////////////////////" << std::endl;
  std::cout << "Best cut is:        " << best_cut    << std::endl;
  std::cout << "Best \Delta\mu  is: " << min_delmu   << std::endl;
  std::cout << "//////////////////////////////////////////" << std::endl;

}
