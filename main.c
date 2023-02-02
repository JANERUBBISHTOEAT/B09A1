#include <stdio.h>
#include <string.h>
#include <stdbool.h>
#include <stdlib.h>
#include <ctype.h>

#include <sys/utsname.h>
#include <sys/resource.h>
#include <sys/sysinfo.h>
#include <sys/types.h>
#include <sys/time.h>
#include <utmp.h>
#include <unistd.h>
#include <paths.h>
// #include <time.h>

#define _MIN(a, b) (((a) < (b)) ? (a) : (b))
#define _MAX(a, b) (((a) > (b)) ? (a) : (b))

void MemoryInfo(bool graphic, double tdelay, double *last_Mem)
{
    // printf("### Memory Usage ###\n");

    struct sysinfo mem_info;
    sysinfo(&mem_info);

    // Current / Total (Physical) Memory Usage
    double mem_phy_used = (double)(mem_info.totalram - mem_info.freeram) / 1024.0 / 1024.0 / 1024.0;
    double mem_phy_total = (double)(mem_info.totalram) / 1024.0 / 1024.0 / 1024.0;
    // printf("%5.2f GB / %5.2f GB --- ", (mem_info.totalram - mem_info.freeram) / 1024.0 / 1024.0 / 1024.0,
    //        mem_info.totalram / 1024.0 / 1024.0 / 1024.0);

    // Current / Total (Physical + Swap) Memory Usage
    double mem_vir_used = (double)(mem_info.totalram - mem_info.freeram + mem_info.totalswap - mem_info.freeswap) / 1024.0 / 1024.0 / 1024.0;
    double mem_vir_total = (double)(mem_info.totalram + mem_info.totalswap) / 1024.0 / 1024.0 / 1024.0;
    // printf("%5.2f GB / %5.2f GB\n", (mem_info.totalram - mem_info.freeram + mem_info.totalswap - mem_info.freeswap) / 1024.0 / 1024.0 / 1024.0,
    //        (mem_info.totalram + mem_info.totalswap) / 1024.0 / 1024.0 / 1024.0);

    if (*last_Mem == 0.0) // First time running the function
    {
        *(last_Mem) = (double)(mem_phy_used);
        return;
    }

    if (graphic)
    {
        double change_rate = (mem_phy_used - *last_Mem) / *last_Mem;
        int bar_length = 100;
        int bar_length_change = abs((int)(change_rate * bar_length));
        char bar_char_pos = '#';
        char bar_char_neg = ':';
        char bar_char = (change_rate > 0) ? bar_char_pos : bar_char_neg;

        printf("%5.2f GB / %5.2f GB --- ", mem_phy_used, mem_phy_total);
        printf("%5.2f GB / %5.2f GB\t|", mem_vir_used, mem_vir_total);
        // printf("leng: %lf - %d", change_rate, bar_length_change);

        if (bar_length_change == 0)
        {
            if (change_rate > 0)
                bar_char = 'o';
            else
                bar_char = '@';
            putchar(bar_char);
        }
        for (int i = 0; i < bar_length_change; i++)
        {
            putchar(bar_char);
        }
        if (bar_length_change != 0)
            (bar_length_change > 0) ? (putchar('*')) : (putchar('@'));
        printf("| %5.4f%%\n", change_rate * 100.0);

        *last_Mem = mem_phy_used;
    }
    else
    {
        printf("%5.2f GB / %5.2f GB --- ", mem_phy_used, mem_phy_total);
        printf("%5.2f GB / %5.2f GB\n", mem_vir_used, mem_vir_total);
    }
}

