#define _POSIX_SOURCE
#define _GNU_SOURCE

#define NELEMS(x)  (sizeof(x) / sizeof((x)[0]))


#include <stdio.h>
#include <signal.h>
#include <stdlib.h>
#include <unistd.h>
#include <papi.h>
#include <pthread.h>
#include <string.h>
#include <sys/syscall.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <sys/time.h>
#include <sched.h>

/**********
 * Name: write_measurements_to_file
 * Description: stores all measurements to a file with the provided output filename
 * ********/

struct PAPI_event
{
    int event;
    char event_name[64];
};

typedef struct PAPI_event PAPI_event;

PAPI_event PAPI_events[] = {
    {PAPI_TOT_INS, "PAPI_TOT_INS"},
    {PAPI_L2_TCM, "PAPI_L2_TCM"},
    {PAPI_L2_DCA, "PAPI_L2_DCA"},
    {PAPI_L3_TCA, "PAPI_L3_TCA"},
    {PAPI_L3_TCM, "PAPI_L3_TCM"}
};

int write_measurements_to_file(unsigned int nr_counters, long long (*measurements)[nr_counters], unsigned int num_measurements,  char *output_filename)
{
    FILE *fp = fopen(output_filename, "w");
    for (size_t i = 0; i < num_measurements; i++)
    {
        for(size_t j = 0; j < nr_counters; j++)
        {
            fprintf(fp, "%llu,", measurements[i][j]);
        }
        fprintf(fp, "\n");
    }
    fclose(fp);
    return 0;
}

void print_header(int nr_counters)
{
    printf("<-- PAPI Counters -->\n");
    for(size_t i = 0; i < nr_counters; i++)
    {
        printf("%s\t", PAPI_events[i].event_name);
    }
    printf("\n");
    return;
}

void print_help()
{
    printf("\n");
    printf("***** Process monitor *****\n");
    printf("Usage: ./process_monitor <number of measurements> <interval in seconds> <write to file> <path to executable to be monitored> \n");
    printf("Params: \n");
    printf(" number of measurements \t <int> \t: number of measurements the monitor will perform before terminating \n");
    printf(" interval in seconds \t <int> \t: with which interval the monitor will take measurements of application \n");
    printf(" write to file \t <int> \t \t: write measurements to CSV file (0 for yes, 1 for no) \n");
    printf(" path to executable \t <string> \t: path to the executable that the process monitor will spawn \n");
    printf("\n");
    printf("Example: ./process_monitor 100 1 1 /home/janne/asm/instructionloop\n");
    printf("\n");
    return;
}

int main(int argc, char *argv[])
{
    if (argc < 4)
    {
        printf("Error: too few arguments.\n");
        print_help();
        return -1;
    }

    int PAPI_eventset = PAPI_NULL;
    unsigned int num_measurements = atoi(argv[1]);
    unsigned int sleep_time = atoi(argv[2]);
    int write_to_file = atoi(argv[3]);
    char executable_path[256] = "";
    // int core = atou(argv[5]);
    strcpy(executable_path, argv[4]);
    // pid_t pid = atoi(argv[5]);

    char outputfile_name[64] = "output.csv";
    int return_code = 0;
    
    /* Set up affinity mask */
    // cpu_set_t cpu_set;
    // CPU_ZERO(&cpu_set);
    // CPU_SET(core, &cpu_set);

    /* Spawn process args */
    char *spawn_args[] = {"4096 ", "0", "1 ", NULL};

   
    int nr_counters = NELEMS(PAPI_events);
    printf("%d\n", nr_counters);
    long long values[nr_counters];
    long long values_storage[num_measurements][nr_counters];
    PAPI_option_t opt;

    printf("PAPI Version: %d\n", PAPI_VER_CURRENT);
    printf("Performing %d measurements with %d second intervals\n", num_measurements, sleep_time);

    if (PAPI_library_init(PAPI_VER_CURRENT) != PAPI_VER_CURRENT)
    {
        perror("Could not init PAPI\n");
        exit(-1);
    }
    if ((return_code = PAPI_create_eventset(&PAPI_eventset) != PAPI_OK))
    {
        perror("Could not create eventset\n");
        exit(-1);
    }

    memset(&opt, 0x0, sizeof(PAPI_option_t));
    opt.inherit.inherit = PAPI_INHERIT_ALL;
    opt.inherit.eventset = PAPI_eventset;

    if ((return_code = PAPI_assign_eventset_component(PAPI_eventset, 0)) != PAPI_OK)
    {
        printf("ERROR: PAPI_assign_eventset_component %d: %s\n", return_code, PAPI_strerror(return_code));
        exit(-1);
    }

    if ((return_code = PAPI_set_opt(PAPI_INHERIT, &opt)) != PAPI_OK)
    {
        printf("PAPI_set_opt error %d: %s\n", return_code, PAPI_strerror(return_code));
        exit(-1);
    }

    printf("Adding %d PAPI events to eventset\n", nr_counters);

    for(size_t i = 0; i < nr_counters; i++)
    {
        if ((return_code = PAPI_add_event(PAPI_eventset, PAPI_events[i].event) != PAPI_OK))
        {
            perror("could not add event to eventset\n");
            exit(-1);
        }
    }
    
    pid_t child_pid = fork();
    if (child_pid < 0)
    {
        perror("Fork() failed.\n");
        exit(-1);
    }
    if (child_pid == 0)
    {
        printf("Started provided executable process with pid: %d\n.", getpid());
        execl(executable_path, "2048", "0", "0", NULL);
        exit(-1);
    }
    else
    {
        /* Affine child process to cpu set mask */

        /* Parent attaches PAPI to child */

        printf("Attaching to pid %d\n", child_pid);
        sleep(1);
        if ((return_code = PAPI_attach(PAPI_eventset, (unsigned long)child_pid) != PAPI_OK))
        {
            perror("Could not attach PAPI to process\n");
            exit(-1);
        }

        /* Start PAPI counters */

        if ((return_code = PAPI_start(PAPI_eventset) != PAPI_OK))
        {
            perror("could not start PAPI\n");
            exit(-1);
        }

        print_header(nr_counters);

        /* Measure for num_measurements */

        for (size_t i = 0; i < num_measurements; i++)
        {
            sleep(sleep_time);
            PAPI_read(PAPI_eventset, values);
            /* Print counter values */
            for(size_t j = 0; j < nr_counters; j++)
            {
                printf("%lld \t\t", values[j]);
                values_storage[i][j] = values[j];
            }
            printf("\n");
            //*values_storage[i] = *values;
            PAPI_reset(PAPI_eventset);
        }

        /* Write output to file is requested */

        if (write_to_file == 0)
        {
            char file_name[32];
            sprintf(file_name, "%d", child_pid);
            strcat(file_name, outputfile_name);
            printf("Writing measurements to output file %s\n", file_name);
            write_measurements_to_file(nr_counters, values_storage, num_measurements, file_name);
        }

        /* Kill the child process */

        if(kill(child_pid, SIGTERM) != 0)
        {
            perror("Could not terminate process.\n");
            exit(-1);
        }
        printf("Application terminated.\n");
    }
    return 0;
}