#define PY_SSIZE_T_CLEAN
#include <python3.7m/Python.h>  // includes favorites such as stdio, stdlib, and string
#include <unistd.h>
#include <dirent.h>
#include <sys/stat.h>
#include <sys/types.h>

/* Standard error function because I'm lazy */
void frick(char *msg) {
    fputs(msg, stderr);
    exit(-1);
}

/*This guy returns information on the system's total, free, and available memory,
in kilobytes, from /proc/meminfo in a Python tuple, and in that order */
static PyObject *get_meminfo(PyObject *self) {
    unsigned int memtotal, memfree, memavailable;
    FILE *fp = fopen("/proc/meminfo", "r");

    fscanf(fp, "%*s %d %*s %*s %d %*s %*s %d", &memtotal, &memfree, &memavailable);
    fclose(fp);
    //printf("%d %d %d", meminfo.memtotal, meminfo.memfree, meminfo.memavailable);
    return Py_BuildValue("iii", memtotal, memfree, memavailable);
}

/* This guy returns the major and minor number of the device
on which root is mounted, in hexadecimal. The dev_t type is an
unsigned long int that thinks it's special. */
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
static PyObject *get_disk_io(PyObject *self) {
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
            return Py_BuildValue("i", io);
        }
    }

    fclose(fp);
    fclose(null);
    frick("Your root filesystem doesn't, uh, exist");
}

/* Self explanatory function; counts processes in /proc */
static PyObject *get_process_count(PyObject *self) {
    int process_count = 0;
    struct dirent *open;
    DIR *dirp;
    if((dirp = opendir("/proc")) < 0) frick("Failed to open /proc while reading process count");

    /* Match directory names to numbers in order to count processes. This is probably a terrible way to do it but it's system-agnostic. */
    while((open = readdir(dirp)) != NULL) if(open->d_type == DT_DIR && (strncmp(open->d_name, "0", 1) == 0 || strncmp(open->d_name, "1", 1) == 0 || 
        strncmp(open->d_name, "2", 1) == 0 || strncmp(open->d_name, "3", 1) == 0 || strncmp(open->d_name, "4", 1) == 0 || strncmp(open->d_name, "5", 1) == 0 ||
        strncmp(open->d_name, "6", 1) == 0 || strncmp(open->d_name, "7", 1) == 0 || strncmp(open->d_name, "8", 1) == 0 || strncmp(open->d_name, "9", 1) == 0)) process_count++;

    return Py_BuildValue("i", process_count);
}

/* This guy returns the first three values of /proc/loadavg
in a handy-dandy Python tuple. They represent the number of jobs
in the run queue or waiting for disk I/O averaged over 1, 5,
and 15 minutes, respectively. */
static PyObject *get_loadavg(PyObject *self) {
    float min1_avg, min5_avg, min15_avg;
    FILE *fp = fopen("/proc/loadavg", "r");

    fscanf(fp, "%f %f %f", &min1_avg, &min5_avg, &min15_avg);
    fclose(fp);
    return Py_BuildValue("fff", min1_avg, min5_avg, min15_avg);
}

/* An array of functions in the monitor module */
static PyMethodDef monitor_funcs[] = {
    {"get_meminfo", (PyCFunction)get_meminfo, METH_NOARGS, "Returns free, total, and available memory"},
    {"get_disk_io", (PyCFunction)get_disk_io, METH_NOARGS, "Returns current number of disk I/Os"},
    {"get_process_count", (PyCFunction)get_process_count, METH_NOARGS, "Returns the current number of processes from /proc"},
    {"get_loadavg", (PyCFunction)get_loadavg, METH_NOARGS, "Returns 1, 5, and 15 minute CPU load averages"},
    {NULL, NULL, 0, NULL}   // Necessary to terminate the method mapping table
};

/* Module definition structure */
static struct PyModuleDef monitor_module = {
    PyModuleDef_HEAD_INIT,
    "monitor",
    NULL,
    -1,
    monitor_funcs
};

/* Module initialization */
PyMODINIT_FUNC PyInit_monitor(void) {
    return PyModule_Create(&monitor_module);
}