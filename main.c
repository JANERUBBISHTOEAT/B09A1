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

void sysInfo()
{
    printf("### System Usage ###\n");

    //     struct utsname
    //     {
    //         char sysname[];  /* Operating system name (e.g., "Linux") */
    //         char nodename[]; /* Name within "some implementation-defined
    //                             network" */
    //         char release[];  /* Operating system release
    //                             (e.g., "2.6.28") */
    //         char version[];  /* Operating system version */
    //         char machine[];  /* Hardware identifier */
    // #ifdef _GNU_SOURCE
    //         char domainname[]; /* NIS or YP domain name */
    // #endif
    //     };

    struct utsname sys_info;
    uname(&sys_info);
    printf("Success getting system info!\n\n");
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
void userInfo()
{
    printf("### User Usage ###\n");

    // struct exit_status {              /* Type for ut_exit, below */
    //     short e_termination;          /* Process termination status */
    //     short e_exit;                 /* Process exit status */
    // };

    // struct utmp {
    //     short   ut_type;              /* Type of record */
    //     pid_t   ut_pid;               /* PID of login process */
    //     char    ut_line[UT_LINESIZE]; /* Device name of tty - "/dev/" */
    //     char    ut_id[4];             /* Terminal name suffix,
    //                                     or inittab(5) ID */
    //     char    ut_user[UT_NAMESIZE]; /* Username */
    //     char    ut_host[UT_HOSTSIZE]; /* Hostname for remote login, or
    //                                     kernel version for run-level
    //                                     messages */
    //     struct  exit_status ut_exit;  /* Exit status of a process
    //                                     marked as DEAD_PROCESS; not
    //                                     used by Linux init(1) */
    //     /* The ut_session and ut_tv fields must be the same size when
    //         compiled 32- and 64-bit.  This allows data files and shared
    //         memory to be shared between 32- and 64-bit applications. */
    // #if __WORDSIZE == 64 && defined __WORDSIZE_COMPAT32
    //     int32_t ut_session;           /* Session ID (getsid(2)),
    //                                     used for windowing */
    //     struct {
    //         int32_t tv_sec;           /* Seconds */
    //         int32_t tv_usec;          /* Microseconds */
    //     } ut_tv;                      /* Time entry was made */
    // #else
    //     long   ut_session;           /* Session ID */
    //     struct timeval ut_tv;        /* Time entry was made */
    // #endif

    //     int32_t ut_addr_v6[4];        /* Internet address of remote
    //                                     host; IPv4 address uses
    //                                     just ut_addr_v6[0] */
    //     char __unused[20];            /* Reserved for future use */
    // };

    struct utmp *user_info_ptr = malloc(sizeof(struct utmp));
    // Get user info
    setutent();
    user_info_ptr = getutent();
    printf("Trying to get user info...\n");
    printf("User:\t%s\t", user_info_ptr->ut_user);
    printf("PID:\t%d\t", user_info_ptr->ut_pid);
    printf("Session:\t%d\t", user_info_ptr->ut_session); //%ld

    printf("\nSuccess getting user info!\n\n");

    while ((user_info_ptr = getutent()) != NULL)
    {
        // Print Session/User Info
        printf("User:\t%s\t", user_info_ptr->ut_user);
        printf("PID:\t%d\t", user_info_ptr->ut_pid);
        printf("Session:\t%d\t", user_info_ptr->ut_session); //%ld
        printf("Time:\t%d\t", user_info_ptr->ut_tv.tv_sec);
        printf("Host:\t%s\t", user_info_ptr->ut_host);
        printf("Line:\t%s\t", user_info_ptr->ut_line);
        printf("ID:\t%s\t", user_info_ptr->ut_id);

        printf("\n");
    }

    endutent();
}

void graph(bool seq, int samples, double tdelay)
{
    // testing args
    printf("seq: %d samples: %d tdelay: %f\n", seq, samples, tdelay);
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
        printf("Usage: %s [arguments] [options]\n", argv[0]);
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

            // Positional arguments: samples, delay
            // ...

            graph(seq, samples, tdelay);
        }
        return 0;
    }

    printf("Invalid argument: %s\n", argv[1]);
    printf("Usage: %s [arguments] [options]\n", argv[0]);
    return -1; // Invalid argument
}