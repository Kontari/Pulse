#Import our C package to monitor system functions
import monitor
import time
system = monitor
#Run infinite loop that checks system info every 5 seconds
while True:
    #Split the tuples into three separate variables
    total, free, avail = system.get_meminfo()
    one_min, five_min, fifteen_min = system.get_loadavg()

    print("Total Memory: ", total)
    print("Free Memory: ", free)
    print("Available Memory: ", avail)
    #FIX LATER: print("Number of Disk I/Os: ",  system.get_disk_io())
    print("Number of Processes: ", system.get_process_count())
    print("One Minute Load Average: ", one_min)
    print("Five Minute Load Average: ", five_min)
    print("Fifteen Minute Load Average: ", fifteen_min, "\n")
    time.sleep(5)