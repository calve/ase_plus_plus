This is the home of a small module for the Linux kernel.

Build it with ``make``

Start your vm with ``./kvm-term.sh``

Then push your module with ``scp -P 10022 tracktime.ko ase@127.0.0.1:/home/ase``

# Usage

```
root@debian-ase:/home/ase# insmod tracktime.ko
[ 1546.789258] Ase initialized
root@debian-ase:/home/ase# ps
PID TTY          TIME CMD
1711 ttyS0    00:00:00 login
1936 ttyS0    00:00:01 bash
2088 ttyS0    00:00:00 ps
root@debian-ase:/home/ase# ls /proc/ase
1936
root@debian-ase:/home/ase# echo -n 1936 > /proc/ase_cmd
[ 1548.887572] ASECMD: Write has been called
[ 1548.889199] ASECMD: Entering the add_pid_action function.
[ 1548.895770] ASECMD: PID find and added in the directory
[ 1548.897725] ASECMD: /proc/ase/1936 initialized at 4296216194

Message from syslogd@debian-ase at Apr 20 10:23:28 ...
kernel:[ 1548.889199] ASECMD: Entering the add_pid_action function.

Message from syslogd@debian-ase at Apr 20 10:23:28 ...
kernel:[ 1548.895770] ASECMD: PID find and added in the directory

Message from syslogd@debian-ase at Apr 20 10:23:28 ...
kernel:[ 1548.897725] ASECMD: /proc/ase/1936 initialized at 4296216194
root@debian-ase:/home/ase# cat /proc/ase/1936
PID : 1936, jiffies 4235
root@debian-ase:/home/ase# cat /proc/ase/1936
PID : 1936, jiffies 5022
root@debian-ase:/home/ase# cat /proc/ase/1936
PID : 1936, jiffies 5658
root@debian-ase:/home/ase# cat /proc/ase/1936
PID : 1936, jiffies 6231
root@debian-ase:/home/ase# cat /proc/ase/1936
PID : 1936, jiffies 6783
root@debian-ase:/home/ase# cat /proc/ase/1936
PID : 1936, jiffies 7351
root@debian-ase:/home/ase# cat /proc/ase/1936
PID : 1936, jiffies 7929
root@debian-ase:/home/ase# cat /proc/ase/1936
PID : 1936, jiffies 8496
```
