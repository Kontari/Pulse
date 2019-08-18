Code that monitors system metrics goes here.

Organization
------------
The code is organized into a C monitor, which scans multiple system metrics from `/proc/`, as well as a Python extension module that implements the same functions as the C monitor (`monitor_module.c`).

Monitor Module Installation
---------------------------
In order to install the monitor module, make sure to have `monitor_module.c` and `setup.py` in the same directory. Then, having navigated to that directory, run:

`sudo python setup.py install`

Afterwards, you will have the module available for use under the name `monitor`.

Module Methods
--------------
The monitor module currently contains four methods, each of which returns a different system metric. They are as follows:

`get_meminfo(void)` - Returns a Python tuple that contains, in kilobytes, the system's total memory, free memory, and available memory in that order.

`get_disk_io(void)` - Returns the current number of disk I/Os in an integer.

`get_process_count(void)` - Returns the current number of running processes, as read from `/proc/`.

`get_loadavg(void)` - Returns a Python tuple that contains the CPU load averages for 1, 5, and 15 minutes in that order.