#include <jni.h>
#include <android/log.h>
#include <pthread.h>
#include <sys/time.h>
#include <dirent.h>


#define LOG(fmt, ...) __android_log_print(ANDROID_LOG_ERROR, "NDKSignalTest", fmt, __VA_ARGS__)

#define KILLSIG SIGUSR2
#define NWORKERS 4
#define MAX_THREADS 100
#define SLEEP_SECS 4
#define ITIMER_INTERVAL_USECS 100 // gprof use the same amount: http://sourceware.org/binutils/docs/gprof/Implementation.html
#define LOG_INTERVAL 2 // throttle the logging

static pthread_mutex_t workerLock = PTHREAD_MUTEX_INITIALIZER;
static unsigned int otherThreadCount = 0;

static pid_t mother_thread_id = 0;
static pid_t logging_thread_id = 0;
static pid_t worker_thread_ids[NWORKERS];
static pid_t other_thread_ids[MAX_THREADS];

static volatile sig_atomic_t mother_signal_count = 0;
static volatile sig_atomic_t logging_signal_count = 0;
static volatile sig_atomic_t worker_signal_count[NWORKERS];
static volatile sig_atomic_t other_signal_count[MAX_THREADS];
static volatile sig_atomic_t unknown_signal_count = 0;

static void sig_handler(int sig_nr, siginfo_t *info, void *context) {
    pid_t me = gettid();
    if (me == mother_thread_id) {
        ++mother_signal_count;
        return;
    } else if (me == logging_thread_id) {
        ++logging_signal_count;
        return;
    } else {
        unsigned int t;
        for (t = 0; t < NWORKERS; ++t) {
            if (me == worker_thread_ids[t]) {
                ++worker_signal_count[t];
                return;
            }
        }
        for (t = 0; t < otherThreadCount; ++t) {
            if (me == other_thread_ids[t]) {
                ++other_signal_count[t];
                return;
            }
        }
    }
    ++unknown_signal_count;
}

static void hardly_working(int seconds) {
    time_t start = time(NULL);
    while (seconds) {
        unsigned int size = arc4random_uniform(1024*1024);
        char *allocated = malloc(size);
        seconds = sleep(seconds);
        if (seconds) {
            //LOG("thread %d interrupted", gettid());
        }
        free(allocated);
    }
}

static unsigned int _workerThreadCount = 0;
static void *worker_thread(void *ignored) {
    LOG("worker thread %d started ...", gettid());

    pthread_mutex_lock(&workerLock);
    worker_thread_ids[_workerThreadCount++] = gettid();
    pthread_mutex_unlock(&workerLock);

    while (1) {
        hardly_working(SLEEP_SECS);
    }
    return NULL;
}

static void *logging_thread(void *ignored) {
    logging_thread_id = gettid();
    LOG("logging thread %d started ...", logging_thread_id);
    time_t prev = time(NULL);

    while (1) {
        //idle_time(gettid(), SLEEP_SECS);
        sleep(LOG_INTERVAL);
        time_t now = time(NULL);
        if (now-prev > LOG_INTERVAL) {
            prev = now;
            LOG("%s", "-------------------------------------------------------");
            if (mother_signal_count) {
                LOG("mother %d signal count:%d", mother_thread_id, mother_signal_count);
            }
            if (unknown_signal_count) {
                LOG("unknown signal count:%d", unknown_signal_count);
            }
            if (logging_signal_count) {
                LOG("logging thrad %d signal count:%d", logging_thread_id, logging_signal_count);
            }
            unsigned int t;
            for (t = 0; t < NWORKERS; ++t) {
                LOG("worker thread %d signal count : %d", worker_thread_ids[t], worker_signal_count[t]);
            }
            for (t = 0; t < otherThreadCount; ++t) {
                LOG("other thread %d signal count : %d", other_thread_ids[t], other_signal_count[t]);
            }
        }
    }

    return NULL;
}

static void gather_other_threads(void) {
    const char *thread_dir = "/proc/self/task";
    DIR *dirp = opendir(thread_dir);
    if (dirp == NULL) {
        LOG("OOPS, could not read %s", thread_dir);
        return;
    }

    struct dirent *dp = NULL;
    while ((dp = readdir(dirp)) != NULL) {
        pid_t tid = strtol(dp->d_name, NULL, 10);
        if (tid && (tid != mother_thread_id) && (tid != logging_thread_id)) {
            unsigned char found = 0;
            unsigned int t;
            for (t = 0; t < NWORKERS; ++t) {
                if (tid == worker_thread_ids[t]) {
                    found = 1;
                }
            }
            if (!found) {
                LOG("found other thread %d", tid);
                other_thread_ids[otherThreadCount++] = tid;
            }
        }
        if (otherThreadCount == MAX_THREADS) {
            LOG("%s", "OOPS, max thread monitoring count reached ...");
            break;
        }
    }
    closedir(dirp);
}

static void *mother_thread(void *ignored) {
    mother_thread_id = gettid();

    unsigned int t;
    for (t = 0; t < MAX_THREADS; ++t) {
        other_signal_count[t] = 0;
    }
    for (t = 0; t < NWORKERS; ++t) {
        worker_signal_count[t] = 0;
    }

    LOG("%s", "BEGIN NDKSignalTest ...");

    // install signal handler
    struct sigaction sa;
    memset(&sa, 0, sizeof(sa));
    sa.sa_sigaction = sig_handler;
    sa.sa_flags = SA_RESTART | SA_SIGINFO;
    sigemptyset(&sa.sa_mask);
    struct sigaction oldsa;
    sigaction(KILLSIG, &sa, &oldsa);
    if (oldsa.sa_sigaction) {
        LOG("Interesting ... old sigaction : %p", oldsa.sa_sigaction); // presumably should be 0x0
    }

    // spin off some native threads
    pthread_t thread;
    int err = pthread_create(&thread, NULL, logging_thread, NULL);
    if (err) {
        LOG("%s", "Problem with pthread_create() ...");
    }
    for (t = 0; t < NWORKERS; ++t) {
        int err = pthread_create(&thread, NULL, worker_thread, NULL);
        if (err) {
            LOG("%s", "Problem with pthread_create() ...");
        }
    }
    // wait for stuff-n-things to settle a bit more ...
    sleep(SLEEP_SECS);

    // gather all the other TIDs
    gather_other_threads();

    // --------------------------------------------------------------------------------------------
    // Problematic section (causes crashes on Android 4.3+) ...
    while (1) {
        usleep(40);
        tkill(logging_thread_id, KILLSIG);
        for (t = 0; t < NWORKERS; ++t) {
            tkill(worker_thread_ids[t], KILLSIG);
        }
#if 1
        /* Kill other (Java) threads */
        for (t = 0; t < otherThreadCount; ++t) {
            tkill(other_thread_ids[t], KILLSIG);
        }
#endif
    }
    // --------------------------------------------------------------------------------------------

    return NULL;
}

void Java_com_apportable_ndksignaltest_NDKSignalTest_beginTest(JNIEnv *env, jclass clazz) {
    pthread_t thread;
    pthread_create(&thread, NULL, mother_thread, NULL);
}