void CPUInfo(double tdelay, int *last_CPU_time_ptr, bool graphic, double *last_CPU)
{
    // printf("### CPU Usage ###\n");

    // Get total number of cores
    int cores = sysconf(_SC_NPROCESSORS_ONLN);

    // Get CPU usage
    FILE *f = fopen("/proc/stat", "r");
    int tmp;
    int system_time, user_time, idle_time;
    fscanf(f, "cpu %d %d %d %d", &user_time, &tmp, &system_time, &idle_time);
    fclose(f);
    if (last_CPU_time_ptr[0] == 0 && last_CPU_time_ptr[1] == 0 && last_CPU_time_ptr[2] == 0)
    {
        last_CPU_time_ptr[0] = system_time;
        last_CPU_time_ptr[1] = user_time;
        last_CPU_time_ptr[2] = idle_time;
        return;
    }
    printf("Number of cores: %d\n", cores);
    printf("sys=%d\tuser=%d\tidle=%d\n", system_time, user_time, idle_time);
    printf("last sys=%d\tlast user=%d\tlast idle=%d\n", last_CPU_time_ptr[0], last_CPU_time_ptr[1], last_CPU_time_ptr[2]);
    printf("delta sys=%d\tdelta user=%d\tdelta idle=%d\n", system_time - last_CPU_time_ptr[0], user_time - last_CPU_time_ptr[1], idle_time - last_CPU_time_ptr[2]);
    double current_cpu = (double)(system_time - last_CPU_time_ptr[0] + user_time - last_CPU_time_ptr[1]) /
                         (double)(system_time - last_CPU_time_ptr[0] + user_time - last_CPU_time_ptr[1] + idle_time - last_CPU_time_ptr[2]) * 100.0;
    printf("total cpu use = %5.4f%%\n\n", current_cpu);
    last_CPU_time_ptr[0] = system_time;
    last_CPU_time_ptr[1] = user_time;
    last_CPU_time_ptr[2] = idle_time;

    if (graphic)
    {
        double change_rate = (current_cpu - *last_CPU) / *last_CPU * 100.0;
        int bar_length = 10;
        int bar_length_change = abs(change_rate / 100.0 * bar_length);
        char bar_char_pos = '|';
        char bar_char_neg = '-';
        char bar_char = (change_rate > 0) ? bar_char_pos : bar_char_neg;
        printf("change: %lf - %lf\n", current_cpu, *last_CPU);
        printf("change rate: %d\n", bar_length_change);
        for (int i = 0; i < _MIN(bar_length_change, bar_length); i++)
        {
            putchar(bar_char);
        }
        for (int i = 0; i < bar_length - bar_length_change; i++)
        {
            putchar(' ');
        }
        printf(" %5.4f%%\n", change_rate);
    }
    *last_CPU = current_cpu;
}

void sysInfo()
{
    printf("\n### System Usage ###\n");

    struct utsname sys_info;
    uname(&sys_info);
    printf("System Name:\t%s\n", sys_info.sysname);
    printf("Machine Name:\t%s\n", sys_info.nodename);
    printf("Version:\t%s\n", sys_info.version);
    printf("Release:\t%s\n", sys_info.release);
    printf("Architecture:\t%s\n", sys_info.machine);
}

// Session/User Info
int userInfo(bool show_all)
{
    // printf("### User Usage ###\n");

    struct utmp *user_info_ptr = malloc(sizeof(struct utmp));
    // Get user info
    setutent();

    int user_count = 0;
    int user_count_max = 5;
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
            if (user_count <= user_count_max || show_all)
            {
                printf("%s\t", user_info_ptr->ut_user);
                printf("%s\t", user_info_ptr->ut_line);
                printf(" (%s)", user_info_ptr->ut_host);
                printf("\n");
            }
            user_count++;
        }
    }
    if (user_count > user_count_max && !show_all)
    {
        printf("... Showing %d of %d users, use --all to show all users\n", user_count_max, user_count);
    }

    endutent();
    return user_count;
}

