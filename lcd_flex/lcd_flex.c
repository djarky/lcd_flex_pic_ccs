/*
 * lcd_flex.c - Driver flexible y avanzado para LCD HD44780 en CCS C Compiler
 *
 * Paridad con LiquidCrystal de Arduino y la clase Print:
 * - Modo 4 bits en cualquier pin (D4..D7, RS, EN, RW a GND)
 * - Pantallas 16x2, 20x2, 16x4, 20x4
 * - Impresión de cadenas de texto (RAM y ROM)
 * - Impresión de enteros (DEC, HEX, BIN, OCT) y floats
 * - Totalmente optimizado para arquitectura PIC (PIC16F877A / PIC18F4550)
 *   sin colisión de pila de llamadas ni variables corruptas en overlays.
 */

#ifndef __LCD_FLEX_C__
#define __LCD_FLEX_C__

#include "lcd_flex.h"

// ============================================================
// Envío de 4 bits (nibble) al bus de datos del LCD
// ============================================================
void lcd_send_nibble(unsigned int8 nibble)
{
   output_bit(LCD_DB4, bit_test(nibble, 0));
   output_bit(LCD_DB5, bit_test(nibble, 1));
   output_bit(LCD_DB6, bit_test(nibble, 2));
   output_bit(LCD_DB7, bit_test(nibble, 3));

   delay_cycles(1);
   output_high(LCD_E);
   delay_us(2);   // Pulso Enable > 450 ns
   output_low(LCD_E);
}

// ============================================================
// Lectura (solo si se activó USE_LCD_RW y existe LCD_RW)
// ============================================================
#if defined(USE_LCD_RW) && defined(LCD_RW)
unsigned int8 lcd_read_nibble(void)
{
   unsigned int8 retval = 0;

   output_high(LCD_E);
   delay_us(1);

   if (input(LCD_DB4)) bit_set(retval, 0);
   if (input(LCD_DB5)) bit_set(retval, 1);
   if (input(LCD_DB6)) bit_set(retval, 2);
   if (input(LCD_DB7)) bit_set(retval, 3);

   output_low(LCD_E);
   return retval;
}

unsigned int8 lcd_read_byte(void)
{
   unsigned int8 high, low;

   output_high(LCD_RW);
   delay_cycles(1);

   high = lcd_read_nibble();
   low  = lcd_read_nibble();

   return ((high << 4) | low);
}
#endif

// ============================================================
// Envío de 1 byte al LCD (address = 0 para comando, 1 para dato)
// ============================================================
void lcd_send_byte(unsigned int8 address, unsigned int8 n)
{
   output_low(LCD_RS);

#if defined(USE_LCD_RW) && defined(LCD_RW)
   while (bit_test(lcd_read_byte(), 7)); // Espera busy flag
#else
   delay_us(50);                         // Retardo de ejecución HD44780 (~37-50us)
#endif

   if (address) {
      output_high(LCD_RS);
   } else {
      output_low(LCD_RS);
   }

   delay_cycles(1);

#if defined(USE_LCD_RW) && defined(LCD_RW)
   output_low(LCD_RW);
   delay_cycles(1);
#endif

   output_low(LCD_E);

   lcd_send_nibble(n >> 4);
   lcd_send_nibble(n & 0x0F);
}

// ============================================================
// Comandos directos y datos crudos
// ============================================================
void lcd_command(unsigned int8 cmd)
{
   lcd_send_byte(0, cmd);
}

void lcd_write(unsigned int8 val)
{
   lcd_send_byte(1, val);
}

// ============================================================
// Inicialización del display (Secuencia estándar HD44780)
// Tiempos seguros y compatibilidad total con simulación Proteus
// ============================================================
void lcd_begin(unsigned int8 cols, unsigned int8 lines)
{
   unsigned int8 i;
   unsigned int8 function_set;

   output_low(LCD_RS);
#if defined(USE_LCD_RW) && defined(LCD_RW)
   output_low(LCD_RW);
#endif
   output_low(LCD_E);

   // Espera tras encendido (> 40 ms según datasheet HD44780)
   delay_ms(50);

   // Secuencia de reset para modo 4 bits
   for (i = 0; i < 3; i++) {
      lcd_send_nibble(0x03);
      delay_ms(5);
   }

   // Cambiar a interfaz de 4 bits
   lcd_send_nibble(0x02);
   delay_ms(1);

   // Configuración de líneas y tamaño de fuente
   if (lines > 1) {
      function_set = 0x28; // 4 bits, 2 líneas, 5x8 dots
   } else {
      function_set = 0x20; // 4 bits, 1 línea, 5x8 dots
   }

   lcd_send_byte(0, function_set); // Function Set
   delay_ms(5);

   lcd_send_byte(0, 0x0C);         // Display ON, cursor OFF, blink OFF
   delay_ms(5);

   lcd_send_byte(0, 0x01);         // Clear display
   delay_ms(5);

   lcd_send_byte(0, 0x06);         // Entry mode: incrementar cursor
   delay_ms(5);
}

void lcd_init(void)
{
   lcd_begin(16, 2);
}

// ============================================================
// Posicionamiento de cursor
// Soporta displays de 16x2, 20x2, 16x4 y 20x4
// ============================================================
void lcd_gotoxy(unsigned int8 x, unsigned int8 y)
{
   unsigned int8 address;

   switch (y)
   {
      case 1:  address = 0x00; break;
      case 2:  address = 0x40; break;
      case 3:  address = 0x14; break;
      case 4:  address = 0x54; break;
      default: address = 0x00; break;
   }

   address += (x - 1);
   lcd_send_byte(0, 0x80 | address);
}

