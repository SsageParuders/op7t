# 函数调用与堆栈回溯

## 前言

早前在内核中实现了堆栈回溯的功能,方便进行外挂分析和逆向, 目前来看绝大多数时间是没有问题的. 但是有的时候某些函数回溯会失败, 其核心原因是fp 是null.

## 堆栈回溯的原理是通过fp进行回溯的

![20211029172735](https://cdn.jsdelivr.net/gh/yhnu/PicBed/20211029172735.png)


![20211029173344](https://cdn.jsdelivr.net/gh/yhnu/PicBed/20211029173344.png)

```c
struct stack_frame_user {
	const void __user *next_fp;
	unsigned long		lr;
};
```

## 为什么失效

因为是基于fp进行堆栈回溯, 但是fp并不是必要功能, 可以被编译器优化掉. 相关文章如下:

[https://blog.csdn.net/myxmu/article/details/13630255](https://blog.csdn.net/myxmu/article/details/13630255)

[https://www.keil.com/support/man/docs/armclang_ref/armclang_ref_vvi1466179578564.htm](https://www.keil.com/support/man/docs/armclang_ref/armclang_ref_vvi1466179578564.htm)

    -fomit-frame-pointer omits the storing of stack frame pointers during function calls.

    The -fomit-frame-pointer option instructs the compiler to not store stack frame pointers if the function does not need it. You can use this option to reduce the code image size.

    The -fno-omit-frame-pointer option instructs the compiler to store the stack frame pointer in a register. In AArch32, the frame pointer is stored in register R11 for A32 code or register R7 for T32 code. In AArch64, the frame pointer is stored in register X29. The register that is used as a frame pointer is not available for use as a general-purpose register. It is available as a general-purpose register if you compile with -fomit-frame-pointer.

关于fp的使用限制也有对应的说明, 简单来说fp可以被用来当成普通的寄存器.

    Frame pointer limitations for stack unwinding
    Frame pointers enable the compiler to insert code to remove the automatic variables from the stack when C++ exceptions are thrown. This is called stack unwinding. However, there are limitations on how the frame pointers are used:
    
    **默认情况下, 并不做保证**
    By default, there are no guarantees on the use of the frame pointers.

    There are no guarantees about the use of frame pointers in the C or C++ libraries.
    If you specify -fno-omit-frame-pointer, then any function which uses space on the stack creates a frame record, and changes the frame pointer to point to it. There is a short time period at the beginning and end of a function where the frame pointer points to the frame record in the caller's frame.
    If you specify -fno-omit-frame-pointer, then the frame pointer always points to the lowest address of a valid frame record. A frame record consists of two words:
    
    **arm的官方也说明了fp和lr寄存器的方式模式**
    the value of the frame pointer at function entry in the lower-addressed word.
    the value of the link register at function entry in the higher-addressed word.
    A function that does not use any stack space does not need to create a frame record, and leaves the frame pointer pointing to the caller's frame.
    In AArch32 state, there is currently no reliable way to unwind mixed A32 and T32 code using frame pointers.
    The behavior of frame pointers in AArch32 state is not part of the ABI and therefore might change in the future. The behavior of frame pointers in AArch64 state is part of the ABI and is therefore unlikely to change.

![20211029175241](https://cdn.jsdelivr.net/gh/yhnu/PicBed/20211029175241.png)

![20211029175546](https://cdn.jsdelivr.net/gh/yhnu/PicBed/20211029175546.png)

## Unity引擎底层编译选项

```shell
.\External\Jamplus\builds_csharp\bin\modules\c-compilers\androidndk-gcc.jam.cs
30:            Vars.NDK_CFLAGS_RELEASE.Assign("-O2", "-fomit-frame-pointer", "-fstrict-aliasing", "-funswitch-loops", "-finline-limit=300");
31:            Vars.NDK_CFLAGS_DEBUG.Assign("-O0", "-g", "-fno-omit-frame-pointer", "-fno-strict-aliasing");
```

```shell
.\External\il2cpp\il2cpp\external\boehmgc\configure.ac
707:        AC_MSG_WARN("Client must not use -fomit-frame-pointer.")
```

通过上面的编译参数可以知晓, Debug版本默认会开启fp, 但是release版本会禁用. il2cpp因为boehmgc的底层原理导致fp必须开启, 所以il2cpp在内核层是可以快速进行回溯的.

## 对应问题的解决方案

一劳永逸的方式是写一个底层驱动,然后在应用层进行离线回溯. 但是如果你能够挂载调试器,这也不是什么大问题了.

## HelpLink

[https://azeria-labs.com/functions-and-the-stack-part-7/](https://azeria-labs.com/functions-and-the-stack-part-7/)

[https://phoenix.goucher.edu/~kelliher/f2013/cs220/10.pdf](https://phoenix.goucher.edu/~kelliher/f2013/cs220/10.pdf)

[https://phoenix.goucher.edu/~kelliher/f2013/cs220/](https://phoenix.goucher.edu/~kelliher/f2013/cs220/)

[https://zhuanlan.zhihu.com/p/336916116](https://zhuanlan.zhihu.com/p/336916116)

[https://www.one-tab.com/page/HXEAFcbxSfySevIUbq0Jeg](https://www.one-tab.com/page/HXEAFcbxSfySevIUbq0Jeg)