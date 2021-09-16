# krhook

系统调用hook

## 使用说明

```shell
insmod krhook.so

# 输入需要过滤的pid, -1 代表不过滤
echo 8600 > /dev/mypid

# 验证是否成功
cat /dev/mypid

# dmesg 可以查看对应log

OnePlus7T:/data/local/tmp # dmesg | tail
[85016.474909] [20210916_17:10:02.294775]@0 myLog::openat64 pathname:[/sys/class/power_supply/battery/health] current->pid:[860]
[85016.475085] [20210916_17:10:02.294953]@0 myLog::openat64 pathname:[/sys/class/power_supply/battery/technology] current->pid:[860]
[85016.475219] [20210916_17:10:02.295087]@0 myLog::openat64 pathname:[/sys/class/power_supply/dc/online] current->pid:[860]
[85016.475382] [20210916_17:10:02.295250]@0 myLog::openat64 pathname:[/sys/class/power_supply/pc_port/online] current->pid:[860]
[85016.475522] [20210916_17:10:02.295390]@0 myLog::openat64 pathname:[/sys/class/power_supply/pc_port/type] current->pid:[860]
[85016.475708] [20210916_17:10:02.295576]@0 myLog::openat64 pathname:[/sys/class/power_supply/pc_port/current_max] current->pid:[860]
[85016.475902] [20210916_17:10:02.295770]@0 myLog::openat64 pathname:[/sys/class/power_supply/pc_port/voltage_max] current->pid:[860]
[85016.476016] [20210916_17:10:02.295884]@0 myLog::openat64 pathname:[/sys/class/power_supply/usb/online] current->pid:[860]
[85016.476265] [20210916_17:10:02.296133]@0 healthd: battery l=100 v=4396 t=27.9 h=2 st=5 c=4 fc=2332000 cc=448 chg=u
[85019.361663] [20210916_17:10:05.181529]@2 [cds_ol_rx_threa][0x17c16b38d20][17:10:05.181512] wlan: [4112:I:DATA] hdd_rx_packet_cbk: ARP packet received
```