// Posicionamiento estilo Arduino (0-indexed: col 0..cols-1, row 0..lines-1)
void lcd_set_cursor(unsigned int8 col, unsigned int8 row)
{
   lcd_gotoxy(col + 1, row + 1);
}

// ============================================================
// Escritura de carácter compatible con printf(lcd_putc, ...)
// Directo y ligero: no satura la pila ni corrompe punteros en PIC16
// ============================================================
void lcd_putc(char c)
{
   switch (c)
   {
      case '\f': // Form feed: Limpiar pantalla
         lcd_send_byte(0, 0x01);
         delay_ms(2);
         break;

      case '\n': // Nueva línea: Ir a fila 2
         lcd_gotoxy(1, 2);
         break;

      case '\b': // Backspace
         lcd_send_byte(0, 0x10);
         break;

      default:   // Carácter normal o Custom Char (0..7)
         lcd_send_byte(1, (unsigned int8)c);
         break;
   }
}

// ============================================================
// Funciones de control de pantalla (LiquidCrystal parity)
// ============================================================
void lcd_clear(void)
{
   lcd_send_byte(0, 0x01);
   delay_ms(2);
}

void lcd_home(void)
{
   lcd_send_byte(0, 0x02);
   delay_ms(2);
}

void lcd_display(void)
{
   lcd_send_byte(0, 0x0C);
}

void lcd_no_display(void)
{
   lcd_send_byte(0, 0x08);
}

void lcd_cursor(void)
{
   lcd_send_byte(0, 0x0E);
}

void lcd_no_cursor(void)
{
   lcd_send_byte(0, 0x0C);
}

void lcd_blink(void)
{
   lcd_send_byte(0, 0x0D);
}

void lcd_no_blink(void)
{
   lcd_send_byte(0, 0x0C);
}

void lcd_scroll_left(void)
{
   lcd_send_byte(0, 0x18);
}

void lcd_scroll_right(void)
{
   lcd_send_byte(0, 0x1C);
}

void lcd_left_to_right(void)
{
   lcd_send_byte(0, 0x06);
}

void lcd_right_to_left(void)
{
   lcd_send_byte(0, 0x04);
}

void lcd_autoscroll(void)
{
   lcd_send_byte(0, 0x07);
}

void lcd_no_autoscroll(void)
{
   lcd_send_byte(0, 0x06);
}

// ============================================================
// Caracteres personalizados en CGRAM (0..7)
// ============================================================
#ifndef LCD_CUSTOM_CHAR_DEFINED
#define LCD_CUSTOM_CHAR_DEFINED
void lcd_create_char(unsigned int8 location, unsigned int8 *charmap)
{
   unsigned int8 i;
   location &= 0x07;
   lcd_send_byte(0, 0x40 | (location << 3));
   for (i = 0; i < 8; i++) {
      lcd_send_byte(1, charmap[i]);
   }
   lcd_send_byte(0, 0x80); // Retornar dirección a DDRAM
}
#endif

// ============================================================
// Impresión de cadenas de texto en RAM
// ============================================================
void lcd_puts(char *s)
{
   while (*s != 0) {
      lcd_putc(*s);
      s++;
   }
}

// ============================================================
// Impresión de números enteros con base configurable
// (DEC, HEX, BIN, OCT) - Optimizado en 16 bits para PIC
// ============================================================
void lcd_print_uint(unsigned int16 n, unsigned int8 base)
{
   char buf[17];
   unsigned int8 i = 0;
   unsigned int8 rem;

   if (base < 2 || base > 16) {
      base = 10;
   }

   if (n == 0) {
      lcd_putc('0');
      return;
   }

   while (n > 0) {
      rem = (unsigned int8)(n % base);
      if (rem < 10) {
         buf[i++] = (char)('0' + rem);
      } else {
         buf[i++] = (char)('A' + rem - 10);
      }
      n /= base;
   }

   while (i > 0) {
      lcd_putc(buf[--i]);
   }
}

void lcd_print_int(signed int16 n, unsigned int8 base)
{
   if (base == 10 && n < 0) {
      lcd_putc('-');
      n = -n;
   }
   lcd_print_uint((unsigned int16)n, base);
}

// ============================================================
// Impresión de punto flotante (float)
// ============================================================
void lcd_print_float(float val, unsigned int8 digits)
{
   unsigned int16 int_part;
   float remainder;
   unsigned int8 i, d;

   if (val < 0.0) {
      lcd_putc('-');
      val = -val;
   }

   int_part = (unsigned int16)val;
   remainder = val - (float)int_part;

   lcd_print_uint(int_part, 10);

   if (digits > 0) {
      lcd_putc('.');
      for (i = 0; i < digits; i++) {
         remainder *= 10.0;
         d = (unsigned int8)remainder;
         lcd_putc((char)('0' + d));
         remainder -= (float)d;
      }
   }
}

// ============================================================
// Funciones estilo println
// ============================================================
void lcd_println(void)
{
   lcd_gotoxy(1, 2);
}

void lcd_println_str(char *s)
{
   lcd_puts(s);
   lcd_println();
}

void lcd_println_int(signed int16 n, unsigned int8 base)
{
   lcd_print_int(n, base);
   lcd_println();
}

void lcd_println_float(float val, unsigned int8 digits)
{
   lcd_print_float(val, digits);
   lcd_println();
}

#endif // __LCD_FLEX_C__
