make clean
make
./iccad ./TestCase/ProblemD_case01-input.txt ./TestCase/ProblemD_case01-output.txt
echo "running python render"
python3 draw_block_layout.py initFloorplan.txt initFloorplan01.png