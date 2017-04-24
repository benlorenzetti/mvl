#ifndef LOREN_IPSUM_H
#define LOREN_IPSUM_H

#define LOREN_IPSUM_MAX_HEADER_SIZE 4

#include <stdlib.h>
#include <assert.h>
#include <stdint.h>

#include <stdio.h>

/*  Line_16..................Introduction
 *  Line_xx.............Type Declarations
 *  Line_xx...........Function Prototypes
*/ 

/*  
 *      Loren Ipsum is set of library functions for replacing
 *  Null-terminated C-strings with length-recorded character strings.
 *  It is intended as complete replacement for <string.h>, so
 *  function names will conflict between the two.
 *  
 *      The big difference compared to C strings is also a dissonance
 *  found in English and Math: strings are big-endian but integers
 *  are stored starting at the little end, specifically at digit 0.
 *
 *        x111  x110  x101  x100  x011  x010  x001   x000
 *      +-----+-----+-----+-----+-----+-----+-----+------+  A string
 *      |  i  |  p  |  s  |  u  |  m  |     |     | 0x16 |  with value
 *      +-----+-----+-----+-----+-----+-----+-----+------+  "ipsum"
 *
 *      +-----+-----+-----+-----+-----+-----+-----+------+  A long int
 *      |  0  |  0  |  0  |  0  |  0  |  0  |  0  | 0x16 |  with value
 *      +-----+-----+-----+-----+-----+-----+-----+------+  22
 *
 *      String records are located at the lowest byte of their
 *  associated memory and contain allocation size and string length
 *  information.
 *
 *                 4th bit: highest bit indicates alloc size = 2^(n-1)
 *                 |
 *                 |       0th bit: the lowest bit(s) are interpreted
 *                 |       |               as RECORD_CONTINUES flag(s)
 *     22 = [0 0 0 1 0 1 1 0]
 *                  *--.--*
 *                     - remaining bits store string length info...
 *
 *      Yet somehow it seems dirty to visualize random access memory
 *  addressed in descending order.  And how do the bits [011] encode
 *  that strlen = 5, not [011] = 3?
 *
 *      To address this we need a little math to define the dual sets
 *  of natural numbers: those starting at 0 and those starting at 1,
 *  and we must realize that it is not a debate of "which is better",
 *  but rather whether the problem at hand is more naturally negative
 *  or positive. I take the view from DSP, that "memory is negative".
 *
 *      In a digital (digit based) system, it is most natural to use
 *  the set of unsigned integers, W = {0, 1, 2, 3, 4,...}, for
 *  arithmetic because [00000000] and [00000001] in **nary can be
 *  readily interpreted as zero and one. The formula is summation of
 *  {d*b^i} over each digit d in position i over all n digits.
 *  (where all i and d_{i} are in W and i < n and d_{i} < b, which is
 *  the base.)
 *
 *      But for a fixed number of digits, There are equivalence
 *  classes created. E.g. the integer eleven would be represented in
 *  binary W4 as 11 = [1011], but if it must be truncated to three
 *  digits then it becomes equivalent to [011] = 5. With two's
 *  complement the equivalence class can nicely represent negative
 *  numbers too. For example the equivalence class of {11 mod 2^3}
 *  includes
 *
 *      In W_base2_size3,
 *      [11] = {..., -21, -13, -5, 3, 11, 19, 27, 35, 43,...}
 *
 *      And *that* is how we visualize memory addresses in Loren Ipsum
 *  and how the bit pattern [011] is the representation for strlen = 5.
 *  The duals of the unsigned integers W are the natural numbers N,
 *  and the conversion of representations between objects of the two
 *  types which preserves interpreted value is just two's complement
 *  negation.
 *
 *             Unsigned_W3               Natural_N3   (Signed_Z3)
 *                           ~(3)+1
 *              3 = [011]  ---------->   [101] = 3      (= -3)
 *                         <----------
 *                           ~(3)+1
 *
 *          x111  x110  x101  x100  x011  x010  x001   x000   (RAM)
 *        +-----+-----+-----+-----+-----+-----+-----+------+ 
 *        {    7     ,6    ,5    ,4    ,3    ,2    ,1   ,0 } = W3
 *   N3 = {    1,    2,    3,    4,    5,    6,    7,    8 }
 *
 *      We consider all pointers to be in the set N, and give connot--
 *  ation to "type sizes" as in W, while "data lengths" are in N.
 *  Pointers point to objects from the right.
 *
 *  Finally, the notion that (size_t == uintptr_t) is not a tautology
 *  completely destroys our ability to reason about memory, and I'll
 *  ignore platforms where this does not hold.
*/

typedef struct {uintptr_t W;} W;
typedef struct {uintptr_t N;} N;

/*
 *      Loren Ipsum discriminates between two different classes of
 *  strings at compile time. "Ipstrings" are those which use
 *  dynamically allocated memory, while "ipstatics" are those whose
 *  memory is managed by someone else--either the compiler/linker/
 *  loader toolchain or by some other thread of control flow in code.
*/

typedef struct {char* ipstr;}  ipstr;
typedef struct {char* ipstat;} ipstat;


// Functions for finding the smallest possible, geometric-sized array
// 	size_needed = power2(ceiling_log2(data_length));

char floor_log2(uintptr_t x);
char ceiling_log2(uintptr_t x);
uintptr_t power2(char n);



char floor_log2(uintptr_t x) {
	char rot_count = -1;
	while(x != 0) {
		x /= 2;
		rot_count++;
	}
	return rot_count;
}

char ceiling_log2(uintptr_t x) {
	if (x == power2(floor_log2(x)))
		return floor_log2(x);
	else
		return 1 + floor_log2(x);
}

uintptr_t power2(char n) {
	// Ensure that 1 << n will not cause an overflow in uintptr type
	assert(n < 8*((char)sizeof(uintptr_t)));
	if(n < 0)
		return 0;
	else
		return 1 << n;
}

char* ctoipstat(const char* c_str) {
	static char* global_conversion_buffer;
	const char* curser;
	size_t len, size; // string length, allocation size

	// Get string length and keep pointer to end
	curser = c_str;
	while(*curser)
		curser++;
	len = curser - c_str;

	// Reallocate global buffer
	size = len + LOREN_IPSUM_MAX_HEADER_SIZE;
	global_conversion_buffer = realloc(global_conversion_buffer, size);


	int i;
	for(i = len; i > 0; i--) {
		//
	}
}

#endif
