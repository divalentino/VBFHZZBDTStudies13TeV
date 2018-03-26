#!/bin/bash

exec=create_TMVAClassification.exe
code=TMVAClassification.cxx
rm -f $exec

queue="8nm"
codefolder=""
if [[ $PWD == *afs* ]]; then
  codefolder="/afs/cern.ch/user/d/${USER}/Research/VBFZPlusJetsStudies/H4lBDTAnalysis/"
else
  codefolder="/home/${USER}/Research/VBFZPlusJetsStudies/H4lBDTAnalysis/"
fi

CMD="$(root-config --cxx --cflags) -lMinuit -lTMVA -o $exec $code $(root-config --libs)"

#I/O information.
start_int=2
end_int=13
num_vars=13
BDTBaseFile="./vars/BDT_vars_${num_vars}"

if [[ $end_int -gt $num_vars ]]; then
  echo "End integer needs to be <= # of variables! Aborting ..."
  exit
fi

#Setting up I/O folders.
exec_dir=$PWD
#mkdir /tmp/${USER}

#Compile executable.
result=$($CMD)

[[ -e $exec ]] && {

  numvar="$1"
  numperm="$2"

  #Running a single permutation.
  if [[ "${numvar}" != "" ]] && [[ "${numperm}" != "" ]]; then
      #cd /tmp/${USER}/
      echo "" &> run.sh
      echo "#!/bin/bash" >> run.sh
      echo "cp -r -v ${codefolder}/* . " >> run.sh
      echo "cd TMVAscripts" >> run.sh
      echo "chmod +x run/*.sh . " >> run.sh
      echo "./run/run_classification_application_var_single.sh $numvar $numperm" >> run.sh
      chmod +x run.sh
      if [[ $exec_dir == *afs* ]]; then
	  bsub -q ${queue} < run.sh
	  cd $exec_dir
          return
      else
	  nohup ./run.sh &
	  cd $exec_dir
          exit
      fi
  else 
      for ((i=${start_int}; i<=${end_int}; i++)); do
	  numfiles=`ls -1 ${BDTBaseFile}_${i}_* | wc -l`
	  for ((j=1; j<=${numfiles}; j++)); do
              #cd /tmp/${USER}/
	      echo "" &> run.sh
	      echo "#!/bin/bash" >> run.sh
	      echo "cp -r -v ${codefolder}/* . " >> run.sh
	      echo "cd TMVAscripts" >> run.sh
	      echo "chmod +x run/*.sh . " >> run.sh
	      echo "./run/run_classification_application_var_single.sh $i $j" >> run.sh
	      chmod +x run.sh
	      if [[ $exec_dir == *afs* ]]; then
		  bsub -q ${queue} < run.sh
		  cd $exec_dir
	      else
		  nohup ./run.sh &
		  cd $exec_dir
	      fi
	  done
      done
  fi

}
