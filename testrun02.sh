make clean
make
./iccad ./TestCase/ProblemD_case02-input.txt ./TestCase/ProblemD_case02-output.txt
echo "running python render"
python3 draw_block_layout.py initFloorplan.txt initFloorplan02.png
python3 draw_block_layout.py finalFloorplan.txt finalFloorplan02.png
