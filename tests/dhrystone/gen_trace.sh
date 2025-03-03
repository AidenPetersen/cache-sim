valgrind --tool=lackey --trace-mem=yes ./dry.c 2>&1 | grep -E '^\s[LSM]' > trace_tmp.txt
