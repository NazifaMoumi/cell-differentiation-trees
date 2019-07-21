#!/bin/bash

for i in {1..7315}
do
	cat RAxML_bestTree.July5$i >> input.txt
done
exit 0
