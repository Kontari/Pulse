#include <stdio.h>
#include <stdlib.h>
#include <dirent.h>
#include <string.h>
#include <sys/stat.h>
#include <sys/types.h>

/* Handy-dandy structure containing CPU load averages for
1, 5, and 15 minutes */
struct loadavgs {
    float min1_avg;
    float min5_avg;
    float min15_avg;
};

/* Standard error function because I'm lazy */
void frick(char *msg) {
    fputs(msg, stderr);
    exit(-1);
}

/* This guy returns the major and minor number of the device
on which root is mounted, in hexadecimal. The dev_t type is a 
short that thinks it's special. */
dev_t get_file_devid(char *path) {
    struct stat filestat;
    char hex_str[8];    // For formatting
    if((stat(path, &filestat)) < 0) frick("Failed to stat() somewhere, probably on root");
    
    /* Format magic ;) */
    sprintf(hex_str, "%x", filestat.st_dev);
    sscanf(hex_str, "%d", &filestat.st_dev);
    return filestat.st_dev;
}

/* Converts major and minor numbers in the form of majd\tmind to
the standard dev_t format. */
dev_t dev_t_conversion(char *dec) {
    dev_t converted;
    short major;
    short minor;
    char hex_str[8];    // For formatting

    /* Format magic ;) assumes fewer than 10 partitions */
    sscanf(dec, "%d %d", &major, &minor);
    sprintf(hex_str, "%x0%x", major, minor);
    sscanf(hex_str, "%d", &converted);
    return converted;
}

int get_process_count(void) {
    int process_count = 0;
    struct dirent *open;
    DIR *dirp;

    if((dirp = opendir("/proc")) < 0) frick("Failed to open /proc while reading process count");
    while((open = readdir(dirp)) != NULL) if(open->d_type == DT_DIR && strcmp(open->d_name, ".") != 0 && strcmp(open->d_name, "..") != 0) process_count++;
    return (process_count - 11);    // There are 11 non-process directories in /proc
}

/* This guy reads the current disk I/O from /proc/diskstats */
int get_disk_io(void) {
    char line[128];
    int io;
    FILE *fp = fopen("/proc/diskstats", "r");
    FILE *null = fopen("/dev/null", "w");

    while(fgets(line, sizeof line, fp)) {
        fputs(line, null);  // Conditional won't trip unless line is written ¯\_(ಥ‿ಥ)_/¯
        if(dev_t_conversion(line) == get_file_devid("/")) {
            sscanf(line, "%*d %*d %*s %*d %*d %*d %*d %*d %*d %*d %*d %d", &io);   // Gotta love format specifiers
            fclose(fp);
            fclose(null);
            return io;
        }
    }

    fclose(fp);
    fclose(null);
    frick("Your root filesystem doesn't, uh, exist");
}

/* This guy returns the first three values of /proc/loadavg
in a handy-dandy structure. They represent the number of jobs
in the run queue or waiting for disk I/O averaged over 1, 5,
and 15 minutes, respectively. */
struct loadavgs get_loadavg(void) {
    struct loadavgs loadavgs;
    FILE *fp = fopen("/proc/loadavg", "r");

    fscanf(fp, "%f %f %f", &loadavgs.min1_avg, &loadavgs.min5_avg, &loadavgs.min15_avg);
    fclose(fp);
    return loadavgs;
}

int main(void) {
    struct loadavgs loadavgs = get_loadavg();
    printf("Load averages:\t%.2f %.2f %.2f\n", loadavgs.min1_avg, loadavgs.min5_avg, loadavgs.min15_avg);
    printf("Disk I/O:\t%d\n", get_disk_io());
    printf("Process count:\t%d\n", get_process_count());
    exit(0);
}