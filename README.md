# 📟 LCD Flex para PIC (CCS C Compiler)

Driver modular de alto rendimiento para pantallas LCD HD44780 (16x2, 20x2, 16x4, 20x4) en microcontroladores PIC (PIC16, PIC18), diseñado con **paridad total respecto a la librería `LiquidCrystal` y la clase `Print` de Arduino**.

---

## 🚀 Ventajas frente al `lcd.c` y `flex_lcd.c` tradicional

1. **Paridad con Arduino**: Mismos métodos y convenciones (`set_cursor(col, row)` con base 0, `print_int` con bases `HEX`/`BIN`/`DEC`, `print_float`, etc.).
2. **Soporte de Strings sin errores**: Funciones dedicadas para cadenas en memoria RAM (`lcd_puts`) y cadenas literales en memoria ROM (`lcd_puts_const`).
3. **Cero consumo excesivo de ROM**: Formateo de enteros y flotantes implementado directamente sin recurrir a las pesadas rutinas de `sprintf` o punto flotante de CCS C.
4. **Modo 6 pines (RW a GND)**: Funciona perfectamente sin conectar el pin RW al PIC.
5. **Mapeo libre de pines**: Cualquier pin de cualquier puerto para `D4..D7`, `RS` y `EN`.
6. **Soporte de Displays 20x4 y 16x4**: Manejo automático de offsets de 4 líneas (`0x00`, `0x40`, `0x14`, `0x54`).
7. **Control completo HD44780**: Display ON/OFF, cursor ON/OFF, blink ON/OFF, autoscroll y desplazamiento de pantalla (scroll).

---

## 🔌 Conexión de Pines Recomendada

### Modo 6 pines (Recomendado):
| Pin LCD | Nombre | Conexión en PIC (Ejemplo) |
| :--- | :--- | :--- |
| 1 | VSS | GND |
| 2 | VDD | +5V |
| 3 | V0  | Potenciómetro 10k (Contraste) |
| 4 | RS  | `PIN_D2` |
| 5 | RW  | **GND** (Directo a masa) |
| 6 | E   | `PIN_D3` |
| 11 | D4 | `PIN_D4` |
| 12 | D5 | `PIN_D5` |
| 13 | D6 | `PIN_D6` |
| 14 | D7 | `PIN_D7` |
| 15 | A   | +5V (a través de resistencia 220Ω) |
| 16 | K   | GND |

---

## 📋 Tabla de Equivalencias: Arduino vs LCD Flex (PIC)

| Arduino (`LiquidCrystal` / `Print`) | LCD Flex (CCS C PIC) | Descripción |
| :--- | :--- | :--- |
| `lcd.begin(cols, rows)` | `lcd_begin(cols, rows)` o `lcd_init()` | Inicializa la pantalla con dimensiones especificadas |
| `lcd.clear()` | `lcd_clear()` | Limpia la pantalla y pone cursor en (0,0) |
| `lcd.home()` | `lcd_home()` | Regresa el cursor a (0,0) sin borrar DDRAM |
| `lcd.setCursor(col, row)` | `lcd_set_cursor(col, row)` | Posiciona cursor (Base 0: col 0..15, row 0..3) |
| *(CCS C tradicional)* | `lcd_gotoxy(x, y)` | Posiciona cursor (Base 1: x 1..16, y 1..4) |
| `lcd.write(byte)` | `lcd_write(byte)` | Envía byte crudo a DDRAM (incluye char 0) |
| `lcd.print("Hola")` | `lcd_puts_const("Hola")` | Imprime string literal desde ROM |
| `lcd.print(buffer_ram)` | `lcd_puts(buffer_ram)` | Imprime string variable desde RAM |
| `lcd.print(123)` | `lcd_print_int(123, DEC)` o `lcd_print_dec(123)` | Imprime entero con signo en base 10 |
| `lcd.print(255, HEX)` | `lcd_print_uint(255, HEX)` o `lcd_print_hex(255)` | Imprime en hexadecimal (ej. `"FF"`) |
| `lcd.print(10, BIN)` | `lcd_print_uint(10, BIN)` o `lcd_print_bin(10)` | Imprime en binario (ej. `"1010"`) |
| `lcd.print(3.1415, 2)` | `lcd_print_float(3.1415, 2)` | Imprime flotante con `N` decimales redondeados |
| `lcd.println(...)` | `lcd_println()` / `lcd_println_str(...)` | Imprime y avanza a la siguiente fila en columna 0 |
| `lcd.createChar(num, data)` | `lcd_create_char(num, data)` | Guarda un carácter personalizado (0 a 7) en CGRAM |
| `lcd.display()` / `noDisplay()` | `lcd_display()` / `lcd_no_display()` | Enciende / apaga el display |
| `lcd.cursor()` / `noCursor()` | `lcd_cursor()` / `lcd_no_cursor()` | Muestra / oculta el cursor de línea |
| `lcd.blink()` / `noBlink()` | `lcd_blink()` / `lcd_no_blink()` | Activa / desactiva parpadeo del cursor |
| `lcd.scrollDisplayLeft()` | `lcd_scroll_left()` | Desplaza toda la pantalla a la izquierda |
| `lcd.scrollDisplayRight()`| `lcd_scroll_right()` | Desplaza toda la pantalla a la derecha |
| `lcd.command(cmd)` | `lcd_command(cmd)` | Envía comando crudo de instrucción al HD44780 |

---

## 💻 Ejemplo de Uso Rápido

```c
#include <18F4550.h>
#fuses HS, NOWDT, NOLVP
#use delay(crystal = 20MHz)

// 1. Definir pines
#define LCD_DATA4  PIN_D4
#define LCD_DATA5  PIN_D5
#define LCD_DATA6  PIN_D6
#define LCD_DATA7  PIN_D7
#define LCD_RS     PIN_D2
#define LCD_EN     PIN_D3

// 2. Incluir el driver
#include "lcd_flex/lcd_flex.c"

void main(void)
{
   float voltios = 4.872;

   lcd_begin(16, 2);

   // Impresión directa
   lcd_set_cursor(0, 0);
   lcd_puts_const("Voltaje:");

   lcd_set_cursor(9, 0);
   lcd_print_float(voltios, 2); // Muestra "4.87"
   lcd_putc('V');

   lcd_set_cursor(0, 1);
   lcd_puts_const("Hex: 0x");
   lcd_print_hex(170);          // Muestra "AA"
   
   while(TRUE);
}
```
