/*
 * ejemplo_liquidcrystal.c - Demostración de todas las funciones estilo Arduino
 *                           en microcontroladores PIC con CCS C Compiler
 *
 * Microcontroladores soportados: PIC16F877A / PIC18F4550
 */

// ============================================================
// Selección de Microcontrolador (Descomenta uno)
// ============================================================
//#define USE_PIC16
#define USE_PIC18

#ifdef USE_PIC16
   #include <16F877A.h>
   #fuses HS, NOWDT, NOPROTECT, NOLVP
   #use delay(crystal = 20MHz)
#endif

#ifdef USE_PIC18
   #include <18F4550.h>
   #fuses HS, NOWDT, NOPROTECT, NOLVP, NOPBADEN, CPUDIV1
   #use delay(crystal = 20MHz)
#endif

// ============================================================
// Configuración de Pines del LCD (Modo 4 bits, RW a GND)
// ============================================================
#define LCD_DATA4  PIN_D4
#define LCD_DATA5  PIN_D5
#define LCD_DATA6  PIN_D6
#define LCD_DATA7  PIN_D7
#define LCD_RS     PIN_D2
#define LCD_EN     PIN_D3

// Inclusión de la librería avanzada
#include "lcd_flex.c"

// ============================================================
// Definición de Caracteres Personalizados (5x8)
// ============================================================
// Carácter 0: Corazón
unsigned int8 icono_corazon[8] = {
   0b00000,
   0b01010,
   0b11111,
   0b11111,
   0b01110,
   0b00100,
   0b00000,
   0b00000
};

// Carácter 1: Campana / Notificación
unsigned int8 icono_campana[8] = {
   0b00100,
   0b01110,
   0b01110,
   0b01110,
   0b11111,
   0b00000,
   0b00100,
   0b00000
};

// Carácter 2: Rayo / Batería
unsigned int8 icono_rayo[8] = {
   0b00010,
   0b00100,
   0b01000,
   0b11111,
   0b00100,
   0b01000,
   0b10000,
   0b00000
};

void main(void)
{
   char buffer_ram[16];
   unsigned int8 contador = 0;
   float temperatura = 25.437;

   // 1. Inicialización de periféricos
   setup_adc(ADC_OFF);
   setup_adc_ports(NO_ANALOGS);
#ifdef USE_PIC18
   setup_comparator(NC_NC_NC_NC);
#endif

   // 2. Inicialización del LCD (16 columnas x 2 filas)
   //    Equivalente a lcd.begin(16, 2) de Arduino
   lcd_begin(16, 2);

   // 3. Registrar Caracteres Personalizados en CGRAM (0 a 7)
   //    Equivalente a lcd.createChar(0, icono_corazon)
   lcd_create_char(0, icono_corazon);
   lcd_create_char(1, icono_campana);
   lcd_create_char(2, icono_rayo);

   // ------------------------------------------------------------
   // DEMO 1: Strings y Posicionamiento estilo Arduino (0-indexed)
   // ------------------------------------------------------------
   lcd_clear();
   lcd_set_cursor(0, 0); // Fila 0, Columna 0
   lcd_puts_const("LiquidCrystalPIC");

   lcd_set_cursor(0, 1); // Fila 1, Columna 0
   lcd_puts_const("Iconos: ");
   lcd_write(0); // Corazón (Custom char 0)
   lcd_putc(' ');
   lcd_write(1); // Campana (Custom char 1)
   lcd_putc(' ');
   lcd_write(2); // Rayo (Custom char 2)
   delay_ms(3000);

   // ------------------------------------------------------------
   // DEMO 2: Impresión de Enteros en distintas bases (DEC, HEX, BIN)
   // ------------------------------------------------------------
   lcd_clear();
   lcd_set_cursor(0, 0);
   lcd_puts_const("HEX:");
   lcd_print_uint(255, HEX);    // Imprime "FF"
   lcd_puts_const(" BIN:");
   lcd_print_uint(10, BIN);     // Imprime "1010"

   lcd_set_cursor(0, 1);
   lcd_puts_const("DEC:");
   lcd_print_int(-1250, DEC);   // Imprime "-1250"
   delay_ms(3500);

   // ------------------------------------------------------------
   // DEMO 3: Números Flotantes y buffer en RAM
   // ------------------------------------------------------------
   lcd_clear();
   lcd_set_cursor(0, 0);
   lcd_puts_const("Temp: ");
   lcd_print_float(temperatura, 2); // Imprime "25.44" (redondeado a 2 decimales)
   lcd_puts_const(" C");

   // Cadenas dinámicas desde memoria RAM
   strcpy(buffer_ram, "RAM String OK!");
   lcd_set_cursor(0, 1);
   lcd_puts(buffer_ram);
   delay_ms(3500);

   // ------------------------------------------------------------
   // DEMO 4: Control de Cursores y Parpadeo
   // ------------------------------------------------------------
   lcd_clear();
   lcd_set_cursor(0, 0);
   lcd_puts_const("Cursor ON...");
   lcd_cursor();
   delay_ms(2000);

   lcd_set_cursor(0, 1);
   lcd_puts_const("Blink ON...");
   lcd_blink();
   delay_ms(2000);

   lcd_no_cursor();
   lcd_no_blink();

   // ------------------------------------------------------------
   // Bucle principal: Contador continuo
   // ------------------------------------------------------------
   lcd_clear();
   lcd_set_cursor(0, 0);
   lcd_puts_const("Contador activo:");

   while (TRUE)
   {
      lcd_set_cursor(0, 1);
      lcd_print_dec(contador);
      lcd_puts_const(" (0x");
      lcd_print_hex(contador);
      lcd_puts_const(")     ");

      contador++;
      delay_ms(500);
   }
}
