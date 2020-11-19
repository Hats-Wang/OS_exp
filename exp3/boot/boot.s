/* GNU as 汇编语言编制的 512B 引导扇区代码
* 运行在 16 位实地址模式下
*/
BOOTSEG = 0x07c0 # BIOS 加载引导扇区代码的原始段地址
.code16
.section .text
.globl _start
_start:
        ljmp $BOOTSEG, $go # 段间跳转,执行之后 cs=0x07C0
go: movw %cs, %ax
    movw %ax, %ds
    movw %ax, %es
    movb %ah, load_msg+17 # 替换字符串中最后一个点符号,响铃一次
    movw $20, %cx # 共显示 20 个字符,包括回车和换行
    movw $0x1004, %dx # 字符串将显示在屏幕的第 17 行,第 5 列处
    movw $0x000c, %bx # 字符显示属性(红色)
    movw $load_msg, %bp # 指向要显示的字符串
    movw $0x1301, %ax
    int $0x10 # BIOS 中断调用 0x10, 功能 0x13, 子功能 01
loop: jmp loop # 无限循环
load_msg:  .ascii "Loading system ..."
          .byte 13,10
.org 510
boot_flag:
      .word 0xAA55 # 有效引导扇区标志,供 BIOS 加载引导扇区使用
