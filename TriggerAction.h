#include <unistd.h>
#include <string.h>
#include <errno.h>
#include <sys/types.h>
#include "TriggerActionDecl.h"

//#ifdef INTV_SAMPLING /* Prints every interval stat, always prints ret_inst & time cycles */
//#ifdef PERF_CNTR /* reads perf counters for ret_inst & time cycles */
//#ifdef PAPI /* Uses hardware CI */
//#ifdef LIBFIBER /* for user level preemptive multitasking mode */
//#ifdef AVG_STATS /* compute avg over each sample on-the-go */

#ifndef OUTPUT_DIRECTORY
#define OUTPUT_DIRECTORY "//home/m8/clean-concord/CompilerInterrupts/local_home/exp_results/interval_stats"
#endif

#ifdef LIBFIBER
#undef INTV_SAMPLING
#undef PERF_CNTR
#undef PAPI
#undef AVG_STATS
#endif

#ifdef INTV_SAMPLING /* Export every interval stats */
#define PERF_CNTR
#endif

#ifdef INTV_SAMPLING
__thread uint64_t *buffer_tsc = NULL;
__thread long *buffer_ret_ic = NULL;
__thread long *buffer_ic = NULL;
#endif

__thread uint64_t last_tsc = 0;
__thread uint64_t last_ret_ic = 0;
__thread double last_avg_ic = 0;
__thread double last_avg_ret_ic = 0;
__thread double last_avg_tsc = 0;

__thread int thread_id = 0;

// To heapify a subtree rooted with node i which is
// an index in arr[]. n is size of heap
static void heapify(long arr[], int n, int i)
{
    int largest = i; // Initialize largest as root
    int l = 2*i + 1; // left = 2*i + 1
    int r = 2*i + 2; // right = 2*i + 2

    // If left child is larger than root
    if (l < n && arr[l] > arr[largest])
        largest = l;

    // If right child is larger than largest so far
    if (r < n && arr[r] > arr[largest])
        largest = r;

    // If largest is not root
    if (largest != i)
    {
        int tmp = arr[i];
        arr[i] = arr[largest];
        arr[largest] = tmp;

        // Recursively heapify the affected sub-tree
        heapify(arr, n, largest);
    }
}

// main function to do heap sort
static void heapSort(long arr[], int n)
{
    // Build heap (rearrange array)
    for (int i = n / 2 - 1; i >= 0; i--)
        heapify(arr, n, i);

    // One by one extract an element from heap
    for (int i=n-1; i>=0; i--)
    {
        // Move current root to end
        int tmp = arr[0];
        arr[0] = arr[i];
        arr[i] = tmp;

        // call max heapify on the reduced heap
        heapify(arr, i, 0);
    }
}

static int get_ir_interval() {
  char *ir_interval = getenv("CI_IR_INTERVAL");
  assert(ir_interval && "CI_IR_INTERVAL environment variable is not set!");
  int interval = atoi(ir_interval);
  assert(interval>0 && "CI_IR_INTERVAL cannot be less than or equal to 0!");
  return interval;
}

static int get_cycles_interval() {
  char *cycles_interval = getenv("CI_CYCLES_INTERVAL");
  assert(cycles_interval && "CI_CYCLES_INTERVAL environment variable is not set!");
  int interval = atoi(cycles_interval);
  assert(interval>0 && "CI_CYCLES_INTERVAL cannot be less than or equal to 0!");
  return interval;
}

static void pin_thread(unsigned long th_id) {
  //int max_cpus = sysconf(_SC_NPROCESSORS_ONLN);
#ifndef REVERSE_INDEX
  assert((th_id>=0) && "Thread id cannot be negative");
  cpu_set_t cpuset;
  int max_cpus_to_use = MAX_CPU;
  CPU_ZERO(&cpuset);
  // int cpu = th_id % (max_cpus_to_use-1);
  int cpu = 4;
  CPU_SET(cpu, &cpuset);
  pthread_t thread = pthread_self();
  printf("Pinning thread %d to cpu %d\n", th_id, cpu);
  int ret = pthread_setaffinity_np(thread, sizeof(cpu_set_t), &cpuset);
  if(ret != 0)
    printf("Unable to set thread affinity for thread %d to cpu %d. Returned: %d. Error: %s\n", th_id, cpu, ret, strerror(errno));
#endif
}

