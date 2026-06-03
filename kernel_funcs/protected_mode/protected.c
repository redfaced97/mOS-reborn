
  #include <protected/protected.h>

  void protected_mode(void) {

      printk("[Protector]\n");

      gdt_init();
      idt_init();

      pic_remap();
      irq_init();

      __asm__ __volatile__("sti");

      printk("\nKernel run protected mode!\n\n");

  }