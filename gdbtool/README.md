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

    ```shell
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
2. 
