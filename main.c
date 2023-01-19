#include <stdio.h>
#include <string.h>
#include <stdbool.h>
#include <stdlib.h>

void sysInfo()
{
    printf("System Usage");
}

void userInfo()
{
    printf("User Usage");
}

void graph(bool seq, int samples, double tdelay)
{
    // testing args
    printf("seq: %d samples: %d tdelay: %f", seq, samples, tdelay);
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