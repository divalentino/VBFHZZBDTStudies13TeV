#!/bin/bash

nCats="$1"
out_file=driver${nCats}.xml

echo "" &> $out_file

echo "<!DOCTYPE Combination  SYSTEM 'HistFactorySchema.dtd'>" >> $out_file

echo '<Combination OutputFilePrefix="./output" >' >> $out_file

for (( i=1; $i <= $nCats; i++ )); do
  echo "<Input>BDT_category${i}_vbf_ZZ.xml</Input>" >> $out_file
done

echo '<Measurement Name="datastat" Lumi="1" LumiRelErr="0.0001" ExportOnly="True">' >> $out_file
echo '    <POI>mu_vbf</POI>' >> $out_file
echo '  </Measurement>' >> $out_file

echo '</Combination>' >> $out_file
