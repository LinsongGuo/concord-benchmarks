source ../../env.sh

echo "Concord DIR = " $CONCORD_DIR

mv overhead_results.txt overhead_results.txt.bak
touch overhead_results.txt

python3 run.py