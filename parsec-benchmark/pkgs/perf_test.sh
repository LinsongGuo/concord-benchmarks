#!/bin/bash
CI=1000
PI="${PI:-5000}"
RUNS="${RUNS:-1}"
AD=100
CUR_PATH=`pwd`
SUB_DIR="${SUB_DIR:-""}"
DIR=$CUR_PATH/parsec_stats/$SUB_DIR
ACCURACY_DIR=$CUR_PATH/accuracy/$SUB_DIR
#CLOCK=1 #0 - predictive, 1 - instantaneous
THREADS="${THREADS:-"1"}"

NOT_CONCORD="${NOT_CONCORD:-"1"}"
UNROLL_COUNT="${UNROLL_COUNT:-"2"}"
ACCURACY_TEST="${ACCURACY_TEST:-"0"}"
CONCORD_PASS_TYPE="${CONCORD_PASS_TYPE:-"cache"}"

LOG_FILE="$DIR/perf_logs-$AD.txt"
DEBUG_FILE="$DIR/perf_debug-$AD.txt"
BUILD_ERROR_FILE="$DIR/perf_test_build_error-$AD.txt"
BUILD_DEBUG_FILE="$DIR/perf_test_build_log-$AD.txt"


dry_run() {
  case "$1" in
    blackscholes)
      cd blackscholes/src > /dev/null
      $ts $prefix ./blackscholes$suffix $threads ../inputs/in_10M.txt prices.txt > /dev/null 2>&1
      echo "$ts $prefix ./blackscholes$suffix $threads ../inputs/in_10M.txt prices.txt > /dev/null 2>&1" >> $DEBUG_FILE
    ;;
    fluidanimate)
      cd fluidanimate/src > /dev/null
      $ts $prefix ./fluidanimate$suffix $threads 5 ../inputs/in_300K.fluid out.fluid > /dev/null 2>&1
      echo "$ts $prefix ./fluidanimate$suffix $threads 5 ../inputs/in_300K.fluid out.fluid > /dev/null 2>&1" >> $DEBUG_FILE
    ;;
    swaptions)
      cd swaptions/src > /dev/null
      $ts $prefix ./swaptions$suffix -ns 128 -sm 200000 -nt $threads > /dev/null 2>&1
      echo "$ts $prefix ./swaptions$suffix -ns 128 -sm 200000 -nt $threads > /dev/null 2>&1" >> $DEBUG_FILE
    ;;
    canneal)
      cd canneal/src > /dev/null
      $ts $prefix ./canneal$suffix $threads 15000 2000 ../inputs/200000.nets 6000 > /dev/null 2>&1
      echo "$ts $prefix ./canneal$suffix $threads 15000 2000 ../inputs/200000.nets 6000 > /dev/null 2>&1" >> $DEBUG_FILE
    ;;
    dedup)
      cd dedup/src > /dev/null
      $ts $prefix ./dedup$suffix -c -p -v -t $threads -i ../inputs/media.dat -o output.dat.ddp -w none > /dev/null 2>&1
      echo "$ts $prefix ./dedup$suffix -c -p -v -t $threads -i ../inputs/media.dat -o output.dat.ddp -w none > /dev/null 2>&1" >> $DEBUG_FILE
    ;;
    streamcluster)
      cd streamcluster/src > /dev/null
      $ts $prefix ./streamcluster$suffix 10 20 128 16384 16384 1000 none output.txt $threads > /dev/null 2>&1 
      echo "$ts $prefix ./streamcluster$suffix 10 20 128 16384 16384 1000 none output.txt $threads > /dev/null 2>&1" >> $DEBUG_FILE
    ;;
  esac
  cd - > /dev/null
}