#ifdef LIBFIBER

/************************************************ LibFiber ******************************************************/
  void init_stats(int index) {
    //intvActionHook(0); // clang bug: __attribute__((used)) not working
    //register_ci(get_ir_interval(), get_cycles_interval(), compiler_interrupt_handler);
  }

  void compiler_interrupt_handler(long ic) {
    fiber_yield();
  }

  void print_timing_stats(void) {
  }

#elif defined(PROFILE)

/************************************************ Profiling ******************************************************/
  __thread int ci_count = 0;
  __thread struct timeval g_tv_prev;
  void init_stats(int index) {
    //intvActionHook(0); // clang bug: __attribute__((used)) not working
#ifdef SYS_gettid
    thread_id = syscall(SYS_gettid);
#else
    #error "SYS_gettid unavailable on this system"
#endif

    gettimeofday((struct timeval *) &g_tv_prev, NULL);

    pin_thread(thread_id);

    //register_ci(get_ir_interval(), get_cycles_interval(), compiler_interrupt_handler);
  }

  void compiler_interrupt_handler(long ic) {
    if(ci_count == 1000000) {
      struct timeval tv_curr;
      gettimeofday(&tv_curr, NULL);
      double diff_sec = (double)((tv_curr.tv_sec - g_tv_prev.tv_sec)*1000000 + (tv_curr.tv_usec - g_tv_prev.tv_usec))/1000000;
      g_tv_prev.tv_sec = tv_curr.tv_sec;
      g_tv_prev.tv_usec = tv_curr.tv_usec;

      printf("Thread %d: 1000000 CI took %lf sec\n", thread_id, diff_sec);
      ci_count = 0;
    }
    else {
      ci_count++;
    }
  }

  void print_timing_stats(void) {
    struct timeval tv_curr;
    gettimeofday(&tv_curr, NULL);
    double diff_sec = (double)((tv_curr.tv_sec - g_tv_prev.tv_sec)*1000000 + (tv_curr.tv_usec - g_tv_prev.tv_usec))/1000000;
    printf("Thread %d: %d CI took %lf sec\n", thread_id, ci_count, diff_sec);
  }

#elif defined(INTV_SAMPLING)

  __thread uint64_t residue_intv = 0;

  void ci_disable_fn() {
    uint64_t rax_lo, rdx_hi, aux_cpuid;

    if(last_tsc) {
      asm volatile ( "rdtscp\n" : "=a" (rax_lo), "=d" (rdx_hi), "=c" (aux_cpuid) : : );
      residue_intv += ((rdx_hi << 32) + rax_lo) - last_tsc;
    }

    last_tsc=0;
    //printf("Disable function got called!! Yayyy!!\n");
    //compiler_interrupt_handler(0);
  }

  void ci_enable_fn() {
    uint64_t rax_lo, rdx_hi, aux_cpuid;

    asm volatile ( "rdtscp\n" : "=a" (rax_lo), "=d" (rdx_hi), "=c" (aux_cpuid) : : );
    last_tsc = (rdx_hi << 32) + rax_lo;
    //printf("Enable function got called!! Yayyy!!\n");

    //int ecx = (1<<30); // For instruction count // https://software.intel.com/en-us/forums/software-tuning-performance-optimization-platform-monitoring/topic/595214
    //asm volatile("rdpmc\n" : "=a"(rax_lo), "=d"(rdx_hi) : "c"(ecx));
    //last_ret_ic = (rax_lo | (rdx_hi << 32));
  }

  void init_stats(int index) {

    //intvActionHook(0); // clang bug: __attribute__((used)) not working
    /* if the thread local variable has been initialized, it means this is a duplicate call for the same thread */
    if(buffer_tsc)
      return;

    /* Delete all previous interval stats files */
    char command[500];
#ifdef SYS_gettid
    thread_id = syscall(SYS_gettid);
    sprintf(command, "exec rm -f %s/interval_stats_thread%ld.txt", OUTPUT_DIRECTORY, thread_id);
    system(command);
#else
    #error "SYS_gettid unavailable on this system"
#endif

    pin_thread(thread_id);

    if(!buffer_tsc) 
      buffer_tsc = (uint64_t *)calloc(BUF_SIZE, sizeof(uint64_t));
    //if(!buffer_ret_ic) 
      //buffer_ret_ic = (long *)malloc(BUF_SIZE*sizeof(long));
    //if(!buffer_ic) 
      //buffer_ic = (long *)malloc(BUF_SIZE*sizeof(long));

    /* This setting is done per thread (using pid parameter of perf_event_open)*/
    struct perf_event_attr attr;
    memset(&attr, 0, sizeof(struct perf_event_attr));
    attr.type = PERF_TYPE_HARDWARE;
    attr.size = sizeof(struct perf_event_attr);
    attr.config = PERF_COUNT_HW_INSTRUCTIONS;
    attr.inherit = 1;
    attr.pinned = 1;
    attr.exclude_idle = 1;
    attr.exclude_kernel = 1;
    //int perf_fds = perf_event_open(&attr, getpid(), -1, -1, 0);
    int perf_fds = perf_event_open(&attr, 0, -1, -1, 0);
    ioctl(perf_fds, PERF_EVENT_IOC_RESET, 0); // Resetting counter to zero
    ioctl(perf_fds, PERF_EVENT_IOC_ENABLE, 0); // Start counters

    //register_ci(get_ir_interval(), get_cycles_interval(), compiler_interrupt_handler);

    #if 1
    //register_ci_enable_hook(ci_enable_fn);
    //register_ci_disable_hook(ci_disable_fn);
    #endif
  }

  /*********************** For Trigger, with every interval stat being directly stored **********************/
  __thread unsigned int ci_count = 0;
  __thread unsigned int no_data_count = 0;
  __thread unsigned int outlier_idx = BUCKET_SIZE;
  /* Code path for getting interval statistics */
  void compiler_interrupt_handler(long ic) {

    /* get the time stamp */
    uint64_t rax_lo, rdx_hi, aux_cpuid;

    asm volatile ( "rdtscp\n" : "=a" (rax_lo), "=d" (rdx_hi), "=c" (aux_cpuid) : : );
    uint64_t curr_tsc = (rdx_hi << 32) + rax_lo;
    uint64_t tsc = curr_tsc - last_tsc + residue_intv;

    /*
    int ecx = (1<<30); // For instruction count // https://software.intel.com/en-us/forums/software-tuning-performance-optimization-platform-monitoring/topic/595214
    asm volatile("rdpmc\n" : "=a"(rax_lo), "=d"(rdx_hi) : "c"(ecx));
    uint64_t curr_ret_ic = (rax_lo | (rdx_hi << 32));
    */

    /* TSC & RET IC are not absolute values, but the interval is defined by the difference between the current & the last */
    if(buffer_tsc && last_tsc) {
      residue_intv = 0;
      if(tsc > 0 && tsc < BUCKET_SIZE) { // histogram
        buffer_tsc[tsc]++;
      } else if (tsc >= BUCKET_SIZE && outlier_idx < BUF_SIZE) {
        buffer_tsc[outlier_idx++]=tsc;
      } else no_data_count++;
    } else no_data_count++;

    /* Resetting all static variables */
    asm volatile ( "rdtscp\n" : "=a" (rax_lo), "=d" (rdx_hi), "=c" (aux_cpuid) : : );
    last_tsc = (rdx_hi << 32) + rax_lo;

    ci_count++;
  }

  void print_timing_stats() {
    //ci_disable();

    /* Print every interval */
    char name[500];
#ifdef SYS_gettid
    sprintf(name, "%s/interval_stats_thread%ld.txt", OUTPUT_DIRECTORY, thread_id);
#else
    #error "SYS_gettid unavailable on this system"
#endif


    /*
    if( access(name, F_OK ) == 0 ) {
      printf("%s already existed. Appending to it.\n", name);
    }
    */

    FILE *fp = fopen(name, "w");
    if(!fp) {
      printf("Could not open file %s to write\n", name);
      exit(1);
    }

    //printf("PushSeq IC RIC TSC\n");
    fprintf(fp, "#Total CI calls: %u\n", ci_count);
    fprintf(fp, "#Uncollected Samples: %u\n", no_data_count);
    fprintf(fp, "PushSeq IC RIC TSC\n");

    int i,j,k=0;
    if(buffer_tsc) {
      for(i=0; i<BUCKET_SIZE; i++) {
        if(buffer_tsc[i]>0) {
          for(j=0; j<buffer_tsc[i]; j++) {
            fprintf(fp, "%d %d %d %d\n", k++, 0, 0, i);
          }
        }
      }

      for(i=BUCKET_SIZE; i<outlier_idx; i++) {
        fprintf(fp, "%d %d %d %ld\n", k++, 0, 0, buffer_tsc[i]);
      }
    }

    fclose(fp);
    //ci_enable();
  }

