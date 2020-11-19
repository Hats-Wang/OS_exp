/*GNU as 格式的多任务内核程序 head.s
*包含 32 位保护模式初始化设置代码,时钟中断代码,系统调用中断代码和
*两个运行在特权级 3 上任务的代码
*初始化完成之后移动到任务 0 开始执行,并在时钟中断控制下进行任务 0
*和任务 1 之间的切换
*/
LATCH      =11930 #定时器初始计数值,即每隔 10ms 发送一次中断请求
SCRN_SEL   =0x18  #屏幕显示内存段选择子
TSS0_SEL   =0x20  #任务 0 的 TSS 段选择子
LDT0_SEL   =0x28  #任务 0 的 LDT 段选择子
TSS1_SEL   =0x30  #任务 1 的 TSS 段选择子
LDT1_SEL   =0x38  #任务 1 的 LDT 段选择子

.global startup_32
.text
.code32
startup_32:
# 加载 DS,SS,ESP
  movl $0x10,%eax
  mov %ax,%ds
  lss init_stack,%esp

# 在新的位置重新设置 IDT 和 GDT 表
  call setup_idt # 设置 IDT
  call setup_gdt # 设置 GDT
  movl $0x10,%eax # 重新加载所有段寄存器
  mov %ax,%ds
  mov %ax,%es
  mov %ax,%fs
  mov %ax,%gs
  lss init_stack,%esp

# 设置 8253 定时芯片
  movb $0x36, %al
  movl $0x43, %edx
  outb %al, %dx
  movl $LATCH, %eax # 频率 100HZ
  movl $0x40, %edx
  outb %al, %dx
  movb %ah, %al
  outb %al, %dx

# 在 IDT 表的第 8 和第 128(0x80)项处分别设置定时中断门描述符
# 和系统调用陷阱门描述符
  movl $0x00080000, %eax
  movw $timer_interrupt, %ax
  movw $0x8E00, %dx # 中断门类型是 14,特权级 0
  movl $0x08, %ecx
  lea idt(,%ecx,8), %esi
  movl %eax,(%esi)
  movl %edx,4(%esi)
  movw $system_interrupt, %ax
  movw $0xef00, %dx # 陷阱门类型是 15, 特权级 3 的程序可执行
  movl $0x80, %ecx
  lea idt(,%ecx,8), %esi
  movl %eax,(%esi)
  movl %edx,4(%esi)

# 为移动到任务 0 中执行来操作堆栈内容,在堆栈中人工建立中断返回时的现场
  pushfl

  andl $0xffffbfff, (%esp) #复位 EFLAGS 中的嵌套任务标志
  popfl
  movl $TSS0_SEL, %eax
  ltr %ax  #把任务 0 的 TSS 段选择子加载到任务寄存器 TR
  movl $LDT0_SEL, %eax
  lldt %ax #把任务 0 的 LDT 段选择子加载到 LDTR
  movl $0, current  #把当前任务号 0 保存在 current 变量中
  sti  #开启中断,并在栈中营造中断返回时的现场
  pushl $0x17  #把任务 0 的堆栈段选择子入栈(SS)
  pushl $init_stack  #把堆栈指针入栈(ESP)
  pushfl  #标志寄存器入栈(EFLAGS)
  pushl $0x0f  #当前代码段选择子入栈(CS)
  pushl $task0  #代码指针入栈(EIP)
  iret  #执行中断返回指令,切换到特权级 3 的任务 0 中执行

/**************设置 GDT 和 IDT 中描述符项的子程序****************/
setup_gdt:
  lgdt lgdt_opcode
  ret

setup_idt:
  lea ignore_int,%edx
  movl $0x00080000,%eax
  movw %dx,%ax
  movw $0x8E00,%dx
  lea idt,%edi
  mov $256,%ecx
rp_sidt:
  movl %eax,(%edi)
  movl %edx,4(%edi)
  addl $8,%edi
  dec %ecx
  jne rp_sidt
  lidt lidt_opcode
  ret


# 显示字符子程序。取当前光标位置并把 AL 中的字符显示在屏幕上。整屏可显示 80x25
# 个字符
write_char:
  push %gs
  pushl %ebx
  mov $SCRN_SEL, %ebx # GS 指向显示内存段
  mov %bx, %gs
  movl scr_loc, %ebx  # 从变量 scr_loc 中取目前字符显示位置值
  shl $1, %ebx
  movb %al, %gs:(%ebx)
  shr $1, %ebx
  incl %ebx
  cmpl $2000, %ebx # 下一个显示位置若大于 2000,复位成 0
  jb 1f
  movl $0, %ebx
  1: movl %ebx, scr_loc
  popl %ebx
  pop %gs
  ret

/************以下是三个中断处理程序***************/
/* ignore_int 是默认的中断处理程序,若系统产生了其他中断,
* 会在屏幕上显示字符'C'
*/
.align 2
ignore_int:
  push %ds
  pushl %eax
  movl $0x10, %eax
  mov %ax, %ds
  movl $67, %eax  /* 显示字符 'C' */
  call write_char
  popl %eax
  pop %ds
  iret

