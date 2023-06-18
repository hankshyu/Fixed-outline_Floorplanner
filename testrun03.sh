make clean
make
./iccad ./TestCase/ProblemD_case03-input.txt ./TestCase/ProblemD_case03-output.txt
echo "running python render"
python3 draw_block_layout.py initFloorplan.txt initFloorplan03.png
python3 draw_block_layout.py finalFloorplan.txt finalFloorplan03.png
