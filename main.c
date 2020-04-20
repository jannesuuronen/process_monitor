#define _POSIX_SOURCE
#define _GNU_SOURCE

#define NELEMS(x)  (sizeof(x) / sizeof((x)[0]))
#define SEC_TO_NS(x) (x * 1000000000)

#include <stdio.h>
#include <signal.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <pthread.h>
#include <sys/syscall.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <sys/time.h>
#include <sched.h>
#include <assert.h>
#include <signal.h>
#include <papi.h>

/**********
 * Name: write_measurements_to_csv_file
 * Description: stores all measurements to a file with the provided output filename
 * ********/

struct PAPI_event
{
    int event;
    char *event_name;
};

typedef struct PAPI_event PAPI_event;

PAPI_event PAPI_events[] = {
    {PAPI_TOT_INS, "PAPI_TOT_INS"},
    {PAPI_L2_TCM, "PAPI_L2_TCM"},
    {PAPI_L2_DCA, "PAPI_L2_DCA"},
    {PAPI_L3_TCA, "PAPI_L3_TCA"},
    {PAPI_L3_TCM, "PAPI_L3_TCM"}
};

int write_measurements_to_csv_file(unsigned int nr_counters, long long (*measurements)[nr_counters], unsigned int num_measurements,  char *output_filename)
{
    FILE *fp = fopen(output_filename, "w");

    /* write column line */
    for (size_t i = 0; i < nr_counters; i++)
    {
        fprintf(fp, "%s,", PAPI_events[i].event_name);
    }
    fprintf(fp, "\n");
    
    /* write captured events */
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

long long calculate_counter_average(long long (*measurements), unsigned int num_measurements)
{
    long long sum = 0;

    for(size_t i = 0; i < num_measurements; i++)
    {
        sum += measurements[i];
    }

    return (sum/num_measurements);
}

void print_counter_averages(unsigned int nr_counters, long long (*measurements)[nr_counters], unsigned int num_measurements)
{
    printf("\n");
    printf("***** Average of captured metrics *****\n");
    for (size_t i = 0; i < nr_counters; i++)
    {
        printf("%s:\t %lld\n", PAPI_events[i].event_name, calculate_counter_average(measurements[i], num_measurements));
    }
    printf("\n");
}

void print_help()
{
    printf("\n");
    printf("***** Process monitor *****\n");
    printf("Usage: ./process_monitor <number of measurements> <interval in milliseconds> <write to file> <path to executable to be monitored> \n");
    printf("Params: \n");
    printf(" number of measurements \t <int> \t: number of measurements the monitor will perform before terminating \n");
    printf(" interval in nanoseconds \t <int> \t: with which interval the monitor will take measurements of application \n");
    printf(" write to file \t <int> \t \t: write measurements to CSV file (0 for yes, 1 for no) \n");
    printf(" path to executable \t <string> <space seperated argument list> \t: path to the executable that the process monitor will spawn with the provided arguments\n");
    printf("\n");
    printf("Example: ./process_monitor 100 10000000 1 /home/janne/asm/instructionloop\n");
    printf("Example: ./process_monitor 200 1 1 /home/janne/payloads/Palloc_program/Matmult/matmult 512 0 0\n");
    printf("\n");
    return;
}


int main(int argc, char const **argv)
{
    if (argc < 4)
    {
        printf("Error: too few arguments.\n");
        print_help();
        return -1;
    }
  
    int num_measurements = atoi(argv[1]);
    int PAPI_eventset = PAPI_NULL;
    int sleep_time = (1000 * atoi(argv[2])); 
    int write_to_file = atoi(argv[3]);
    int return_code = 0;
    char executable_path[256];
    char *test = malloc(128);
    char outputfile_name[64] = "output.csv";
    char **spawn_args = (char**)malloc(512 * sizeof(char *));

    for (size_t i = 0; i < argc; i++)
    {
        spawn_args[i] = (char *)malloc(256 * sizeof(char));
    }
    
    /* copy the executable path */
    strcpy(executable_path, argv[4]);
    executable_path[255] = '\0';

    /* assign executable path to the first cmd-line argument */
    spawn_args[0] = executable_path;

    /* store command lines arguments for provided executable to argument array */
    int j = 1;
    for (size_t i = 5; i < argc; i++)
    {
        sprintf(spawn_args[j], "%s", argv[i]);
        j++;
    }
    spawn_args[++j] = NULL;

    /* sanity check */    
    assert(sleep_time >= 0);
    assert(num_measurements > 0);

    int nr_counters = NELEMS(PAPI_events);
    long long values[nr_counters];
    long long values_storage[num_measurements][nr_counters];
    PAPI_option_t opt;

    printf("PAPI Version: %d\n", PAPI_VER_CURRENT);
    printf("Performing %d measurements with %d ms intervals\n", num_measurements, sleep_time/1000);

    if (PAPI_library_init(PAPI_VER_CURRENT) != PAPI_VER_CURRENT)
    {
        perror("Could not init PAPI\n");
        exit(-1);
    }
    if (PAPI_create_eventset(&PAPI_eventset) != PAPI_OK)
    {
        perror("Could not create eventset\n");
        exit(-1);
    }

    memset(&opt, 0x0, sizeof(PAPI_option_t));
    opt.inherit.inherit = PAPI_INHERIT_ALL;
    opt.inherit.eventset = PAPI_eventset;

    if (PAPI_assign_eventset_component(PAPI_eventset, 0) != PAPI_OK)
    {
        printf("ERROR: PAPI_assign_eventset_component %d: %s\n", return_code, PAPI_strerror(return_code));
        exit(-1);
    }

    if (PAPI_set_opt(PAPI_INHERIT, &opt) != PAPI_OK)
    {
        printf("PAPI_set_opt error %d: %s\n", return_code, PAPI_strerror(return_code));
        exit(-1);
    }

    printf("Adding %d PAPI events to eventset\n", nr_counters);

    for(size_t i = 0; i < nr_counters; i++)
    {
        if (PAPI_add_event(PAPI_eventset, PAPI_events[i].event) != PAPI_OK)
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
        printf("Started provided executable process with pid: %d\n", getpid());
        execv(executable_path, spawn_args);
        perror("Execv failed\n");
        exit(-1);
    }
    else
    {
        /* Parent attaches PAPI to child */

        printf("Attaching to pid %d\n", child_pid);
        if (PAPI_attach(PAPI_eventset, child_pid) != PAPI_OK)
        {
            perror("Could not attach PAPI to process");
            exit(-1);
        }

        /* Start PAPI counters */

        if (PAPI_start(PAPI_eventset) != PAPI_OK)
        {
            perror("could not start PAPI\n");
            exit(-1);
        }

        print_header(nr_counters);

        /* Measure for num_measurements */

        for (size_t i = 0; i < num_measurements; i++)
        {
            usleep(sleep_time);
            
            PAPI_read(PAPI_eventset, values);
            /* Print counter values */
            for(size_t j = 0; j < nr_counters; j++)
            {
                printf("%lld \t\t", values[j]);
                values_storage[i][j] = values[j];
            }
            printf("\n");
            PAPI_reset(PAPI_eventset);
            
            /* check if process still is active */
            if(kill(child_pid, 0) < 0)
            {
                perror("Status check failed");
                break;   
            }
        }

        /* Stop PAPI counters */
        PAPI_stop(PAPI_eventset, values);

        /* Print the averages of collected data */
        print_counter_averages(nr_counters, values_storage, num_measurements);

        /* Write output to file is requested */

        if (write_to_file == 0)
        {
            char file_name[32];
            sprintf(file_name, "%d", child_pid);
            strcat(file_name, outputfile_name);
            printf("Writing measurements to output file %s\n", file_name);
            write_measurements_to_csv_file(nr_counters, values_storage, num_measurements, file_name);
        }

        /* Kill the child process */

        if(kill(child_pid, SIGTERM) != 0)
        {
            perror("Could not terminate process.\n");
            exit(-1);
        }
        printf("Application terminated.\n");
        
        PAPI_shutdown();
    }
    return 0;
}