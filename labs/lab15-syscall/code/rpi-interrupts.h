#ifndef __RPI_INTERRUPT_H__
#define __RPI_INTERRUPT_H__

#include "rpi.h"

// from the valvers description.

/** @brief Bits in the Enable_Basic_IRQs register to enable various interrupts.
   See the BCM2835 ARM Peripherals manual, section 7.5 */
#define RPI_BASIC_ARM_TIMER_IRQ         (1 << 0)
#define RPI_BASIC_ARM_MAILBOX_IRQ       (1 << 1)
#define RPI_BASIC_ARM_DOORBELL_0_IRQ    (1 << 2)
#define RPI_BASIC_ARM_DOORBELL_1_IRQ    (1 << 3)
#define RPI_BASIC_GPU_0_HALTED_IRQ      (1 << 4)
#define RPI_BASIC_GPU_1_HALTED_IRQ      (1 << 5)
#define RPI_BASIC_ACCESS_ERROR_1_IRQ    (1 << 6)
#define RPI_BASIC_ACCESS_ERROR_0_IRQ    (1 << 7)

// http://xinu.mscs.mu.edu/BCM2835_Interrupt_Controller
typedef struct {
    volatile uint32_t IRQ_basic_pending;
    volatile uint32_t IRQ_pending_1;
    volatile uint32_t IRQ_pending_2;
    volatile uint32_t FIQ_control;
    volatile uint32_t Enable_IRQs_1;
    volatile uint32_t Enable_IRQs_2;
    volatile uint32_t Enable_Basic_IRQs;
    volatile uint32_t Disable_IRQs_1;
    volatile uint32_t Disable_IRQs_2;
    volatile uint32_t Disable_Basic_IRQs;
} rpi_irq_controller_t;

volatile rpi_irq_controller_t* RPI_GetIRQController(void);

// and this with IRQ_basic_pending to see if it's a timer interrupt.
#define RPI_BASIC_ARM_TIMER_IRQ         (1 << 0)

unsigned rpi_extract_basic_int(unsigned);

void system_enable_interrupts(void);
void system_disable_interrupts(void);

// return from exception in terms of privilege
void lower_privilege(void);

// location of these registers.
#define INTERRUPT_ENABLE_1  0x2000b210
#define INTERRUPT_ENABLE_2  0x2000b214
#define INTERRUPT_DISABLE_1 0x2000b21c
#define INTERRUPT_DISABLE_2 0x2000b220

// where the interrupt handlers go.
#define RPI_VECTOR_START  0

extern unsigned _interrupt_table;
extern unsigned _interrupt_table_end;


/*
  ldr pc, _reset_asm
  ldr pc, _undefined_instruction_asm
  ldr pc, _software_interrupt_asm
  ldr pc, _prefetch_abort_asm
  ldr pc, _data_abort_asm
  ldr pc, _irq_asm
  ldr pc, _interrupt_asm
*/
enum {
    RESET_INT = 0,          // 0  0x0
    UNDEFINED_INT = 1,      // 4  0x4
    SWI_INT = 2,            // 8  0x8
    PREFETCH_INT = 3,       // 12 0xc
    DATA_ABORT_INT = 4,     // 16 0x10
    INVALID = 5,            // 20 0x14  is a gap
    IRQ_INT0 = 6,           // 24 0x18
    FIQ_INT = 7             // 28 0x1c
};

typedef void (*interrupt_t)(uint32_t pc, uint32_t *saved_regs);

// override a given type of interrupt/exception.  call before int_init();
void int_set_handler(int t, interrupt_t handler);

// copy vectors, clear interrupt state.
void int_init(void);

#endif 
