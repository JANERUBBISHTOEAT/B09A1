#include <stdio.h>
#include <string.h>
#include <stdbool.h>
#include <stdlib.h>

#include <sys/utsname.h>
#include <sys/resource.h>
#include <sys/sysinfo.h>
#include <sys/types.h>
#include <utmp.h>
#include <paths.h>
#include <unistd.h>

void MemoryInfo()
{
    printf("### Memory Usage ###\n");

    struct sysinfo mem_info;
    sysinfo(&mem_info);

    // Current / Total (Physical) Memory Usage
    printf("%5.2f GB / %5.2f GB --- ", (mem_info.totalram - mem_info.freeram) / 1024.0 / 1024.0 / 1024.0, 
            mem_info.totalram / 1024.0 / 1024.0 / 1024.0);
    
    // Current / Total (Physical + Swap) Memory Usage
    printf("%5.2f GB / %5.2f GB\n", (mem_info.totalram - mem_info.freeram + mem_info.totalswap - mem_info.freeswap) / 1024.0 / 1024.0 / 1024.0, 
            (mem_info.totalram + mem_info.totalswap) / 1024.0 / 1024.0 / 1024.0);
}

void CPUInfo()
{
    printf("### CPU Usage ###\n");
    // Template:
    // Number of cores: 4 
    // total cpu use = 0.00%

    // Get total number of cores
    int cores = sysconf(_SC_NPROCESSORS_ONLN);
    printf("Number of cores: %d", cores);

    // Get total CPU usage
    struct rusage usage;
    getrusage(RUSAGE_SELF, &usage);
    printf("total cpu use = %ld", usage.ru_utime.tv_sec + usage.ru_stime.tv_sec);
}

void sysInfo()
{
    printf("### System Usage ###\n");

    struct utsname sys_info;
    uname(&sys_info);
    printf("System Name:\t%s\n", sys_info.sysname);
    printf("Machine Name:\t%s\n", sys_info.nodename);
    printf("Version:\t%s\n", sys_info.version);
    printf("Release:\t%s\n", sys_info.release);
#ifdef _GNU_SOURCE
    printf("Domain Name:\t%s\n", sys_info.domainname);
#endif
    printf("Architecture:\t%s\n", sys_info.machine);
}

// Session/User Info
int userInfo()
{
    printf("### User Usage ###\n");

    struct utmp *user_info_ptr = malloc(sizeof(struct utmp));
    // Get user info
    setutent();

    int user_count = 0;
    while ((user_info_ptr = getutent()) != NULL)
    {
        // Print Session/User Info
        // printf("User:\t%s\t", user_info_ptr->ut_user);
        // printf("PID:\t%d\t", user_info_ptr->ut_pid);
        // printf("Session:\t%d\t", user_info_ptr->ut_session); //%ld
        // printf("Time:\t%d\t", user_info_ptr->ut_tv.tv_sec);
        // printf("Host:\t%s\t", user_info_ptr->ut_host);
        // printf("Line:\t%s\t", user_info_ptr->ut_line);
        // printf("ID:\t%s\t", user_info_ptr->ut_id);
        if (user_info_ptr->ut_type == USER_PROCESS)
        {
            printf("%s\t", user_info_ptr->ut_user);
            printf("%s\t", user_info_ptr->ut_line);
            printf(" (%s)", user_info_ptr->ut_host);
            printf("\n");
            user_count++;
        }
    }

    endutent();
    return user_count;
}


void graph(bool seq, int samples, double tdelay)
{
    // testing args
    printf("seq: %d samples: %d tdelay: %f\n", seq, samples, tdelay);
}