#elif defined(PERF_CNTR)
  void init_stats(int index) {
    //intvActionHook(0); // clang bug: __attribute__((used)) not working
    /* This setting is done per thread (using pid parameter of perf_event_open)*/
    struct perf_event_attr attr;
    memset(&attr, 0, sizeof(struct perf_event_attr));
    attr.type = PERF_TYPE_HARDWARE;
    attr.size = sizeof(struct perf_event_attr);
    attr.config = PERF_COUNT_HW_INSTRUCTIONS;
    attr.inherit = 1;
    attr.pinned = 1;
    attr.exclude_idle = 1;
    attr.exclude_kernel = 1;
    int perf_fds = perf_event_open(&attr, 0, -1, -1, 0);
    ioctl(perf_fds, PERF_EVENT_IOC_RESET, 0); // Resetting counter to zero
    ioctl(perf_fds, PERF_EVENT_IOC_ENABLE, 0); // Start counters

#ifdef SYS_gettid
    thread_id = syscall(SYS_gettid);
#else
    #error "SYS_gettid unavailable on this system"
#endif

    pin_thread(thread_id);

    //register_ci(get_ir_interval(), get_cycles_interval(), compiler_interrupt_handler);
  }

  __thread int64_t sample_count = -1;
  void compiler_interrupt_handler(long ic) {
      /* get the time stamp */
    uint64_t rax_lo, rdx_hi, aux_cpuid;

    asm volatile ( "rdtscp\n" : "=a" (rax_lo), "=d" (rdx_hi), "=c" (aux_cpuid) : : );
    uint64_t curr_tsc = (rdx_hi << 32) + rax_lo;

    int ecx = (1<<30); // For instruction count // https://software.intel.com/en-us/forums/software-tuning-performance-optimization-platform-monitoring/topic/595214
    asm volatile("rdpmc\n" : "=a"(rax_lo), "=d"(rdx_hi) : "c"(ecx));
    uint64_t curr_ret_ic = (rax_lo | (rdx_hi << 32));

    /* TSC & RET IC are not absolute values, but the interval is defined by the difference between the current & the last */
    if(sample_count >= 0) {
      last_avg_tsc = (double)((last_avg_tsc*sample_count) + (curr_tsc - last_tsc)) / (sample_count+1);
      last_avg_ret_ic = (double)((last_avg_ret_ic*sample_count) + (curr_ret_ic - last_ret_ic)) / (sample_count+1);
      last_avg_ic = (double)((last_avg_ic*sample_count) + ic) / (sample_count+1);
    }

    /* Resetting all static variables */
    last_tsc = curr_tsc;
    last_ret_ic = curr_ret_ic;

    sample_count++;
  }

  void print_timing_stats() {
    // //ci_disable();
    printf("samples:%d, avg_intv_cycles:%0.1lf, avg_intv_ic:%0.1lf\n", sample_count, last_avg_tsc, last_avg_ic);
    // //ci_enable();
  }

