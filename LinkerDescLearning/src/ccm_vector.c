typedef unsigned long uint32_t;
/* Начальные адреса памятей и периферии (общие для всех микроконтроллеров STM32) */
#define FLASH_BASE 0x08000000
#define SRAM_BASE 0x20000000
#define PERIPH_BASE 0x40000000
/* Определение конечного адреса SRAM как указателя начала стека
* (специфично для каждого микроконтроллера STM32) */
#define SRAM_SIZE 64*1024 // STM32F303RE имеет 64 КБ SRAM
#define SRAM_END (SRAM_BASE + SRAM_SIZE)
/* Адреса системы RCC, применимые к GPIOA
* (специфично для каждого микроконтроллера STM32) */
#define RCC_BASE (PERIPH_BASE + 0x21000)
#define RCC_AHBENR ((uint32_t*)(RCC_BASE + 0x14))
/* Адреса периферийного устройства GPIOA
* (специфично для каждого микроконтроллера STM32) */
#define GPIOA_BASE (PERIPH_BASE + 0x8000000)
#define GPIOA_MODER ((uint32_t*)(GPIOA_BASE + 0x00))
#define GPIOA_ODR ((uint32_t*)(GPIOA_BASE + 0x14))

extern const uint32_t _estack;

void SysTick_Handler(void);

uint32_t *ccm_vector_table[] __attribute__((section(".isr_vector_ccm"))) = {
	(uint32_t *)SRAM_END, // указатель начала стека
	(uint32_t *) 0, // Reset_Handler не перемещаемый
	(uint32_t *) 0,
	(uint32_t *) 0,
	(uint32_t *) 0,
	(uint32_t *) 0,
	(uint32_t *) 0,
	(uint32_t *) 0,
	(uint32_t *) 0,
	(uint32_t *) 0,
	(uint32_t *) 0,
	(uint32_t *) 0,
	(uint32_t *) 0,
	(uint32_t *) 0,
	(uint32_t *) 0,
	(uint32_t *) SysTick_Handler
};
void __attribute__((section(".ccm"))) SysTick_Handler(void) {
	*GPIOA_ODR = *GPIOA_ODR ? 0x0 : 0x20; // Вызывает мигание светодиода LD2
}