#1 - benchmark name, 2 - #thread
# Do not print anything in this function as a value is returned from this
get_time() {
  rm -f out
  threads=$2
  suffix_conf=$3
  declare suffix
  if [ $suffix_conf -eq 0 ]; then
    suffix="_llvm"
  else
    suffix="_ci"
  fi
  # if [ $4 -eq 1 ]; then
  #   prefix="taskset 0x00000001 "
  # else
  #   prefix=""
  # fi
  prefix=""
  ts="taskset -c 4,6,8,10,12,14"
  OUT_FILE="$DIR/out"
  SUM_FILE="$DIR/sum"
  MEDIAN_FILE="$DIR/median"
  rm -f $MEDIAN_FILE

  DIVISOR=`expr $RUNS \* 1000`
  rm -f $OUT_FILE $SUM_FILE
  dry_run $1

  echo -n "scale=2;(" > $SUM_FILE
  for j in `seq 1 $RUNS`
  do
    case "$1" in
      blackscholes)
        cd blackscholes/src > /dev/null
        $ts $prefix ./blackscholes$suffix $threads ../inputs/in_10M.txt prices.txt > $OUT_FILE
        sleep 1.5
        echo "$ts $prefix ./blackscholes$suffix $threads ../inputs/in_10M.txt prices.txt > $OUT_FILE" >> $DEBUG_FILE
        cd - > /dev/null
      ;;
      fluidanimate)
        cd fluidanimate/src > /dev/null
        $ts $prefix ./fluidanimate$suffix $threads 2 ../inputs/in_300K.fluid out.fluid > $OUT_FILE
        sleep 0.5
        echo "$ts $prefix ./fluidanimate$suffix $threads 2 ../inputs/in_300K.fluid out.fluid > $OUT_FILE" >> $DEBUG_FILE
        cd - > /dev/null
      ;;
      swaptions) 
        cd swaptions/src > /dev/null
        $ts $prefix ./swaptions$suffix -ns 128 -sm 200000 -nt $threads > $OUT_FILE
	sleep 1.5
	echo "$ts $prefix ./swaptions$suffix -ns 128 -sm 200000 -nt $threads > $OUT_FILE" >> $DEBUG_FILE
        cd - > /dev/null
      ;;
      canneal) 
        cd canneal/src > /dev/null
        $ts $prefix ./canneal$suffix $threads 15000 2000 ../inputs/200000.nets 6000 > $OUT_FILE
        sleep 1.5
	echo "$ts $prefix ./canneal$suffix $threads 15000 2000 ../inputs/200000.nets 6000 > $OUT_FILE" >> $DEBUG_FILE
        cd - > /dev/null
      ;;
      dedup)
        cd dedup/src > /dev/null
        $ts $prefix ./dedup$suffix -c -p -v -t $threads -i ../inputs/media.dat -o output.dat.ddp -w none > $OUT_FILE
        sleep 1.5
        echo "$ts $prefix ./dedup$suffix -c -p -v -t $threads -i ../inputs/media.dat -o output.dat.ddp -w none > $OUT_FILE" >> $DEBUG_FILE
        cd - > /dev/null
      ;;
      streamcluster)
        cd streamcluster/src > /dev/null
        $ts $prefix ./streamcluster$suffix 10 20 128 16384 16384 1000 none output.txt $threads > $OUT_FILE
        sleep 1.5
        echo "$ts $prefix ./streamcluster$suffix 10 20 128 16384 16384 1000 none output.txt $threads > $OUT_FILE" >> $DEBUG_FILE
        cd - > /dev/null
      ;;
    esac
    time_in_us=`cat $OUT_FILE | grep "$1 runtime: " | cut -d ':' -f 2 | cut -d ' ' -f 2 | tr -d '[:space:]'`
    # echo "Run $j: $time_in_us us" 
    if [ ! -z "$time_in_us" ]; then
      echo $time_in_us | tr -d '\n' >> $SUM_FILE
      echo $time_in_us >> $MEDIAN_FILE
      echo "$time_in_us us" >> $DEBUG_FILE
      if [ $j -lt $RUNS ]; then
        echo -n "+" >> $SUM_FILE
      fi
    else
      echo "Run failed. Output: " >> $DEBUG_FILE
      cat $OUT_FILE >> $DEBUG_FILE
      exit
    fi
  done
  echo ")/$DIVISOR" >> $SUM_FILE 
  time_in_ms=`cat $SUM_FILE | bc` 
  echo "Average: $time_in_ms ms" >> $DEBUG_FILE
  time_in_ms=$(echo "scale=3; $(sort -n $MEDIAN_FILE | awk '{a[i++]=$1} END {print a[int(i/2)];}') / 1000" | bc -l)
  echo "Median: $time_in_ms ms" >> $DEBUG_FILE
  echo $time_in_ms
}

