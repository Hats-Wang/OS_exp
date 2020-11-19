/* GNU as 格式的 boot.s 程序
* 首先利用 BIOS 中断把内核代码(head.s 编译后的代码)加载到内存 0x10000 处,
* 然后移动到内存 0 处,最后进入保护模式,并跳转到内存 0 处继续运行
*/
BOOTSEG = 0x07c0
SYSSEG = 0x1000                  # 内核先加载到 0x10000 处,然后移动到 0x0 处
SYSLEN = 17                      # 内核占用的最大磁盘扇区数
.code16

.section .text
.globl _start
_start:
  ljmp $BOOTSEG, $go  # 段间跳转,当本程序刚开始运行时,所有段寄存器值
go: movw %cs, %ax   # 均为 0。该跳转语句会把 cs 寄存器加载为 0x07c0
  movw %ax, %ds
  movw %ax, %ss
  movw $0x400, %sp # 临时栈指针

# 加载内核代码到内存 0x10000 处
load_system:
  movw $0x0000, %dx # 如果从软盘启动 0x0000
  movw $0x0002, %cx
  movw $SYSSEG, %ax
  movw %ax, %es
  xorw %bx, %bx
  movw $0x200+SYSLEN, %ax
  int $0x13 # 利用 BIOS 中断 int 0x13 功能 2 从启动盘读取 head 代码
  jnc ok_load
die: jmp die

# 把内核代码移动到内存 0 开始处,共移动了 16 个扇区
ok_load:
  cli # 关闭中断
  movw $SYSSEG, %ax
  movw %ax, %ds
  xorw %ax, %ax
  movw %ax, %es
  movw $0x2000, %cx
  subw %si, %si
  subw %di, %di
  rep
  movsw

# 加载 IDT 和 GDT 基地址寄存器 IDTR 和 GDTR
  movw $BOOTSEG, %ax
  movw %ax, %ds
  lidt idt_48 # 加载 IDTR。6 字节操作数
  lgdt gdt_48 # 加载 GDTR。6 字节操作数

# 设置控制寄存器 CR0(机器状态字),进入保护模式
  movw $0x0001, %ax
  lmsw %ax
  ljmp $8, $0 # 此时段值已经是段选择子。注意与实模式的区别

# 下面是全局描述符表 GDT 的内容。包含 3 个段描述符
# 第一个不用,另外两个是代码段和数据段描述符
gdt: .word 0,0,0,0 # 段描述符 0,不用

  .word 0x07FF #段描述符 1。8Mb - 段限长=2047
  .word 0x0000 #段基地址
  .word 0x9A00 #代码段,可读/可执行
  .word 0x00C0 #段属性颗粒度=4KB

  .word 0x07FF #段描述符 2。8Mb - 段限长=2047
  .word 0x0000 #段基地址
  .word 0x9200 #数据段,可读/可写
  .word 0x00C0 #段属性颗粒度=4KB

# LIDT 和 LGDT 指令的 6 字节操作数
idt_48: .word 0
  .word 0,0
gdt_48: .word 0x7ff
  .word 0x7c00+gdt,0

.org 510
boot_flag:
  .word 0xAA55 # 引导扇区有效标志,必须处于引导扇区最后两个字节
