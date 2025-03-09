TESTS="openssl flac pgbench x265 fftw"

for test in $TESTS
do
    echo "Installing test: $test "
    phoronix-test-suite batch-install $test
    echo "Running test: $test "
    valgrind --tool=lackey --trace-mem=yes phoronix-test-suite batch-run $test 3>&- 1>&2 2>&3 | grep -E '^\s[LSM]' > $test.tmp
    echo "Converting $test to trace..."
    python3 ../scripts/val2trace.py $test.tmp $test.trace
done
