我原本想跟据写的顺序来说的，不过进程调度的关系比较紧密，所以我先说系统调用好了……

//1、添加系统调用（包括第二问，在作业里面一共要添加4个，而且例子里面原本的那个get_ticks的不能删）
对于添加步骤，可见pdf的p315

系统调用……我的理解就是调用一个操作系统所提供的API……
像是获取系统当前时间、弹出一个对话框啥的……
也就是把事情交给操作系统来做
比如要访问一段进程之间公用的内存（应该也属于系统调用）

在第六章例子的代码中其实写的不是很完善，在里面的是没有参数传递的，然后我就看了第七章的来写
所以可能改的有点多，我到后面再说233

系统调用的大致流程是这样的：
1、进程A调用一个函数（有可能这个函数就是系统的函数，不过我是没有直接调用系统的函数……）
2、调用系统函数func()
3、系统函数func()是用汇编写的，它会把要的参数存起来，然后中断这个进程
3.5、这个中断是需要自己实现的，要给它一个和系统中断不一样的值，然后去操作那个中断用的芯片，不过例子里面已经实现出来了
4、来到内核，查表得到所对应的内核函数sys_func()，调用
5、返回

按照流程，开始具体的修改步骤：
/*
1、先写系统函数（也就是上面说的func()），在syscall.asm里面：
	1、写上声明_NR_sys_disp_str equ 1;之类的，这个东西代表的是：这个函数在内核查表的时候的下标，比如0就是查找这个数组中的第一个东西，1是第二个，至于这个数组存的是什么等下再说。
	2、然后写要实现的四个函数（其实内容都是一样的）：1、先把上面声明的那个下标值存到寄存器eax里面，2、加上一句mov ebx, [esp+4]，把参数压到ebx里面（因为都是只有一个参数），3、调用中断

2、修改kernal.asm，要在sys_call里面把参数压栈，我这部分有点懒得看orz

3、记得在syscall.asm的上面加上global那四个函数名，在proto.h加上那四个函数带上参数返回值的声明，以便让c调用

4、进入内核，为了查表，首先要声明有多少个系统函数，写在const.h的 #define NR_SYS_CALL 5

5、然后global.c里面的sys_call_table[NR_SYS_CALL]，后面要根据第1步中_NR_sys_disp_str之类的值，写上要被调用的函数名，比如我_NR_sys_disp_str 1，那么我在这个数组里面的第二个就是sys_disp_str，这个sys_disp_str就是这个函数所在的位置，我查表查到了这个，然后就会调用这个函数

6、然后就是实现系统调用的函数了，我都写在proc.c里面了
	void sys_process_sleep(int i) ---等说到再讲这个……
	void sys_disp_str(char* str) ----也就是第二问，直接调用那个disp_str(str);
	int sys_sem_p(int ID) -----p操作，根据ID对对应的变量-1，然后返回这个变量的值
	int sys_sem_V(int ID) -----v操作，根据ID对对应的变量+1，然后返回这个变量的值

7、再在proto.h加上那四个函数带上参数返回值的声明，给C调用……



8、把main.c里面Test A、B、C、D（等下要加上D的）里面的disp_str全部改成sys_disp_str

9、强迫症患者还将屏幕清空了，在main的kernal_main函数里面，restart前面加了
	disp_pos=0;
	for(i=0;i<80*25;i++){
		disp_str(" ");
	}
	disp_pos=0;
   来清空屏幕（就是用空白覆盖了有字的地方然后把开始的位置移到左上角）

*/


//2、在第六章代码之上新增一个进程（作业第四点）
对于添加步骤，可见pdf的p226

在例子的代码中已经是写好了进程之间所有切换的步骤，我们只需要加入新的进程，修改所有进程数就好，

/*
1、由于要新建一个进程，所以必须要有新建之后进程执行的函数，所以首先要在main.c新增一个函数TestD，这个函数与TestA、B、C一样，会被一个进程单独调用（函数里面有一个死循环）

2、然后就是新建一个进程。在global.c的task_table中新增了{TestD, STACK_SIZE_TESTD, "TestD"}。task_table这个表（实际就是个数组）就是系统开始运行后所要启动的进程、栈的大小、名字，它会在main.c的kernel_main()里面初始化

3、然后要告诉内核说我现在新增了一个进程，所以要在proc.h的NR_TASKS里面的值加1，这个其实就是进程的个数，比如在初始化的时候就会被用到

4、然后还要在下面新定义一个进程的栈大小 #define STACK_SIZE_TESTD 0x8000，由于在一个进程被中断之后，数据什么的要被保存下来，最后在最后的栈的总大小STACK_SIZE_TOTAL里面，还要加上STACK_SIZE_TESTD的大小

5、在proto.h新增一个void TestD();的声明
*/

这样下来就新增了一个进程……
然后在这里说一下进程的切换……

假设要从进程A切换到进程B，先不管是什么原因造成的（在时钟中断的时候再说）
1、系统需要先把进程A中运行的所有数据（寄存器里面的值）先存到堆栈里面，
2、然后栈顶指针esp跳到进程表，指向下一个进程进程B（这里可能执行了某些调度方法）
3、栈顶指针esp跳到存了进程B数据的地方，然后把所有的数据读到寄存器里去






======还有两个orz========







// 1、关于sys_process_sleep
用成员变量记录当前进程是否时间没到 ==> global.h里面【ticks那里

proc.h在调度的时候加一个判断进程的tick，抛弃了优先级priority;
proc.c的调度，clock.c加了一个

在每次中断的时候所有的成员变量tick都-1如果，不为0就不进这个进程





//4、PV
三个全局变量
gloal.h =====>
EXTERN	int		mutex;
EXTERN	int		full;
EXTERN	int		empty;

const.h =====>
#define	MUTEX  0;
#define	FULL  1;
#define	EMPTY  2;
#define	BUFFER_NUMBER  5;

初始化
main.h =====>
mutex = 1;
full = 0;
empty = BUFFER_NUMBER;

main.c ====>
P(int ID)
V(int ID)

main.c PV操作的队列====>由于只有三个所以大小为3
proc.h 新增 isWait
proc.c ====>
		if(p_proc_ready->isWait){
			p_proc_ready++;
		}


sys_disp_str、sys_sem_p和sys_sem_v


