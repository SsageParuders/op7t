# krhook

系统调用hook

## 使用说明

```shell
insmod krhook.so

# 输入需要过滤的pid, -1 代表不过滤
echo 860 > /dev/mypid

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

## 详细使用说明

[https://github.com/yhnu/op7t/tree/dev/kr_offline](https://github.com/yhnu/op7t/tree/dev/kr_offline)

## 偷天换日之文件重定向

[https://blog.csdn.net/Rong_Toa/article/details/86585086](https://blog.csdn.net/Rong_Toa/article/details/86585086)

[https://android.googlesource.com/kernel/common/+/android-trusty-4.14/arch/arm64/lib/copy_to_user.S](https://android.googlesource.com/kernel/common/+/android-trusty-4.14/arch/arm64/lib/copy_to_user.S)

[http://www.wowotech.net/memory_management/454.html](http://www.wowotech.net/memory_management/454.html)


```c
do
{            
    mm_segment_t fs;
    fs =get_fs();
    set_fs(KERNEL_DS);
    if(strstr(bufname, "a.txt"))
    {
        if(access_ok(VERIFY_WRITE, pathname, len))
        {
            if(copy_to_user(pathname+len-5, "b.txt", 5)) { //Internal error: Accessing user space memory with fs=KERNEL_DS: 9600004f [#1] PREEMPT SMP
                printk("[i] bingo magic\n");
            } else {
                printk("[e] copy to usr error. %p\n", pathname);
            }                    
        }
        else {
            printk("[e] access not ok\n");
        }
    }            
    set_fs(fs);
} while (0);

```

```shell
4,466251,1099988608,-;[20211009_16:11:06.256577]@2 [e]openat [/storage/emulated/0/Android/data/com.DefaultCompany.krhook_unity3d/files/a.txt] current->pid:[7671] ppid:[758] uid:[10226] tgid:[7636] stack:0x753214c388|0x742b412c18|0x742b434568|0x742be87054|0x742be7a930|0x742be78a6c|0x742be7493c|0x742be748b0|0x742bea7918|0x742b998fd8|0x742b9a5290|0x742b9a861c|0x742b7f68b0|0x742b7f6980|0x742b2f6750|0x742b7e10b8|0x742baf6d38|0x742bcce3fc|0x742b2f9ab0|0x742b7f0388|0x742b7ee83c|0x742b7ee0cc|0x742b25884c|0x742b7df7e4|0x742b274230|0x742b4a8eac|0x742b40bf04|0x74410c51d0|0xffffffffffffffff|
4,466252,1099988617,-;[20211009_16:11:06.256587]@2 [e] copy to usr error. 0000000064a1fc40
```
