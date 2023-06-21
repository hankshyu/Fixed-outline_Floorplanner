make clean
make
./iccad ./TestCase/ProblemD_case02-input.txt ./TestCase/ProblemD_case02-output.txt
echo "running python render"
python3 draw_block_layout.py legalFloorplan.txt legalFloorplan02.png