void showOutput(bool sys, bool user, bool graphic, bool seq, int samples, double tdelay, bool show_all)
{
    printf("Samples: %d, Delay: %.2f\n", samples, tdelay);

    // Get first CPU time
    int last_CPU_time[3] = {0, 0, 0};
    int *last_CPU_time_ptr = last_CPU_time;
    double last_CPU = 0;
    CPUInfo(tdelay, last_CPU_time_ptr, graphic, &last_CPU);

    // Get first memory info
    double last_Mem = 0;
    MemoryInfo(graphic, tdelay, &last_Mem);

    if (seq)
    {
        for (int i = 0; i < samples; i++)
        {
            if (sys)
                printf("\n### Memory Usage ### (Phys.Used/Tot -- Virtual Used/Tot)\n"), MemoryInfo(graphic, tdelay, &last_Mem);
            if (user)
                printf("\n### Session/Users ###\n"), userInfo(show_all);
            if (sys)
                printf("\n### CPU Usage ###\n"), CPUInfo(tdelay, last_CPU_time_ptr, graphic, &last_CPU);
            printf("\n");
            sleep((int)tdelay);
            // usleep(tdelay * 1000000);
        }
    }
    else
    {
        printf("\033[2J"); // Clear screen

        for (int i = 0; i < samples; i++)
        {
            printf("\033[H"); // Go to (0, 0)
            if (sys)
            {
                printf("\n### Memory Usage ### (Phys.Used/Tot -- Virtual Used/Tot)\n");
                printf("\033[%dB", i + 1); // Go down (i + 1) lines (Already printed i lines)
                MemoryInfo(graphic, tdelay, &last_Mem);
                for (int j = 0; j < samples - i - 1; j++)
                    printf("\n");
            }
            if (user)
            {
                printf("\n### Session/Users ###\n");
                userInfo(show_all);
            }
            if (sys)
            {
                printf("\n### CPU Usage ###\n");
                CPUInfo(tdelay, last_CPU_time_ptr, graphic, &last_CPU);
            }
            printf("\n");
            sleep((int)tdelay);
            // usleep(tdelay * 1000000);
        }
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
    char *all_user_key = "--all";

    char *seq_key = "--sequential";
    char *samples_key = "--samples";
    char *delay_key = "--tdelay";
    char *seq_key_alt = "--seq";

    int samples = 10;
    double tdelay = 1;
    bool seq = false;
    bool sys = true;
    bool user = true;
    bool graphic = false;
    bool show_all = false;

    bool samples_set = false;
    bool tdelay_set = false;

    // No arguments, print usage
    if (argc == 1)
    {
        showOutput(sys, user, graphic, seq, samples, tdelay, show_all);
        return 1;
    }
    else
    {
        for (int i = 1; i < argc; i++)
        {
            if ((!strcmp(argv[i], sys_key) || !strcmp(argv[i], sys_key_alt)))
                sys = true, user = false;
            else if ((!strcmp(argv[i], user_key) || !strcmp(argv[i], user_key_alt)))
                user = true, sys = false;
            else if ((!strcmp(argv[i], graph_key) || !strcmp(argv[i], graph_key_alt)))
                graphic = true;
            else if ((!strcmp(argv[i], seq_key) || !strcmp(argv[i], seq_key_alt)))
                seq = true;
            else if ((!strcmp(argv[i], all_user_key)))
                show_all = true;
            else if (strstr(argv[i], samples_key))
            {
                char *token = strtok(argv[i], "=");
                token = strtok(NULL, "=");
                // Check if token is an int
                if (token == NULL)
                {
                    printf("Invalid argument: %s\n", argv[i]);
                    printf("Usage: \"%s --graphics (--sequential) (--samples=N(int)) (--tdelay=T)\n\"", argv[0]);
                    printf("Samples set to default of 10");
                    // samples = 10;
                }
                samples = atoi(token);
                if (samples < 1)
                {
                    printf("Invalid argument: %s\n", argv[i]);
                    printf("Samples must >= 1\n");
                    printf("Usage: \"%s --graphics (--sequential) (--samples=N(int>1)) (--tdelay=T)\n\"", argv[0]);
                    printf("Samples set to minimum of 1");
                    samples = 1;
                }
                samples_set = true;
            }
            else if (strstr(argv[i], delay_key))
            {
                char *token = strtok(argv[i], "=");
                token = strtok(NULL, "=");
                // Check if token is a number
                if (token == NULL)
                {
                    printf("Invalid argument: %s\n", argv[i]);
                    printf("Usage: \"%s --graphics (--sequential) (--samples=N) (--tdelay=T(double))\"\n", argv[0]);
                    printf("Delay set to default of 1\n");
                    // tdelay = 1;
                }
                tdelay = atof(token);
                if (tdelay < 1)
                {
                    printf("Invalid argument: %s\n", argv[i]);
                    printf("Delay must >= 0.1\n");
                    printf("Usage: \"%s --graphics (--sequential) (--samples=N) (--tdelay=T(double>1))\"\n", argv[0]);
                    printf("Delay set to minimum of 1\n");
                    tdelay = 1;
                }
                tdelay_set = true;
            }

            // Positional arguments
            else if (isdigit(argv[i][0])) // Samples or Delay
            {
                if (samples_set == false)
                {
                    samples = atoi(argv[i]);
                    if (samples < 1)
                    {
                        printf("Invalid number of samples. Sample set to minimum of 1.\n");
                        samples = 1;
                    }
                    samples_set = true;
                }
                else if (tdelay_set == false)
                {
                    tdelay = atof(argv[i]);
                    if (tdelay < 1)
                    {
                        printf("Invalid delay. Delay set to minimum of 1.\n");
                        tdelay = 1;
                    }
                    tdelay_set = true;
                }
                else
                {
                    printf("Invalid *positional* argument: %s\n", argv[i]);
                    printf("Usage: \"%s pos_arg_1 pos_arg_2\"\n", argv[0]);
                    printf("Samples set to default of 10\n");
                    printf("Delay set to default of 1\n");
                    // samples = 10;
                    // tdelay = 1;
                }
            }
            // else if (argv[i][0] == '.' && !tdelay_set) // Tdelay is float num
            // {
            //     tdelay = atof(argv[i]);
            //     if (tdelay < 0.1)
            //     {
            //         printf("Invalid delay. Delay set to minimum of 0.1.\n");
            //         tdelay = 0.1;
            //     }
            //     tdelay_set = true;
            // }
            else
            {
                printf("Invalid argument: %s\n", argv[i]);
                printf("Usage: \"%s --graphics (--sequential) (--samples=N) (--tdelay=T)\n\"", argv[0]);
                printf("Can't help you set them to default this time :( I totally didn't understand which ones you wanted to change.\n");
            }
        }
    }

    showOutput(sys, user, graphic, seq, samples, tdelay, show_all);
    return 0;
}