void showOutput(bool sys, bool user, bool graphic, bool seq, int samples, double tdelay)
{
    printf("Samples: %d, Delay: %f\n", samples, tdelay);

    if (!seq)
    {
        for (int i = 0; i < samples; i++)
        {
            if (sys)
                MemoryInfo();
            if (user)
                userInfo();
            if (sys)
                CPUInfo();
            if (graphic)
                graph(seq, samples, tdelay);
            sleep(tdelay);
            printf("\n");
        }
    }
    else
    {
        int user_count = 0;
        for (int i = 0; i < samples; i++)
        {
            printf("ESC[H"); // Go to (0, 0)
            if (sys)
            {
                MemoryInfo();
                printf("\n" * (samples - i));
            }
            if (user)
            {
                user_count = userInfo();
            }
            if (graphic)
            {
                graph(seq, samples, tdelay);
            }
            printf("\n");
        }
        sleep(tdelay);
    }
    if (sys)
        sysInfo();
}

int main(int argc, char *argv[])
{
    char *sys_key = "--system";
    char *user_key = "--user";
    char *graph_key = "--graph";
    char *sys_key_alt = "--sys";
    char *user_key_alt = "--u";
    char *graph_key_alt = "--g";

    char *seq_key = "--sequential";
    char *samples_key = "--samples";
    char *delay_key = "--tdelay";
    char *seq_key_alt = "--seq";

    // No arguments, print usage
    if (argc == 1)
    {
        showOutput(true, true, false, false, 10, 1);
        return 1;
    }

    if ((!strcmp(argv[1], sys_key) || !strcmp(argv[1], sys_key_alt))) // System Usage
    {
        sysInfo();
        return 0;
    }

    if ((!strcmp(argv[1], user_key) || !strcmp(argv[1], user_key_alt))) // User Usage
    {
        userInfo();
        return 0;
    }

    if ((!strcmp(argv[1], graph_key) || !strcmp(argv[1], graph_key_alt))) // Graph
    {
        bool seq = false;
        int samples = 10;
        double tdelay = 1.0;

        if (argc == 2) // No options
        {
            graph(false, samples, tdelay);
            return 0;
        }
        else // argc > 2
        {
            for (int i = 2; i < argc; i++) // Keywords
            {
                if (strstr(argv[i], seq_key) || strstr(argv[i], seq_key_alt))
                {
                    seq = true;
                }
                else if (strstr(argv[i], samples_key))
                {
                    char *token = strtok(argv[i], "=");
                    token = strtok(NULL, "=");
                    // Check if token is an int
                    if (token == NULL)
                    {
                        printf("Invalid argument: %s\n", argv[i]);
                        printf("Usage: %s --graphics (--sequential) (--samples=N(int)) (--tdelay=T)\n", argv[0]);
                        return -1; // Invalid argument
                    }
                    samples = atoi(token);
                    if (samples < 1)
                    {
                        printf("Invalid argument: %s\n", argv[i]);
                        printf("Samples must >= 1\n");
                        printf("Usage: %s --graphics (--sequential) (--samples=N(int>1)) (--tdelay=T)\n", argv[0]);
                        return -1; // Invalid argument
                    }
                }
                else if (strstr(argv[i], delay_key))
                {
                    char *token = strtok(argv[i], "=");
                    token = strtok(NULL, "=");
                    // Check if token is a number
                    if (token == NULL)
                    {
                        printf("Invalid argument: %s\n", argv[i]);
                        printf("Usage: %s --graphics (--sequential) (--samples=N) (--tdelay=T(double))\n", argv[0]);
                        return -1; // Invalid argument
                    }
                    tdelay = atof(token);
                    if (tdelay < 0.1)
                    {
                        printf("Invalid argument: %s\n", argv[i]);
                        printf("Delay must >= 0.1\n");
                        printf("Usage: %s --graphics (--sequential) (--samples=N) (--tdelay=T(double>0.1))\n", argv[0]);
                        return -1; // Invalid argument
                    }
                }
                else
                {
                    printf("Invalid argument: %s\n", argv[i]);
                    printf("Usage: %s --graphics (--sequential) (--samples=N) (--tdelay=T)\n", argv[0]);
                    return -1; // Invalid argument
                }
            }

            graph(seq, samples, tdelay);
        }
        return 0;
    }

    printf("Invalid argument: %s\n", argv[1]);
    printf("Usage: %s [arguments] [options]\n", argv[0]);
    return -1; // Invalid argument
}