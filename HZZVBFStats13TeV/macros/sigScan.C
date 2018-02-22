#include "p0_calculation.C"

void sigScan(bool do2DScan = true)
{

  if (do2DScan) {

    float best_tight_cut(-1), best_medium_cut(-1), max_Z0(0.), corr_dmu(-1.);

    for (float tight_cut(0.6); tight_cut < 0.9; tight_cut += 0.05) {
      for (float medium_cut(0.3); medium_cut < tight_cut; medium_cut += 0.05) {

	gSystem->Exec(TString("cd ../; ./scripts/run_hzz_stats.sh 1 ")+Form("%f %f 0", tight_cut, medium_cut));

	std::pair<float,float> Z0AndDMu(p0_calculation("output_combined_datastat_model.root", "combined", "ModelConfig", "asimovData", false));
	float value = Z0AndDMu.first;
	if (value > max_Z0) {
	  best_tight_cut  = tight_cut;
	  best_medium_cut = medium_cut;
	  max_Z0          = value;
	  corr_dmu        = Z0AndDMu.second;
	}
      }
    }

    std::cout << "//////////////////////////////////////////" << std::endl;
    std::cout << "Best tight cut is:  " << best_tight_cut     << std::endl;
    std::cout << "Best medium cut is: " << best_medium_cut    << std::endl;
    std::cout << "Best Z0  is:        " << max_Z0             << std::endl;
    std::cout << "Corr. dMu is:       " << corr_dmu           << std::endl;
    std::cout << "//////////////////////////////////////////" << std::endl;

  } else {
    
    //Set this to -1 to perform a 1D search to find best single category.
    float best_tight_cut(0.6);
    float starting_cut  (0.0);
    float del_cut       (0.1);
  
    float best_medium_cut(-1), max_Z0(0.), corr_dmu(-1.);
    float best_cut(-1);

    for (float cut(starting_cut); cut < ((best_tight_cut>0) ? best_tight_cut : 1.0); cut += del_cut) {

      if (best_tight_cut>0)
	gSystem->Exec(TString("cd ../; ./scripts/run_hzz_stats.sh 1 ")+Form("%f %f 0", best_tight_cut, cut));
      else
	gSystem->Exec(TString("cd ../; ./scripts/run_hzz_stats.sh 1 ")+Form("%f", cut));

      std::pair<float,float> Z0AndDMu(p0_calculation("output_combined_datastat_model.root", "combined", "ModelConfig", "asimovData", false));
      float value = Z0AndDMu.first;
      if (value > max_Z0) {
	best_cut        = cut;
	max_Z0          = value;
	corr_dmu        = Z0AndDMu.second;
      }
    
    }

    std::cout << "//////////////////////////////////////////" << std::endl;
    std::cout << "Best cut is:         " << best_cut          << std::endl;
    std::cout << "Best Z0  is:        " << max_Z0             << std::endl;
    std::cout << "Corr. dMu is:       " << corr_dmu           << std::endl;
    std::cout << "//////////////////////////////////////////" << std::endl;

  }

}