#elif defined(AVG_STATS)

  /* this function gets called from all threads, but twice from main. disregard it the second time */
  void init_stats(int index) {
#ifdef SYS_gettid
    thread_id = syscall(SYS_gettid);
#else
    #error "SYS_gettid unavailable on this system"
#endif
    //intvActionHook(0); // clang bug: __attribute__((used)) not working
    pin_thread(thread_id);

    //register_ci(get_ir_interval(), get_cycles_interval(), compiler_interrupt_handler);
  }

  /*********************** For Trigger, with every interval stat being directly stored **********************/
  /* Code path for getting interval statistics */
  __thread int64_t sample_count = 0;
  void compiler_interrupt_handler(long ic) {
    if(sample_count >= 0)
      last_avg_ic = (double)((last_avg_ic*sample_count) + ic) / (sample_count+1);

    sample_count++;
  }

  void print_timing_stats() {
    //ci_disable();
#ifdef SYS_gettid
    printf("Thread %d: avg_intv_ic:%0.1lf\n", syscall(SYS_gettid), last_avg_ic);
#else
    #error "SYS_gettid unavailable on this system"
#endif
    //ci_enable();
  }

#elif defined(PAPI)

  /*************************************** For PAPI *********************************************/
  #include <pthread.h>

  /* For PAPI */
  __thread int events[NUM_HWEVENTS] = { PAPI_TOT_CYC, PAPI_TOT_INS };
  __thread int event_set[MAX_COUNT];

  __thread int counter_id = 0;
  __thread int counter_id_alloc = 0;
  int __get_id_and_increment() {
    //using the gcc atomic built-ins
    return __sync_fetch_and_add(&counter_id_alloc, 1);
  }

  void init_stats(int index) {
    //intvActionHook(0); // clang bug: __attribute__((used)) not working
#ifdef SYS_gettid
    thread_id = syscall(SYS_gettid);
#else
    #error "SYS_gettid unavailable on this system"
#endif

    pin_thread(thread_id);
    instruction_counter_init();
    instruction_counter_register_thread();
  }

  __thread int64_t sample_count = -1;
  void hw_interrupt_handler(long long tot_inst, long long tot_cyc) {

    if(sample_count >= 0) {
        last_avg_tsc = (double)((last_avg_tsc*sample_count) + tot_cyc) / (sample_count+1);
        last_avg_ret_ic = (double)((last_avg_ret_ic*sample_count) + tot_inst) / (sample_count+1);
    }

    sample_count++;
  }

  void papi_interrupt_handler(int i, void *v1, long_long ll, void *v2) {
    long long counter_values[NUM_HWEVENTS];
    if (PAPI_read(event_set[counter_id], counter_values) != PAPI_OK){
      //printf("PAPI_read failed with error %s\n", strerror(errno));
      return;
    }
    //assert(counter_values[TOT_INST_IDX] >= 0 && counter_values[TOT_CYC_IDX] >= 0);
    //printf("Counter: %lld, Cycles: %lld\n", counter_values[TOT_INST_IDX], counter_values[TOT_CYC_IDX]);
    assert(counter_values[TOT_CYC_IDX] >= 0);
    assert(counter_values[TOT_INST_IDX] >= 0);
    __reset();
    hw_interrupt_handler(counter_values[TOT_INST_IDX], counter_values[TOT_CYC_IDX]);
  }

  void print_timing_stats(void) {
    /* Print every interval */
    printf("samples:%ld, avg_intv_cycles:%0.1lf, avg_intv_ret_ic:%0.1lf\n", sample_count, last_avg_tsc, last_avg_ret_ic);
  }

  /* Called once in the program, from main() */
  int instruction_counter_init() {

    static int first = 0;

    if(first!=0)
      return 0;
    else
      first++;

    int retval = PAPI_library_init( PAPI_VER_CURRENT );

    if ( retval != PAPI_VER_CURRENT ){
      perror("PAPI: library failed...");
      return -1;
    }

    if (PAPI_thread_init(pthread_self) != PAPI_OK) {
      perror("PAPI: failed to init thread...");
      return -1;
    }

    return 0;
  }

  /* Called per thread */
  int instruction_counter_register_thread(){

    counter_id = __get_id_and_increment(); // counter_id becomes 0 the first time
    if(counter_id) // thread has already been registered
      return 0;

    PAPI_register_thread();

    /*set domain*/
    if (PAPI_set_domain(PAPI_DOM_USER) != PAPI_OK) {
      perror("PAPI: domain set failed...");
      return -1;
    }

    /* Create an EventSet */ 
    memset(event_set, PAPI_NULL, sizeof(int) * MAX_COUNT);
    int err = PAPI_create_eventset(&event_set[counter_id]);
    if (err != PAPI_OK) {
      perror("PAPI: event set failed...");
      printf("create eventset failed with error: %s\n", strerror(errno));
      return -1;
    }

    int event_codes[NUM_HWEVENTS] = {PAPI_TOT_INS, PAPI_TOT_CYC};
    if (PAPI_add_events(event_set[counter_id], event_codes, NUM_HWEVENTS) != PAPI_OK) {
      perror("PAPI: add events failed...");
      printf("add eventset failed for thread %ld with error: %s\n", thread_id, strerror(errno));
      return -1;
    }

    printf("Using PAPI interval threshold %d for thread %ld (counter id: %d, %d)\n", get_cycles_interval(), thread_id, counter_id, counter_id_alloc);
    //printf("Setting handler for thread %d\n", counter_id);
    int ret = PAPI_overflow(event_set[counter_id], PAPI_TOT_CYC, get_cycles_interval(), 0, papi_interrupt_handler);
    //printf("Have set handler for thread %d\n", counter_id);
    if (ret != PAPI_OK){
      printf("PAPI_overflow returned : %d for counter id %d\n", ret, counter_id);
      printf("PAPI: failed to register handler function for overflow with error %s\n", strerror(errno));
      return -1;
    }

    ret = PAPI_start(event_set[counter_id]);
    if (ret != PAPI_OK && ret != PAPI_EISRUN) {
      perror("PAPI: failed to start counters...");
      printf("start counters failed with error %s\n", strerror(errno));
      return -1;
    }
    return 0;
  }

  int __reset() {
    if (PAPI_reset(event_set[counter_id]) != PAPI_OK){
      //perror("PAPI: failed to read counter...");
      return -1;
    }
    else{
      return 0;
    }
  }

  int instruction_counter_stop() {
    long long counter_values[NUM_HWEVENTS];
    printf("PAPI stop for thread %d\n", counter_id);
    if (PAPI_stop(event_set[counter_id], counter_values) != PAPI_OK) {
      perror("PAPI: failed to stop counter...");
      return -1;
    }
    //printf("Counter at stop: %lld\n", counter_values[TOT_INST_IDX]);
    return 0;
  }

#else

  /************************************* Default implementations ***********************************/
  void init_stats(int index) {
    //intvActionHook(0); // clang bug: __attribute__((used)) not working
#ifdef SYS_gettid
    thread_id = syscall(SYS_gettid);
#else
    #error "SYS_gettid unavailable on this system"
#endif
    pin_thread(thread_id);
  }

  void print_timing_stats(void) {
  }

#endif
