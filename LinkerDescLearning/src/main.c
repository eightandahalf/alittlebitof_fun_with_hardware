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
uint32_t *vector_table[] __attribute__((section(".isr_vector"))) = {
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

volatile uint32_t dataVar = 0x3f;
volatile uint32_t bssVar;

void __initialize_bss (unsigned int* bss_begin, unsigned int* bss_end) {
	unsigned int *p = bss_begin;
	while (p < bss_end)
	*p++ = 0;
}

inline void __initialize_data (uint32_t* flash_begin, uint32_t* data_begin, uint32_t* data_end) {
	uint32_t *p = data_begin;
	while (p < data_end)
	*p++ = *flash_begin++;
}

void __attribute__ ((noreturn,weak)) _start (void) {
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

	while(1) {
		*GPIOA_ODR = 0x20;
		delay(200000);
		*GPIOA_ODR = 0x0;
		delay(200000);
	}
}

void delay(uint32_t count) {
	while(count--);
}
