#ifndef PIO_H
#define PIO_H

#include <stdio.h> // for FILE obj

typedef unsigned char Wchar;
typedef char cstr;

#define PIO_CONVERSION_BUFFER_SIZE 1024

FILE* pio_fopen(const cstr *, const cstr *);
int pio_fclose(FILE *);
int pio_printf(const cstr *, ...);
int pio_fprintf(FILE *, const cstr *, ...);
int pio_putchar(int);

#endif