perf_test() {
  echo "=================================== PERFORMANCE TEST ==========================================="
  declare final_thread

  for thread in $THREADS
  do
    PER_THREAD_STAT_FILE="$DIR/parsec-perf_stats-th$thread-ad$AD.txt"
    echo -e "benchmark\torig\tnaive\tperiodic" > $PER_THREAD_STAT_FILE
  done

  for bench in $*
  do
    BENCH_DIR=""
    case "$bench" in
    "canneal" | "dedup" | "streamcluster")
      BENCH_DIR="kernels/"
      ;;
    *)
      BENCH_DIR="apps/"
      ;;
    esac

    LOG_FILE="$DIR/$bench-perf_logs.txt"
    DEBUG_FILE="$DIR/$bench-perf_debug.txt"

    cd $BENCH_DIR
    PER_BENCH_ORIG_STAT_FILE="$DIR/$bench-perf_orig-ad$AD.txt"
    PER_BENCH_PERIODIC_STAT_FILE="$DIR/$bench-perf_periodic-ad$AD.txt"
    PER_BENCH_NAIVE_STAT_FILE="$DIR/$bench-perf_naive-ad$AD.txt"
    echo "************* $bench ***************" | tee -a $LOG_FILE $DEBUG_FILE 
    echo "Unroll count: $UNROLL_COUNT" | tee -a $LOG_FILE $DEBUG_FILE
    echo "Accuracy test: $ACCURACY_TEST" | tee -a $LOG_FILE $DEBUG_FILE
    echo "Concord pass type: $CONCORD_PASS_TYPE" | tee -a $LOG_FILE $DEBUG_FILE

    #1. Build original program with pthread
    echo "Building original pthread program: " | tee -a $DEBUG_FILE $BUILD_DEBUG_FILE $BUILD_ERROR_FILE >/dev/null
    BUILD_LOG=$BUILD_DEBUG_FILE ERROR_LOG=$BUILD_ERROR_FILE make -f Makefile.llvm ${bench}-clean
    BUILD_LOG=$BUILD_DEBUG_FILE ERROR_LOG=$BUILD_ERROR_FILE make -f Makefile.llvm ${bench} 

    declare orig_time_thr1 periodic_time_thr1 naive_time_thr1 orig_time periodic_time naive_time
    for thread in $THREADS
    do
      PER_THREAD_STAT_FILE="$DIR/parsec-perf_stats-th$thread-ad$AD.txt"
      orig_time=$(get_time $bench $thread 0 0)
      # orig_time=1
      echo -ne "$bench" >> $PER_THREAD_STAT_FILE
      echo -ne "\t$orig_time" >> $PER_THREAD_STAT_FILE
      echo -e "$thread\t$orig_time" >> $PER_BENCH_ORIG_STAT_FILE
      echo -e "$thread, $orig_time (orig)" >> $LOG_FILE
      orig_time_thr1=$orig_time
    done

    #4. Build original program with Naive CI
    echo "Building original program with Naive CI: " | tee -a $DEBUG_FILE $BUILD_DEBUG_FILE $BUILD_ERROR_FILE >/dev/null
    ACCURACY_TEST=$ACCURACY_TEST CONCORD_PASS_TYPE=$CONCORD_PASS_TYPE UNROLL_COUNT=$UNROLL_COUNT BUILD_LOG=$BUILD_DEBUG_FILE ERROR_LOG=$BUILD_ERROR_FILE make -f Makefile.ci ${bench}-clean
    UINTR=$NOT_CONCORD ACCURACY_TEST=$ACCURACY_TEST CONCORD_PASS_TYPE=$CONCORD_PASS_TYPE UNROLL_COUNT=$UNROLL_COUNT BUILD_LOG=$BUILD_DEBUG_FILE ERROR_LOG=$BUILD_ERROR_FILE PROFILE_FLAGS="-DAVG_STATS" make -f Makefile.ci ${bench}
    for thread in $THREADS
    do
      PER_THREAD_STAT_FILE="$DIR/parsec-perf_stats-th$thread-ad$AD.txt"
      lc_naive_time=$(get_time $bench $thread 1 0)
      # lc_naive_time=1
      echo -ne "\t$lc_naive_time" >> $PER_THREAD_STAT_FILE
      echo -e "$thread\t$lc_naive_time" >> $PER_BENCH_NAIVE_STAT_FILE
      echo -e "$thread, $lc_naive_time (lc-naive)" >> $LOG_FILE
      naive_time_thr1=$lc_naive_time

      ACCURACY_FILE="$ACCURACY_DIR/$bench"
      # mv ${CONCORD_TIMESTAMP_PATH} $ACCURACY_FILE
      # rm ${CONCORD_TIMESTAMP_PATH}
    done
    sleep 1

    #Print
    echo -e "naive_time_thr1=$naive_time_thr1, orig_time_thr1=$orig_time_thr1"
    echo -e "Statistics for $bench" >> $LOG_FILE
    # slowdonw
    slowdown=$(echo "scale=3; ($naive_time_thr1 / $orig_time_thr1)" | bc)
    echo -e "Overhead: $slowdown"
    echo -e "Overhead $slowdown" >> $LOG_FILE
    
    cd ../ > /dev/null

  done
}

#1 - benchmark name (optional)
run_perf_test() {
  if [ $# -eq 0 ]; then
    perf_test blackscholes fluidanimate swaptions canneal dedup streamcluster
  else
    perf_test $@
  fi
}

rm -f $LOG_FILE $DEBUG_FILE $BUILD_ERROR_FILE $BUILD_DEBUG_FILE
#echo "Note: Script has both accuracy tests & performance tests. Change the mode in the next few lines if any one of them is required only. "
#echo "Note: Number of threads for running performance tests need to be configured inside the file"
echo "Configured values:-"
echo "Commit interval: $CI, Push Interval: $PI, Number of runs: $RUNS, Allowed deviation: $AD, Threads: $THREADS"
echo "Usage: ./perf_test_libfiber <nothing / space separated list of parsec benchmarks>"
mkdir -p $DIR
mkdir -p $ACCURACY_DIR
if [ $# -eq 0 ]; then
  run_perf_test
else
  run_perf_test $@
fi

rm -f $OUT_FILE $SUM_FILE
