#include "pio.h"
#include "piv_string.h"
#include <stdarg.h>
#include <stdlib.h>
#include <stdio.h>

#define PIO_UNIMPLEMENTED() \
do { \
    printf("[[error in %s line %d]]",__FILE__,__LINE__); \
    exit(EXIT_FAILURE); \
} while(0)


int pio_putchar(int c) {
    return putchar(c);
}

FILE* pio_fopen(const cstr* filename, const cstr* mode) {
    return fopen(filename, mode);
}
int pio_fclose(FILE* stream) {
    return fclose(stream);
}



struct pio_conspec;
struct pio_conspec pio_parse_conspec(char**);
/*  
 *  The following structures, constants, and enumerations help record
 *  the results from parsing a conversion specifier in a printf()
 *  format string.
 *
 *  Function parse_conspec() parses a substring beginning with the '%'
 *  conversion escape character, advancing the substring pointer to
 *  the last conversion character and returning the conversion_spec-
 *  structured result.
*/
struct pio_conspec {
    int field_width;
    Wchar flags;
    Wchar precision;
    Wchar obj_type;
    Wchar conversion_char;
};
static const int VARIABLE_FIELD_WIDTH = -1;
static const char VARIABLE_PRECISION = -1;
static const char LEFT_ALIGN = 1;
static const char PRINT_PLUS = 2;
static const char PLUS_SPACE = 4;
static const char PAD_ZEROS = 8;
static const char ALT_FORM = 16;
enum pio_conspec_obj_types {NO_INFO = 0, HH, H, L, LL, J, Z, T};

int pio_fprintf(FILE* stream, const cstr* fmt, ...) {
    va_list args;
    va_start(args, fmt);
    
    int n;
    struct pio_conspec conspec;
    while(*fmt) {
        n++;          // character count
        if(!(*fmt))
            return n;
        if(*fmt != '%') {
            if(EOF == fputc(*fmt, stream)) {
                fprintf(stderr, "fputc() fail = %d\n", ferror(stream));
                exit(EXIT_FAILURE);
            }
            fmt++;
            continue;
        }
        // Else *fmt == '%'; Start of Conversion Specifier
        conspec = pio_parse_conspec((char**) &fmt);
        char bufr[PIO_CONVERSION_BUFFER_SIZE];
        char* bufl = &bufr[PIO_CONVERSION_BUFFER_SIZE];
        int buf_space = PIO_CONVERSION_BUFFER_SIZE;
        Wchar *bufptr_l, *bufptr_r;
        double generic_data[4] = {0,0,0,0};
        if(conspec.conversion_char == 'S') {
            ustr arg = (ustr) va_arg(args, ustr);
            Wchar* str = ustr_get_str(&arg);
            bufptr_r = bufptr_l = bufl;
            do {
                *(--bufptr_r) = *(--str);
            } while(--buf_space && *str);
        }
        else if(conspec.conversion_char == 's') {
            Wchar* str = (Wchar*) va_arg(args, Wchar*);
            bufptr_r = bufptr_l = bufl;
            do *(--bufptr_r) = *(--str);
            while (--buf_space && *str);
        }
        switch(conspec.conversion_char) {
            case 'S':; case 's':
            break;
            case 'd':; case 'i':
                // For integers, switch on object type sizes
                switch(conspec.obj_type) {
                    bufptr_l = bufptr_r = bufr;
                    *bufptr_l++ = 0;
                    case NO_INFO:
                        ((int*) generic_data)[0] = va_arg(args, int);
                        if(((int*)generic_data)[0] >= 0)
                        do {
                            *bufptr_l++ = '0' + ((int*)generic_data)[0] % 10;
                            ((int*) generic_data)[0] /= 10;
                        } while( ( (int*)generic_data)[0] );
                        else {
                            ((int*)generic_data)[0] *= -1;
                            do {
                                *bufptr_l++ = '0' + ((int*)generic_data)[0] % 10;
                                ((int*)generic_data)[0] /= 10;
                            } while ( ((int*)generic_data)[0] );
                            // positive integers have 0 in sign position
                            *bufptr_l = '-';
                        }                    
                    default:
                        break; // object type size not implemented
                }
                // Add plus sign or space and correctly increment left bufptr
                if(conspec.flags & (PRINT_PLUS | PLUS_SPACE))
                    PIO_UNIMPLEMENTED();
                else if(*bufptr_l == 0 && conspec.flags & PRINT_PLUS)
                    *bufptr_l++ = '+';
                else if(*bufptr_l == 0 && conspec.flags & PLUS_SPACE)
                    *bufptr_l++ = ' ';
                else if(*bufptr_l == '-')
                    bufptr_l++;
                // Add Padding
            break;                
            default:
                fprintf(stderr, "In oi_fprintf(), conversion character");
                fprintf(stderr, " '%c'", conspec.conversion_char);
                fprintf(stderr, " not recognized.\n");
                exit(EXIT_FAILURE);
        }
        // Print from buffer from (bufptr_l, bufptr_r]
        if(bufptr_l > bufptr_r)
        do {
            int temp = (int)(*(--bufptr_l));
            putc(temp, stream);
        } while (bufptr_l != bufptr_r);
        // Advance fmt pointer before next iteration
        fmt++;
    }
}


