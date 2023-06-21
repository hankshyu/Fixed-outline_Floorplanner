make clean
make
./iccad ./TestCase/ProblemD_case03-input.txt ./TestCase/ProblemD_case03-output.txt
echo "running python render"
python3 draw_block_layout.py legalFloorplan.txt legalFloorplan03.png
