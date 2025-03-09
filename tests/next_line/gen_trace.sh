valgrind --tool=lackey --trace-mem=yes ./a.out 2>&1 | grep -E '^\s[LSM]' > trace_tmp.txt
