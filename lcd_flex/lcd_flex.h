/*
 * lcd_flex.h - Driver avanzado para LCD HD44780 en CCS C Compiler
 *
 * Paridad con LiquidCrystal de Arduino y la clase Print:
 * - 4 bits en cualquier pin (D4..D7, RS, EN, RW a GND)
 * - Pantallas 16x2, 20x2, 16x4, 20x4
 * - Impresión de cadenas de texto (RAM y ROM)
 * - Impresión de números en bases DEC, HEX, BIN, OCT
 * - Impresión de punto flotante (float)
 * - Control de display, cursor, blink, scroll y custom characters (CGRAM 0-7)
 */

#ifndef __LCD_FLEX_H__
#define __LCD_FLEX_H__

// Directiva requerida por CCS C para permitir pasar cadenas constantes a funciones
#device PASS_STRINGS = IN_RAM

// ============================================================
// Bases numéricas (compatibilidad con Arduino)
// ============================================================
#ifndef DEC
   #define DEC 10
#endif
#ifndef HEX
   #define HEX 16
#endif
#ifndef OCT
   #define OCT 8
#endif
#ifndef BIN
   #define BIN 2
#endif

// ============================================================
// Comandos del controlador HD44780
// ============================================================
#define LCD_CLEARDISPLAY   0x01
#define LCD_RETURNHOME     0x02
#define LCD_ENTRYMODESET   0x04
#define LCD_DISPLAYCONTROL 0x08
#define LCD_CURSORSHIFT    0x10
#define LCD_FUNCTIONSET    0x20
#define LCD_SETCGRAMADDR   0x40
#define LCD_SETDDRAMADDR   0x80

// Flags para Entry Mode
#define LCD_ENTRYRIGHT          0x00
#define LCD_ENTRYLEFT           0x02
#define LCD_ENTRYSHIFTINCREMENT 0x01
#define LCD_ENTRYSHIFTDECREMENT 0x00

// Flags para Display / Cursor Control
#define LCD_DISPLAYON  0x04
#define LCD_DISPLAYOFF 0x00
#define LCD_CURSORON   0x02
#define LCD_CURSOROFF  0x00
#define LCD_BLINKON    0x01
#define LCD_BLINKOFF   0x00

// Flags para Shift de Display y Cursor
#define LCD_DISPLAYMOVE 0x08
#define LCD_CURSORMOVE  0x00
#define LCD_MOVERIGHT   0x04
#define LCD_MOVELEFT    0x00

// Flags para Function Set
#define LCD_8BITMODE 0x10
#define LCD_4BITMODE 0x00
#define LCD_2LINE    0x08
#define LCD_1LINE    0x00
#define LCD_5x10DOTS 0x04
#define LCD_5x8DOTS  0x00

// ============================================================
// Normalización de pines (acepta distintas nomenclaturas)
// ============================================================
#if !defined(LCD_DB4) && defined(LCD_DATA4)
   #define LCD_DB4 LCD_DATA4
#endif
#if !defined(LCD_DB5) && defined(LCD_DATA5)
   #define LCD_DB5 LCD_DATA5
#endif
#if !defined(LCD_DB6) && defined(LCD_DATA6)
   #define LCD_DB6 LCD_DATA6
#endif
#if !defined(LCD_DB7) && defined(LCD_DATA7)
   #define LCD_DB7 LCD_DATA7
#endif

#if !defined(LCD_RS) && defined(LCD_RS_PIN)
   #define LCD_RS LCD_RS_PIN
#endif

#if !defined(LCD_E) && defined(LCD_EN)
   #define LCD_E LCD_EN
#elif !defined(LCD_E) && defined(LCD_ENABLE_PIN)
   #define LCD_E LCD_ENABLE_PIN
#endif

#if !defined(LCD_RW) && defined(LCD_RW_PIN)
   #define LCD_RW LCD_RW_PIN
#endif

// Asignación por defecto en PORTD (PIC16F877A / PIC18F4550) si no se definieron
#ifndef LCD_DB4
   #define LCD_DB4 PIN_D4
#endif
#ifndef LCD_DB5
   #define LCD_DB5 PIN_D5
#endif
#ifndef LCD_DB6
   #define LCD_DB6 PIN_D6
#endif
#ifndef LCD_DB7
   #define LCD_DB7 PIN_D7
#endif
#ifndef LCD_RS
   #define LCD_RS  PIN_D2
#endif
#ifndef LCD_E
   #define LCD_E   PIN_D3
#endif

// ============================================================
// Prototipos de funciones principales
// ============================================================

// Nivel bajo
void lcd_send_nibble(unsigned int8 nibble);
void lcd_send_byte(unsigned int8 address, unsigned int8 n);
void lcd_command(unsigned int8 cmd);
void lcd_write(unsigned int8 val);

// Inicialización
void lcd_init(void);
void lcd_begin(unsigned int8 cols, unsigned int8 lines);

// Posicionamiento
void lcd_set_cursor(unsigned int8 col, unsigned int8 row); // 0-indexed (estilo Arduino)
void lcd_gotoxy(unsigned int8 x, unsigned int8 y);          // 1-indexed (estilo CCS C)

// Control de pantalla y cursor
void lcd_clear(void);
void lcd_home(void);
void lcd_display(void);
void lcd_no_display(void);
void lcd_cursor(void);
void lcd_no_cursor(void);
void lcd_blink(void);
void lcd_no_blink(void);
void lcd_scroll_left(void);
void lcd_scroll_right(void);
void lcd_left_to_right(void);
void lcd_right_to_left(void);
void lcd_autoscroll(void);
void lcd_no_autoscroll(void);

// Caracteres personalizados en CGRAM (0 a 7)
void lcd_create_char(unsigned int8 location, unsigned int8 *charmap);

// Emisión básica de caracteres con secuencias de escape ('\f', '\n', '\b')
void lcd_putc(char c);

// Impresión de cadenas de texto
void lcd_puts(char *s);
#define lcd_puts_const(str)       lcd_puts(str)
#define lcd_print_str(str)        lcd_puts(str)

// Impresión de números (Equivalente al motor Print de Arduino)
void lcd_print_uint(unsigned int16 n, unsigned int8 base);
void lcd_print_int(signed int16 n, unsigned int8 base);
void lcd_print_float(float val, unsigned int8 digits);

// Funciones con salto de línea (estilo println)
void lcd_println(void);
void lcd_println_str(char *s);
#define lcd_println_const(str)    do { printf(lcd_putc, str); lcd_println(); } while(0)
void lcd_println_int(signed int16 n, unsigned int8 base);
void lcd_println_float(float val, unsigned int8 digits);

// ============================================================
// Macros de conveniencia estilo Arduino
// ============================================================
#define lcd_print_dec(n)         lcd_print_int((signed int16)(n), DEC)
#define lcd_print_hex(n)         lcd_print_uint((unsigned int16)(n), HEX)
#define lcd_print_bin(n)         lcd_print_uint((unsigned int16)(n), BIN)
#define lcd_print_oct(n)         lcd_print_uint((unsigned int16)(n), OCT)
#define lcd_print_flt(val, dec)  lcd_print_float((float)(val), (dec))

// Inclusión automática de la implementación para CCS C
#ifndef __LCD_FLEX_C__
   #include "lcd_flex.c"
#endif

#endif // __LCD_FLEX_H__
