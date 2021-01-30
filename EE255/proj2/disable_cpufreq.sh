CPU_PATH=/sys/devices/system/cpu
for cpu in 0 1 2 3; do echo performance > $CPU_PATH/cpu$cpu/cpufreq/scaling_governor; done
