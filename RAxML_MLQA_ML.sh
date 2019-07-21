#!/bin/bash
chmod 777 raxmlHPC
raxmlHPC -m BINGAMMA

#for MLQA approach
raxmlHPC -m BINGAMMA -s overlap_datarepresentation_2.phy -f q -p 12345 -n h3k36_2

#for ML based approach
 raxmlHPC -m BINGAMMA -s overlap_datarepresentation_2.phy -p 1234 -n July1 -N 5

exit 0

