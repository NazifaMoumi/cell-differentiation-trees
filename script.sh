#!/bin/bash

chmod 777 raxmlHPC
raxmlHPC -m BINGAMMA
for number in {1..7315}
do
filename='Quert'
extension='.phy'
finalname=$filename$number$extension
date='July5'
outputname=$date$number
raxmlHPC -m BINGAMMA -s $finalname -p 1234 -n $outputname -N 5
echo $finalname
echo $outputname
rm RAxML_result.*
rm RAxML_log.*
rm RAxML_info.*
rm *.RUN.*
done
exit 0

