#!/bin/bash

# Run the first makefile and save the output to a file
cd ./task01
make -f ./Makefile
echo "Running L1..."
./L1Cache > ../outputL1.txt


# Run the second makefile and save the output to a file
cd ../task02
make -f ./Makefile
echo "Running L2..."
./L2Cache > ../outputL2.txt

# Run the third makefile and save the output to a file
cd ../task03
make -f ./Makefile
echo "Running L2W2..."
./L2W2Cache > ../outputL2W2.txt

# Compare the outputs using diff
cd ..
echo "Comparing the outputs..."
diff ./outputL1.txt ./tests_alunos/results_L1.txt > diffL1.txt
diff ./outputL2.txt ./tests_alunos/results_L2_1W.txt > diff1L2.txt
diff ./outputL2W2.txt ./tests_alunos/results_L2_2W.txt > diff2L2W2.txt

# Display the results
echo "Comparison between L1 test:"
cat diffL1.txt
echo "Comparison between L2 test:"
cat diff1L2.txt
echo "Comparison between L3 test:"
cat diff2L2W2.txt

# Clean up
rm outputL1.txt outputL2.txt outputL2W2.txt diffL1.txt diff1L2.txt diff2L2W2.txt ./task01/L1Cache ./task02/L2Cache ./task03/L2W2Cache