/*
 *  Conversion specifier string parsing in parse_conspec() follows
 *  this 7-state machine.
 *                            _________State_Machine_Data___________
 *                                               int field_width
 *                                enum conspec_flags flags
 *                                     unsigned char precision_width
 *                             enum conspec_obj_type obj_type
 *                                     unsigned char conversion_char
 *                                             char* cursor
 *                       [1],.......,            int loop_count
 *                          : Begin :
 *                 .----<---:  '%'  :--->---+------.
 *                /         '''''''|'        \      \               *
 *               /      ___ -,+,0, |          Y      \              * 
 *              /|     /   \  #,SP |          |       +--->---.
 *             / |    /     Y......V,[2]      |        \       \
 *            /  |    '--<--:  Set  :         |         Y       \
 *           /   |          : Flags :----.    |         |        \
 *          /    | 1,2,3,  /''|''''|'     \'*'|         |         \
 *         /     | 4,...9 /   |    |       \  |         |          \
 *        /  [3],V.......Y,   |    | [4],..V..V....,    |           Y
 *       /  .-->: Literal :   +    V    : Variable :    |           |
 *      /  /    :  Width  :  /|    |    :  Width   :    |'.'        |
 *     /   \    '/'''|'''|' / |    |   /'|''''''''|'    |           |
 *    Y     \___/    | __|_V__|____|__V  |        | '.' |           |
 *    |  0,1,...9    |/  |    |    |     |  '.' ,.V.....V...,[5]    |
 *    |              V   +----)----'>----)----->: Precision :<--.   |
 *    | h,l,j,z,t,L /    |    |          |      /'''''|''''\'    )  V
 *[6],V............Y,    V    V          V     /      |     \___/   |
 *   : Object Size  :    |    |          |    /       |   *,0,1,2,  |
 *   :  Specifier   :<---)----)----------)-<-'h,l,j,  |   3,...9    /
 *   ''''''''\'''''''    |    |          |    z,t,L   |            / 
 *            \          |    |          |            |           /
 *             \         |    |          |            |          /
 *         [7],.Y........V....V..........V............V.........Y.,
 *            : d, i, x, X, u, c, s, S, f, e, E, g, G, p, n, N, % :
 *            :           Terminal Conversion Character           :
 *            '''''''''''''''''''''''''''''''''''''''''''''''''''''
*/
struct pio_conspec pio_parse_conspec(char **cursor_ptr) {
    char *cursor = *cursor_ptr;
    struct pio_conspec conversion_spec = {0,0,0,0,0};
    int loop_count;
    State_One:
        if(*cursor++ != '%') {/*error handle*/}
        loop_count = 0;
        switch(*cursor) {
            case '-':; case '+':; case '0':; case '#':; case ' ':
                goto State_Two;
            case '1':; case '2':; case '3':; case '4':; case '5':;
            case '6':; case '7':; case '8':; case '9':
                goto State_Three;
            case '*':
                goto State_Four;
            case '.':
                goto State_Five;
            case 'h':; case 'l':; case 'j':; case 'z':; case 't':;
            case 'L':
                goto State_Six;
            case 'd':; case 'i':; case 'x':; case 'X':; case 'u':;
            case 'c':; case 's':; case 'f':; case 'e':; case 'E':;
            case 'g':; case 'G':; case 'p':; case 'n':; case 'N':;
            case '%':; case 'S':
                goto State_Seven;
            default:
                break; /* error handle */
        }
    State_Two:
        if(loop_count > 5) {/*handle error*/}
        switch(*cursor) {
            case '-':
                conversion_spec.flags |= LEFT_ALIGN;
                loop_count++; cursor++; goto State_Two;
            case '+':
                conversion_spec.flags |= PRINT_PLUS;
                loop_count++; cursor++; goto State_Two;
            case ' ':
                conversion_spec.flags |= PLUS_SPACE;
                loop_count++; cursor++; goto State_Two;
            case '0':
                conversion_spec.flags |= PAD_ZEROS;
                loop_count++; cursor++; goto State_Two;
            case '#':
                conversion_spec.flags |= ALT_FORM;
                loop_count++; cursor++; goto State_Two;
            case '1':; case '2':; case '3':; case '4':; case '5':;
            case '6':; case '7':; case '8':; case '9':
                loop_count = 0; goto State_Three;
            case '*':
                loop_count = 0; goto State_Four;
            case '.':
                loop_count = 0; goto State_Five;
            case 'h':;case 'l':; case 'j':; case 'z':; case 't':;
            case 'L':
                loop_count = 0; goto State_Six;
            case 'd':; case 'i':; case 'x':; case 'X':; case 'u':;
            case 'c':; case 's':; case 'f':; case 'e':; case 'E':;
            case 'g':; case 'G':; case 'p':; case 'n':; case 'N':;
            case '%':; case 'S':
                loop_count = 0; goto State_Seven;
            default:
                break; /* error handle */   
        }
    State_Three:
        if(loop_count > 8) {/* error handle */}
        switch(*cursor) {
            case '0':; case '1':; case '2':; case '3':; case '4':;
            case '5':; case '6':; case '7':; case '8':; case '9':
                conversion_spec.field_width *= 10;
                conversion_spec.field_width += *cursor - '0';
                loop_count++; cursor++; goto State_Three;
            case '.':
                loop_count = 0; goto State_Five;
            case 'h':; case 'l':; case 'j':; case 'z':; case 't':;
            case 'L':
                loop_count = 0; goto State_Six;
            case 'd':; case 'i':; case 'x':; case 'X':; case 'u':;
            case 'c':; case 's':; case 'f':; case 'e':; case 'E':;
            case 'g':; case 'G':; case 'p':; case 'n':; case 'N':;
            case '%':; case 'S':
                loop_count = 0; goto State_Seven;
            default:
                break; /* error handle */   
        }
    State_Four:
        conversion_spec.field_width = VARIABLE_FIELD_WIDTH;
        cursor++, loop_count = 0;
        switch(*cursor) {
            case '.':
                goto State_Five;
            case 'h':; case 'l':; case 'j':; case 'z':; case 't':;
            case 'L':
                goto State_Six;
            case 'd':; case 'i':; case 'x':; case 'X':; case 'u':;
            case 'c':; case 's':; case 'f':; case 'e':; case 'E':;
            case 'g':; case 'G':; case 'p':; case 'n':; case 'N':;
            case '%':; case 'S':
                goto State_Seven;
            default:
                break; /* error handle */   
        }
    State_Five:
        if(loop_count > 10) {
            printf("state 5 loop count error\n");
            exit(EXIT_FAILURE);
        }
        switch(*cursor) {
            case '.':
                cursor++; goto State_Five;
            case '*':
                if(conversion_spec.precision) {
                    /* handle error */
                }
                conversion_spec.precision = VARIABLE_PRECISION;
                cursor++; goto State_Five;
            case '0':; case '1':; case '2':; case '3':; case '4':;
            case '5':; case '6':; case '7':; case '8':; case '9':
                if(conversion_spec.precision == VARIABLE_PRECISION) {
                    /* handle error */
                }
                conversion_spec.precision *= 10;
                conversion_spec.precision += *cursor - '0';
                loop_count++; cursor++; goto State_Five;
            case 'h':; case 'l':; case 'j':; case 'z':; case 't':;
            case 'L':
                loop_count = 0; goto State_Six;
            case 'd':; case 'i':; case 'x':; case 'X':; case 'u':;
            case 'c':; case 's':; case 'f':; case 'e':; case 'E':;
            case 'g':; case 'G':; case 'p':; case 'n':; case 'N':;
            case '%':; case 'S':
                loop_count = 0; goto State_Seven;
            default:
                printf("State 5 default error\n");
        }
    State_Six:
        switch(*cursor++) {
            case 'h':
                if(*cursor == 'h')
                    {conversion_spec.obj_type = HH; cursor++; goto State_Seven;}
                else
                    {conversion_spec.obj_type = H; goto State_Seven;}
            case 'l':
                if(*cursor == 'h')
                    {conversion_spec.obj_type = HH; cursor++; goto State_Seven;}
                else
                    {conversion_spec.obj_type = H; goto State_Seven;}
            case 'j':
                conversion_spec.obj_type = J; goto State_Seven;
            case 'z':
                conversion_spec.obj_type = Z; goto State_Seven;
            case 't':
                conversion_spec.obj_type = T; goto State_Seven;
            case 'L':
                conversion_spec.obj_type = L; goto State_Seven;
            default:
                break; // handle error
       }
    State_Seven:
        conversion_spec.conversion_char = *cursor;
        *cursor_ptr = cursor;
        return conversion_spec;
}