/* 定时中断处理程序,主要任务是执行任务切换操作 */
.align 2
timer_interrupt:
  push %ds
  pushl %eax
  movl $0x10, %eax
  mov %ax, %ds
  movb $0x20, %al
  outb %al, $0x20
  movl $1, %eax # 判断当前任务,若是任务 1 则去执行任务 0,或反之
  cmpl %eax, current
  je 1f
  movl %eax, current #若当前任务是 0,则 current=1,并跳转到任务 1 去执行
  ljmp $TSS1_SEL, $0
  jmp 2f
1: movl $0, current
   ljmp $TSS0_SEL, $0
2: popl %eax
   pop %ds
   iret

/* 系统调用中断 int 0x80 处理程序,该示例只有一个显示字符的功能 */
.align 2
system_interrupt:
  push %ds
  pushl %edx
  pushl %ecx
  pushl %ebx
  pushl %eax
  movl $0x10, %edx
  mov %dx, %ds
  call write_char
  popl %eax
  popl %ebx
  popl %ecx
  popl %edx
  pop %ds
  iret
/*********************************************/
current:.long 0 # 当前任务号(0 或 1)
scr_loc:.long 0 # 屏幕当前显示位置。按左上角到右下角顺序显示

.align 2
lidt_opcode:
  .word 256*8-1
  .long idt
lgdt_opcode:
  .word (end_gdt-gdt)-1
  .long gdt

  .align 8
idt: .fill 256,8,0 # IDT 表空间
gdt: .quad 0x0000000000000000 # GDT 表。第 1 个描述符不用
  .quad 0x00c09a00000007ff # 第 2 个是内核代码段描述符。选择子是 0x08
  .quad 0x00c09200000007ff # 第 3 个是内核数据段描述符。选择子是 0x10
  .quad 0x00c0920b80000002 # 第 4 个是显示内存段描述符。选择子是 0x18
  
  .word 0x0068, tss0, 0xe900, 0x0 # 第 5 个是 TSS0 段描述符。选择子是 0x20
  .word 0x0040, ldt0, 0xe200, 0x0 # 第 6 个是 LDT0 段描述符。选择子是 0x28
  .word 0x0068, tss1, 0xe900, 0x0 # 第 7 个是 TSS1 段描述符。选择子是 0x30
  .word 0x0040, ldt1, 0xe200, 0x0 # 第 8 个是 LDT1 段描述符。选择子是 0x38
end_gdt:
  .fill 128,4,0 # 初始内核堆栈空间
init_stack:
  .long init_stack
  .word 0x10


# 下面是任务 0 的 LDT 表段中的局部描述符
.align 8
ldt0: .quad 0x0000000000000000 # 第 1 个描述符不用
  .quad 0x00c0fa00000003ff # 第 2 个是局部代码段描述符,选择子 0x0f
  .quad 0x00c0f200000003ff # 第 3 个是局部数据段描述符,选择子 0x17

# 下面是任务 0 的 TSS 段内容
tss0: .long 0 /*前一任务链*/
  .long krn_stk0, 0x10 /* esp0, ss0 */
  .long 0, 0, 0, 0, 0 /*esp1, ss1, esp2, ss2, cr3 */
  .long 0, 0, 0, 0, 0 /*eip, eflags, eax, ecx, edx */
  .long 0, 0, 0, 0, 0 /*ebx esp, ebp, esi, edi */
  .long 0, 0, 0, 0, 0, 0 /*es, cs, ss, ds, fs, gs */
  .long LDT0_SEL, 0x8000000 /*ldt, I/O 位图基地址 */

  .fill 128,4,0 # 任务 0 的内核空间
krn_stk0:


# 下面是任务 1 的 LDT 表段内容和 TSS 段内容
.align 8
ldt1: .quad 0x0000000000000000
  .quad 0x00c0fa00000003ff
  .quad 0x00c0f200000003ff

tss1: .long 0
  .long krn_stk1, 0x10
  .long 0, 0, 0, 0, 0
  .long task1, 0x200
  .long 0, 0, 0, 0
  .long usr_stk1, 0, 0, 0
  .long 0x17,0x0f,0x17,0x17,0x17,0x17
  .long LDT1_SEL, 0x8000000

  .fill 128,4,0 # 任务 1 的内核空间,其用户栈直接使用初始栈空间
krn_stk1:

# 下面是任务 0 和任务 1 的子程序,分别循环显示字符 A 和 B
task0:
  movl $0x17, %eax
  movw %ax, %ds
  movb $65, %al /* 显示字符 'A' */
  int $0x80
  movl $0xfff, %ecx
1: loop 1b
  jmp task0

task1:
  movl $0x17, %eax
  movw %ax, %ds
  movb $66, %al /* 显示字符 'B' */
  int $0x80
  movl $0xfff, %ecx
1: loop 1b
  jmp task1

  .fill 128,4,0 # 任务 1 的用户栈空间
usr_stk1:
