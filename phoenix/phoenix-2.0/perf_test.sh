#!/bin/bash
PI="${PI:-5000}"
RUNS="${RUNS:-1}"
AD=100
CUR_PATH=`pwd`
SUB_DIR="${SUB_DIR:-""}"
DIR=$CUR_PATH/phoenix_stats/$SUB_DIR
THREADS="${THREADS:-"1"}"

UNROLL_COUNT="${UNROLL_COUNT:-"0"}"
ACCURACY_TEST="${ACCURACY_TEST:-"1"}"
CONCORD_PASS_TYPE="${CONCORD_PASS_TYPE:-"rdtsc"}"

dry_run() {
  # Dry run - so that any disk caching does not hamper the process
  case "$1" in
    histogram)
      command="MR_NUMTHREADS=1 timeout 5m ./tests/$program/$program ../input_datasets/${program}_datafiles/large.bmp > /dev/null 2>&1"
    ;;
    kmeans)
      command="MR_NUMTHREADS=1 timeout 5m ./tests/$program/$program -d 100 -c 10 -p 500000 -s 50 > /dev/null 2>&1"
    ;;
    pca) 
      command="MR_NUMTHREADS=1 timeout 5m ./tests/$program/$program -r 1000 -c 1000 -s 1000 > /dev/null 2>&1"
    ;;
    string_match)
      command="MR_NUMTHREADS=1 timeout 5m ./tests/$program/$program ../input_datasets/${program}_datafiles/key_file_100MB.txt > /dev/null 2>&1"
    ;;
    linear_regression)
      command="MR_NUMTHREADS=1 timeout 5m ./tests/$program/$program ../input_datasets/${program}_datafiles/key_file_500MB.txt > /dev/null 2>&1"
    ;;
    linear_regression-seq)
      command="MR_NUMTHREADS=1 timeout 5m ./tests/$program/$program ../input_datasets/${program}_datafiles/key_file_500MB.txt > /dev/null 2>&1"
    ;;
    word_count)
      command="MR_NUMTHREADS=1 timeout 5m ./tests/$program/$program ../input_datasets/${program}_datafiles/word_50MB.txt > /dev/null 2>&1"
    ;;
    matrix_multiply)
      command="MR_NUMTHREADS=1 timeout 5m ./tests/$program/$program 1000 100 0 > /dev/null 2>&1"
    ;;
    reverse_index)
      command="MR_NUMTHREADS=1 $ts timeout 5m ./tests/$program/$program ../input_datasets/${program}_datafiles/www.stanford.edu/dept/news/ > /dev/null 2>&1"
    ;;
  esac
  echo "Dry run: "$command >> $DEBUG_FILE
  eval $command
}

#1 - benchmark name, 2 - #thread
# Do not print anything in this function as a value is returned from this
get_time() {
  rm -f out
  threads=$2
  program=$1

  ts="taskset -c 4,6,8"

  DIVISOR=`expr $RUNS \* 1000`
  rm -f sum
  rm -f median
  dry_run $program

  echo -n "scale=2;(" > sum
  for j in `seq 1 $RUNS`
  do
    case "$program" in
      histogram)
        command="MR_NUMTHREADS=$threads timeout 5m ./tests/$program/$program ../input_datasets/${program}_datafiles/large.bmp > out 2>&1"
      ;;
      kmeans)
        command="MR_NUMTHREADS=$threads timeout 5m ./tests/$program/$program -d 100 -c 10 -p 500000 -s 50 > out 2>&1"
      ;;
      pca) 
        command="MR_NUMTHREADS=$threads timeout 5m ./tests/$program/$program -r 1000 -c 1000 -s 1000 > out 2>&1"
      ;;
      string_match)
        command="MR_NUMTHREADS=$threads timeout 5m ./tests/$program/$program ../input_datasets/${program}_datafiles/key_file_100MB.txt > out 2>&1"
      ;;
      linear_regression)
        command="MR_NUMTHREADS=$threads timeout 5m ./tests/$program/$program ../input_datasets/${program}_datafiles/key_file_500MB.txt > out 2>&1"
      ;;
      word_count)
        command="MR_NUMTHREADS=$threads timeout 5m ./tests/$program/$program ../input_datasets/${program}_datafiles/word_50MB.txt > out 2>&1"
      ;;
      matrix_multiply)
        command="MR_NUMTHREADS=$threads timeout 5m ./tests/$program/$program 1000 100 0 > out 2>&1"
      ;;
      reverse_index)
        command="MR_NUMTHREADS=$threads $ts timeout 5m ./tests/$program/$program ../input_datasets/${program}_datafiles/www.stanford.edu/dept/news/ > out 2>&1"
      ;;
    esac
    echo $command >> $DEBUG_FILE
    eval $command
    # time_in_us=`cat out | grep "$program runtime: " | cut -d ':' -f 2 | cut -d ' ' -f 2 | tr -d '[:space:]'`
    time_in_us=`cat out | grep "library: " | tail -n 1 | cut -d ':' -f 2 | cut -d ' ' -f 2 | tr -d '[:space:]'`
    echo $time_in_us | tr -d '\n' >> sum
    echo $time_in_us >> median
    in_ms=`echo "scale=2;($time_in_us/1000)" | bc`
    echo $in_ms >> $BENCH_LOG
    echo "$time_in_us ms" >> $DEBUG_FILE
    if [ $j -lt $RUNS ]; then
      echo -n "+" >> sum
    fi
  done
  echo ")/$DIVISOR" >> sum
  time_in_ms=`cat sum | bc`
  echo "Average: $time_in_ms ms" >> $DEBUG_FILE
  time_in_ms=$(echo "scale=3; $(sort -n median | awk '{a[i++]=$1} END {print a[int(i/2)];}') / 1000" | bc -l)
  echo "Median: $time_in_ms ms" >> $DEBUG_FILE
  echo $time_in_ms
}

