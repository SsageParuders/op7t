# GDBServer 硬件断点使用说明

## 准备工作

1. Linux内核要在2.6.37以后的版本才支持对ARM添加硬件断点
    
    我们使用的内核版本是4.14.117,因此完全支持的

2. GDB要在7.3以后的版本才支持对ARM添加硬件断点(我们使用最新的ndk-23)

    因为工作环境使用比较老的NDK版本,因此会出现下面的错误

    ![20211006121214](https://cdn.jsdelivr.net/gh/yhnu/PicBed/20211006121214.png)

3. 内核的编译选项需要开启硬件断点支持

    硬件断点的功能主要由kernel的ptrace模块针对arm硬件中断进行了封装处理, 但是需要开启对应宏编译, 如果想自己实现硬件断点也可以参考对应的实现

    ```shell
    # https://github.com/yhnu/op7t/blob/dev/blu7t/op7-r70/arch/arm64/kernel/ptrace.c
    CONFIG_COMPAT=y
    CONFIG_HAVE_HW_BREAKPOINT=y
    ```

## 小试牛刀

1. 使用ndk编译下面的示例代码

    ```c++
    // hello.cpp
    #include <stdio.h>
    #include <unistd.h>

    char gBuf[1024] = "1024";
    int gInt = 0;
    int main(int argc, char const *argv[])
    {
        printf("gBuf=%p %p %d\n", gBuf, &gInt, getpid());
        while(1) {
            gInt++;
            // printf("gBuf=%p %d\n", gBuf, gInt);
            sleep(5);
        }
        return 0;
    }
    ```
2. 将64位gdbserver和hello push to /data/local/tmp

    ```shell
    adb push android-arm64/gdbserver/gdbserver /data/local/tmp
    adb push hello /data/local/tmp
    ```

3. android启动对应gdbserver

    ```shell
    OnePlus7T:/data/local/tmp # ./gdbserver :1234 hello
    warning: Found custom handler for signal 39 (Unknown signal 39) preinstalled.
    Some signal dispositions inherited from the environment (SIG_DFL/SIG_IGN)
    won't be propagated to spawned programs.
    Process /data/local/tmp/hello created; pid = 8022
    Listening on port 1234
    ```

4. pc连接安卓gdbserver

    adb forward tcp:1234 tcp:1234
    #gdb: aliased to /opt/android-ndk-r21e//prebuilt/darwin-x86_64/bin/gdb
    ➜  op7t git:(dev) ✗ gdb
    GNU gdb (GDB) 8.3
    Copyright (C) 2019 Free Software Foundation, Inc.
    License GPLv3+: GNU GPL version 3 or later <http://gnu.org/licenses/gpl.html>
    This is free software: you are free to change and redistribute it.
    There is NO WARRANTY, to the extent permitted by law.
    Type "show copying" and "show warranty" for details.
    This GDB was configured as "x86_64-apple-darwin14.5.0".
    Type "show configuration" for configuration details.
    For bug reporting instructions, please see:
    <http://www.gnu.org/software/gdb/bugs/>.
    Find the GDB manual and other documentation resources online at:
        <http://www.gnu.org/software/gdb/documentation/>.

    For help, type "help".
    Type "apropos word" to search for commands related to "word".
    (gdb) target remote :1234
    Remote debugging using :1234
    Reading /data/local/tmp/hello from remote target...
    warning: File transfers from remote targets can be slow. Use "set sysroot" to access files locally instead.
    Reading /data/local/tmp/hello from remote target...
    Reading symbols from target:/data/local/tmp/hello...
    Reading /system/bin/linker64 from remote target...
    Reading /system/bin/linker64 from remote target...
    Reading symbols from target:/system/bin/linker64...
    (No debugging symbols found in target:/system/bin/linker64)
    0x0000007fbf634a50 in __dl__start () from target:/system/bin/linker64
    (gdb) b *main #set breakpoint
    Breakpoint 1 at 0x5555555754: file F:/F2021-07/ak47hook/hello/hello.c, line 7.
    (gdb) c       #run 
    Continuing.
    Reading /system/lib64/libandroid.so from remote target...
    Reading /apex/com.android.runtime/lib64/bionic/libm.so from remote target...
    Reading /apex/com.android.runtime/lib64/bionic/libdl.so from remote target...
    Reading /apex/com.android.runtime/lib64/bionic/libc.so from remote target...

    #trigger the breakpoint

    Breakpoint 1, main (argc=0, argv=0x0) at F:/F2021-07/ak47hook/hello/hello.c:7
    7	F:/F2021-07/ak47hook/hello/hello.c: No such file or directory.
    
    (gdb) hbreak *sleep #trigger the hard breakpoint
    Hardware assisted breakpoint 3 at 0x7fbc9ea430
    (gdb) c
    Continuing.

    Breakpoint 3, 0x0000007fbc9ea430 in sleep () from target:/apex/com.android.runtime/lib64/bionic/libc.so
    (gdb)
