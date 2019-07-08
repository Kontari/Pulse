import os  # Import the os module to fork processes (The Fork Bomb)
import psutil  # Import the psutil module to monitor the cpu/virtual ram usage

# psutil needs to be manually installed via the command:
#   python -m pip install psutil


def check(params):  # Checks either cpu, virtual memory, both, or either
    if params == "mem":  # Checks virtual memory only
        if psutil.virtual_memory().percent > 95:  # If percentage use of virtual ram is above 95%
            for proc in psutil.process_iter():  # for all running processes
                if proc.name() == 'python.exe':  # if its a python process
                    proc.kill()  # kill it
            return True  # return the True to end the program
        else:
            return False  # if its not above 95% return false to keep going

    elif params == "cpu":  # check cpu only
        if psutil.cpu_percent() > 95:  # if usage is above 95%
            for proc in psutil.process_iter():
                if proc.name() == 'python.exe':
                    proc.kill()
            return True
        else:
            return False

    elif params == "either":  # checks if either cpu or virtual memory is above 95%
        if psutil.cpu_percent() > 95 or psutil.virtual_memory().percent > 95:
            for proc in psutil.process_iter():
                if proc.name() == 'python.exe':
                    proc.kill()
            return True
        else:
            return False

    elif params == "both":  # Checks if both cpu and virtual memory are above 95%
        if psutil.cpu_percent() > 95 and psutil.virtual_memory().percent > 95:
            for proc in psutil.process_iter():
                if proc.name() == 'python.exe':
                    proc.kill()
            return True
        else:
            return False

    else:  # If things broke or input was put incorrectly
        print("Sorry that input didn't work right.")
        exit()  # Exit the program


def main():
    # Get use input for what kind of monitoring is wanted
    get_params = input("Type [mem] to check virtual memory or [cpu] for cpu or [either] for either or [both] for both.")
    while True:  # Main loop
        os.fork()  # Make a fork
        if check(get_params) == True:  # If the check function returns True (above 95% and processes killed)
            exit()  # end the program


if __name__ in "__main__":
    main()