perf_test() {
  echo "=================================== PERFORMANCE TEST ==========================================="
  LOG_FILE="$DIR/perf_logs-ad$AD.txt"
  DEBUG_FILE="$DIR/perf_debug-$*-ad$AD.txt"
  BUILD_ERROR_FILE="$DIR/perf_test_build_error-ad$AD.txt"
  BUILD_DEBUG_FILE="$DIR/perf_test_build_log-ad$AD.txt"
  DEBUG_ORIG_FILE="$DIR/perf_debug-orig-ad$AD.txt"
  BUILD_ERROR_ORIG_FILE="$DIR/perf_test_build_error-orig-ad$AD.txt"
  BUILD_DEBUG_ORIG_FILE="$DIR/perf_test_build_log-orig-ad$AD.txt"
  #FIBER_CONFIG is set in the Makefile. Unless needed, do not pass a new config from this script
#LEGACY_INTV="1 10 100 1000 10000"
  LEGACY_INTV="100 1000"

  rm -f $LOG_FILE $DEBUG_FILE $BUILD_ERROR_FILE $BUILD_DEBUG_FILE
  rm -f $DEBUG_ORIG_FILE $BUILD_ERROR_ORIG_FILE $BUILD_DEBUG_ORIG_FILE

  for thread in $THREADS
  do
    PER_THREAD_STAT_FILE="$DIR/phoenix-perf_stats-th$thread-ad$AD.txt"
    echo -ne "Type" > $PER_THREAD_STAT_FILE
    for bench in $*
    do
      echo -ne "\t$bench" >> $PER_THREAD_STAT_FILE
    done
    echo "" >> $PER_THREAD_STAT_FILE
  done

  for bench in $*
  do
    ORIG_STAT_FILE="$DIR/$bench-perf_orig-ad$AD.txt"
    NAIVE_STAT_FILE="$DIR/$bench-perf_naive-ad$AD.txt"
    echo "pthread" > $ORIG_STAT_FILE
    echo "naive" > $NAIVE_STAT_FILE

    for thread in $THREADS
    do
      BENCH_LOG="$DIR/$bench-th$thread-orig-runs-log.txt"
      echo "PThread - Thread $thread, $RUNS runs" > $BENCH_LOG
      BENCH_LOG="$DIR/$bench-th$thread-naive-runs-log.txt"
      echo "Naive TL - Thread $thread, $RUNS runs" > $BENCH_LOG
    done
  done

  # arrays to hold results of original and naive runs
  declare -a orig_times
  declare -a naive_times

  #run original 
  echo "Building original program: " | tee -a $DEBUG_FILE
  make -f Makefile.orig clean >$BUILD_DEBUG_ORIG_FILE 2>$BUILD_ERROR_ORIG_FILE
  ORIG=1 make -f Makefile.orig >$BUILD_DEBUG_ORIG_FILE 2>$BUILD_ERROR_ORIG_FILE
  echo "Running original program: " | tee -a $DEBUG_FILE
  for thread in $THREADS
  do
    PER_THREAD_STAT_FILE="$DIR/phoenix-perf_stats-th$thread-ad$AD.txt"
    echo -ne "Orig" >> $PER_THREAD_STAT_FILE
  done

  # define array index
  arr_index_orig=0

  for bench in $*
  do
    ORIG_STAT_FILE="$DIR/$bench-perf_orig-ad$AD.txt"
    for thread in $THREADS
    do
      BENCH_LOG="$DIR/$bench-th$thread-orig-runs-log.txt"
      PER_THREAD_STAT_FILE="$DIR/phoenix-perf_stats-th$thread-ad$AD.txt"
      orig_time=$(get_time $bench $thread 0)
      echo -ne "\t$orig_time" >> $PER_THREAD_STAT_FILE
      echo -e "$thread\t$orig_time" >> $ORIG_STAT_FILE
      echo "Bench: $bench - Thread $thread - ${orig_time}ms" | tee -a $DEBUG_FILE

      #store the original run time
      orig_times[$arr_index_orig]=$orig_time
      arr_index_orig=$((arr_index_orig+1))
    done
  done

  #run naive
  echo "Building naive program: " | tee -a $DEBUG_FILE
  make -f Makefile.lc clean >$BUILD_DEBUG_FILE 2>$BUILD_ERROR_FILE
  UINTR=1 ACCURACY_TEST=$ACCURACY_TEST CONCORD_PASS_TYPE=$CONCORD_PASS_TYPE UNROLL_COUNT=$UNROLL_COUNT make -f Makefile.lc >$BUILD_DEBUG_FILE 2>$BUILD_ERROR_FILE
  echo "Running naive program: " | tee -a $DEBUG_FILE
  for thread in $THREADS
  do
    PER_THREAD_STAT_FILE="$DIR/phoenix-perf_stats-th$thread-ad$AD.txt"
    echo "" >> $PER_THREAD_STAT_FILE
    echo -ne "Naive" >> $PER_THREAD_STAT_FILE
  done

  arr_index_naive=0

  for bench in $*
  do
    NAIVE_STAT_FILE="$DIR/$bench-perf_naive-ad$AD.txt"
    for thread in $THREADS
    do
      BENCH_LOG="$DIR/$bench-th$thread-naive-runs-log.txt"
      PER_THREAD_STAT_FILE="$DIR/phoenix-perf_stats-th$thread-ad$AD.txt"
      naive_time=$(get_time $bench $thread 1)
      echo -ne "\t$naive_time" >> $PER_THREAD_STAT_FILE
      echo -e "$thread\t$naive_time" >> $NAIVE_STAT_FILE
      echo "Bench: $bench - Thread $thread - ${naive_time}ms" | tee -a $DEBUG_FILE

      #store the naive run time
      naive_times[$arr_index_naive]=$naive_time
      arr_index_naive=$((arr_index_naive+1))

      ACCURACY_FILE="$DIR/accuracy-$bench.txt"
      cp ${CONCORD_TIMESTAMP_PATH} $ACCURACY_FILE
      rm ${CONCORD_TIMESTAMP_PATH}
    done
  done

  for thread in $THREADS
  do
    PER_THREAD_STAT_FILE="$DIR/phoenix-perf_stats-th$thread-ad$AD.txt"
    echo "" >> $PER_THREAD_STAT_FILE
    final_thread=$thread
  done


  echo "========================== Finished =========================="
  
  # Print the runtime performance overhead

  bench_index=0
  for bench in $*
  do
    echo "Statistics for $bench"
    echo "Original Time: ${orig_times[$bench_index]} ms"
    echo "Naive Time: ${naive_times[$bench_index]} ms"
    echo "Overhead: `echo "scale = 3; (${naive_times[$bench_index]} / ${orig_times[$bench_index]})" | bc`"
    bench_index=$((bench_index+1))
  done
}

run_perf_test() {
  if [ $# -eq 0 ]; then
    perf_test histogram kmeans pca string_match linear_regression word_count
  else
    perf_test $@
  fi
}

echo "Note: Script has performance tests for both instantaneous & predictive clocks."
echo "Configured values:-"
echo "WARNING: Remove Passed Config if you don't need it!"
mkdir -p $DIR;

if [ $# -eq 0 ]; then
  run_perf_test
else
  run_perf_test $@
fi

# rm -f out sum
