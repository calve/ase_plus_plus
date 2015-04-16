This is the home of a small module for the Linux kernel.

Build it with ``make``

Start your vm with ``./kvm-term.sh``

Then push your module with ``scp -P 10022 tracktime.ko ase@127.0.0.1:/home/ase``

# Usage

```
root@debian-ase:/home/ase# insmod tracktime.ko
[ 4719.935481] Ase initialized
root@debian-ase:/home/ase# ps
PID TTY          TIME CMD
1711 ttyS0    00:00:00 login
1935 ttyS0    00:00:01 bash
2088 ttyS0    00:00:00 ps
root@debian-ase:/home/ase# echo 1935 > /proc/ase_cmd
[ 4753.358606] ASECMD: Write has been called
[ 4753.362227] ASECMD: Entering the add_pid_action function.
[ 4753.366314] ASECMD: PID find and added in the directory
root@debian-ase:/home/ase#
Message from syslogd@debian-ase at Apr 17 01:17:09 ...
kernel:[ 4753.362227] ASECMD: Entering the add_pid_action function.

Message from syslogd@debian-ase at Apr 17 01:17:09 ...
kernel:[ 4753.366314] ASECMD: PID find and added in the directory

root@debian-ase:/home/ase# ls /proc/ase
1935
root@debian-ase:/home/ase# cat /proc/ase/1935
PID : 1935

```
