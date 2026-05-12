#include <string.h>
#include <stdlib.h>
#include "cmsis_gcc.h"
#include "core_cm4.h"
#include "stm32f303xe.h"

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

/* Пользовательские функции */
int main(void);
void _start(void);
void delay(uint32_t count);

/* Минимальная таблица векторов */
uint32_t *vector_table[] __attribute__((section(".isr_vector"))) = {  // Секция называется ".isr_vector"
	(uint32_t *)SRAM_END, // указатель начала стека
	(uint32_t *)_start // _start в качестве Reset_Handler
};

// Начальный адрес значений инициализации секции .data,
// определенный в скрипте компоновщика.
extern uint32_t _sidata;
// Начальный адрес секции .data; определен в скрипте компоновщика
extern uint32_t _sdata;
// Конечный адрес секции .data; определен в скрипте компоновщика
extern uint32_t _edata;
// Начальный адрес секции .bss; определен в скрипте компоновщика
extern unsigned int _sbss;
// Конечный адрес секции .bss; определен в скрипте компоновщика
extern unsigned int _ebss;

extern unsigned int _sccm;
extern unsigned int _slccm;
extern unsigned int _ccmsize;

volatile uint32_t dataVar = 0x3f;
volatile uint32_t bssVar;
const char msg[] = "Hello World!";
const float vals[] = {3.14, 0.43, 1.414};
const int x = 5;
const int y = 5;
const int z = 5;

void *_sbrk(int incr) {
	extern uint32_t _end_static; /* определена компоновщиком */
	extern uint32_t _Heap_Limit;
	static uint32_t *heap_end;
	uint32_t *prev_heap_end;
	if (heap_end == 0) {
		heap_end = &_end_static;
	}
	prev_heap_end = heap_end;
	#ifdef __ARM_ARCH_6M__ // Если у нас микроконтроллер Cortex-M0/0+
	incr = (incr + 0x3) & (0xFFFFFFFC); /* Это гарантирует, что порции памяти всегда кратны 4 */
	#endif
	if (heap_end + incr > &_Heap_Limit) {
		asm("BKPT");
	}
	heap_end += incr;
	return (void*) prev_heap_end;
}

void __initialize_bss (unsigned int* bss_begin, unsigned int* bss_end) {
	unsigned int *p = bss_begin;
	while (p < bss_end)
	*p++ = 0;
}

void __initialize_data (uint32_t* flash_begin, uint32_t* data_begin, uint32_t* data_end) {
	uint32_t *p = data_begin;
	while (p < data_end)
	*p++ = *flash_begin++;
}

void __attribute__ ((noreturn,weak)) _start (void) {
	/* Копирование секции .ccm из FLASH-памяти (_slccm) в CCM-память */
	memcpy(&_sccm, &_slccm, (size_t)&_ccmsize);
	__DMB(); // Это гарантирует, что запись в память завершена
	SCB->VTOR = (uint32_t)&_sccm; /* Перемещение таблицы векторов в 0x1000 0000 */
	SYSCFG->RCR = 0xF; /* Включение защиты от записи для CCM-памяти */
	__DSB(); // Это гарантирует, что следующие команды используют новую конфигурацию

	__initialize_data(&_sidata, &_sdata, &_edata);
	__initialize_bss(&_sbss, &_ebss);
	main();
	for(;;);
}

int main() {
	/* Разрешение подачи тактирования на периферийные устройства GPIOA и GPIOC */
	*RCC_AHBENR = 0x20000;
	/* Конфигурирование PA5 в качестве выхода с подтяжкой к питанию */
	*GPIOA_MODER |= 0x400; // Установка MODER[11:10] = 0x1

//	char *heapMsg = (char*)malloc(sizeof(char)*strlen(msg));
//	strcpy(heapMsg, msg);

	SysTick_Config(4000000);

//	while(vals[0] >= 3.14 && strcmp(heapMsg, msg) == 0) {
//		*GPIOA_ODR = 0x20;
//		delay(200000);
//		*GPIOA_ODR = 0x0;
//		delay(200000);
//	}
}

void delay(uint32_t count) {
	while(count--);
}
