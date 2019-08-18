#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <dirent.h>
#include <string.h>
#include <ncurses.h>
#include <sys/stat.h>
#include <sys/types.h>

/* Handy-dandy structure containing CPU load averages for
1, 5, and 15 minutes */
struct loadavgs {
    float min1_avg;
    float min5_avg;
    float min15_avg;
};

/* Handy-dandy structure containing info on total, free, and
available memory in kilobytes */
struct meminfo {
    unsigned int memtotal;
    unsigned int memfree;
    unsigned int memavailable;
};

/* Standard error function because I'm lazy */
void frick(char *msg) {
    fputs(msg, stderr);
    exit(-1);
}

/*This guy returns information on the system's total, free, and available memory
from /proc/meminfo in a handy-dandy structure */
struct meminfo get_meminfo(void) {
    struct meminfo meminfo;
    FILE *fp = fopen("/proc/meminfo", "r");

    fscanf(fp, "%*s %d %*s %*s %d %*s %*s %d", &meminfo.memtotal, &meminfo.memfree, &meminfo.memavailable);
    fclose(fp);
    return meminfo;
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

/* Self explanatory function; counts processes in /proc */
int get_process_count(void) {
    int process_count = 0;
    struct dirent *open;
    DIR *dirp;
    if((dirp = opendir("/proc")) < 0) frick("Failed to open /proc while reading process count");

    /* Match directory names to numbers in order to count processes. This is probably a terrible way to do it but it's system-agnostic. */
    while((open = readdir(dirp)) != NULL) if(open->d_type == DT_DIR && (strncmp(open->d_name, "0", 1) == 0 || strncmp(open->d_name, "1", 1) == 0 || 
        strncmp(open->d_name, "2", 1) == 0 || strncmp(open->d_name, "3", 1) == 0 || strncmp(open->d_name, "4", 1) == 0 || strncmp(open->d_name, "5", 1) == 0 ||
        strncmp(open->d_name, "6", 1) == 0 || strncmp(open->d_name, "7", 1) == 0 || strncmp(open->d_name, "8", 1) == 0 || strncmp(open->d_name, "9", 1) == 0)) process_count++;

    return process_count;
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
    initscr();  // Begin curses mode
    struct loadavgs loadavgs;
    struct meminfo meminfo;
    for(;;) {
        clear();
        loadavgs = get_loadavg();
        meminfo = get_meminfo();
        printw("Load averages:\t%.2f, %.2f, %.2f\n", loadavgs.min1_avg, loadavgs.min5_avg, loadavgs.min15_avg);
        printw("Memory Info:\t%d Mb Total, %d Mb Free, %d Mb Available\n", meminfo.memtotal/1000, meminfo.memfree/1000, meminfo.memavailable/1000);
        printw("Disk I/O:\t%d\n", get_disk_io());
        printw("Process count:\t%d\n", get_process_count());
        refresh();
        sleep(3);   // Wait 3 seconds before refreshing values
    }

    /* Considering that these will never run, this program probably has a memory leak */
    endwin();   // Free the memory allocated by ncurses
    exit(0);
}