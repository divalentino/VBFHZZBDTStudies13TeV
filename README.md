# VBFHZZBDTStudies13TeV

Code portfolio demonstrating scripts and code written for the ATLAS multivariate VBF H -> ZZ* -> 4l analysis. See also the documentation in 'vbf_hzz_bdt_studies.pdf' for more details.

## Scripts of note:

The following is a list of the most important code/scripts in the
repository, and the functions they serve:

| File | Function |
|--------|----------|
| ```run/run_classification_var_scan.sh``` | Train all BDT permutations using cluster computing grid |
|```run/run_classification_application_var_scan.sh``` | Apply all BDT permutations to testing samples using cluster computing grid |
| ```run/sig_scan.sh``` |Find the best significance among 'N' variables from resultant trainings |
