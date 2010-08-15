/*
	SSE4 string search --- modification of Karp-Rabin algorithm, $Revision: 1.11 $
	
	Acceleration of strstr using SSE4 instruction MPSADBW.
	This program includes one wrapper sse4_strstr around
	following functions:

	* sse4_strstr_any - exact comparison is done with built-in
	  function strncmp.c
	* sse4_strstr_len3, see4_strstr_len4 - optimized
	  for substring of length 3 and 4 chars, no additional comparison
	  is needed
	* sse4_strstr_max20, sse4_strstr_max36 - optimized
	  for substring of length 4..20 and 20..36, exact comparision
	  is done with few assebler instructions


	Author: Wojciech Mu³a
	e-mail: wojciech_mula@poczta.onet.pl
	www:    http://0x80.pl/
	
	License: BSD
	
	initial release 27-05-2008, last update $Date: 2008-06-08 23:00:44 $
*/

#include <stdint.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <strings.h>

static uint8_t mask[][16] = {
	{0xff,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00},
	{0xff,0xff,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00},
	{0xff,0xff,0xff,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00},
	{0xff,0xff,0xff,0xff,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00},
	{0xff,0xff,0xff,0xff,0xff,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00},
	{0xff,0xff,0xff,0xff,0xff,0xff,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00},
	{0xff,0xff,0xff,0xff,0xff,0xff,0xff,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00},
	{0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00},
	{0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0x00,0x00,0x00,0x00,0x00,0x00,0x00},
	{0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0x00,0x00,0x00,0x00,0x00,0x00},
	{0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0x00,0x00,0x00,0x00,0x00},
	{0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0x00,0x00,0x00,0x00},
	{0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0x00,0x00,0x00},
	{0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0x00,0x00},
	{0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0x00},
	{0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff},
};

char* sse4_strstr_any(char* s1, int n1, char* s2, int n2);
char* sse4_strstr_len3(char* s1, int n1, char* s2, int n2);
char* sse4_strstr_len4(char* s1, int n1, char* s2, int n2);
char* sse4_strstr_max20(char* s1, int n1, char* s2, int n2);
char* sse4_strstr_max36(char* s1, int n1, char* s2, int n2);


char* sse4_strstr(char* s1, int n1, char* s2, int n2) {
	switch (n1) {
		case 0:
			return NULL;
		case 1:
			return strchr(s2, s1[1]);
		case 2:
			return strstr(s2, s1);
		case 3:
			return sse4_strstr_len3(s1, n1, s2, n2);
		case 4:
			return sse4_strstr_len4(s1, n1, s2, n2);
		case 5: case 6: case 7: case 8: case 9:
		case 10: case 11: case 12: case 13: case 14:
		case 15: case 16: case 17: case 18: case 19:
		case 20: /* 5..20 */
			return sse4_strstr_max20(s1, n1, s2, n2);
		case 21: case 22: case 23: case 24: case 25: 
		case 26: case 27: case 28: case 29: case 30: 
		case 31: case 32: case 33: case 34: case 35: 
		case 36: /* 21..36 */
			return sse4_strstr_max36(s1, n1, s2, n2);
		default:
			return sse4_strstr_any(s1, n1, s2, n2);

	}
}


char* sse4_strstr_any(char* s1, int n1, char* s2, int n2) {
	// n1 > 4, n2 > 4
	char* result;
	uint32_t dummy __attribute__((unused));
	
	__asm__ volatile ("movdqu (%%eax), %%xmm1" : : "a" (s1));
	__asm__ volatile ("pxor    %%xmm0, %%xmm0" : : );
	__asm__ volatile (
		/*** initialization ****************************************************/
		// we have to save 3 registers: eax, ecx and edx
		// also strncmp needs three arguments, thus esp -= (3+3)*4 = 
		"	addl   $-24, %%esp		\n"

		// function strncmp is invoke with argument s1+4, s2+4, n1-4 -- s1+4 and
		// n1-4 are constant across all iterations, thus stack frame
		// can be partially initialize:
		"	movl   8(%%ebp), %%eax		\n"
		"	addl         $4, %%eax		\n"
		"	movl      %%eax, 0(%%esp)	\n" // s1+4
		"					\n"
		"	movl  12(%%ebp), %%eax		\n"
		"	subl         $4, %%eax		\n"
		"	movl      %%eax, 8(%%esp)	\n" // n1-4
		"					\n"
		
		/*** main loop *********************************************************/
		"0:					\n"
			// load 16 bytes, we consider just 8+3 chars at the beggining
		"	movdqu (%%esi), %%xmm2		\n"
		"	addl $8, %%esi			\n" // advance pointer: s1 += 8
							
			// xmm2 - vector of L1 distances between s1's 4-byte prefix
			// and sequence of eight 4-byte subvectors from xmm2
		"	mpsadbw $0, %%xmm1, %%xmm2	\n"

			// xmm2 - word become 0xffff if L1=0, 0x0000 otherwise
		"	pcmpeqw %%xmm0, %%xmm2		\n"

			// any L1=0?  if no, skip comparision inner loop
		"	ptest   %%xmm2, %%xmm0		\n"
		"	jc      1f			\n"

			/*** inner loop ************************************************/
			// comparision inner loop: convert word mask to bitmask
			"	pmovmskb %%xmm2, %%edx		\n"
				// we are interested in **word** indexes
			"	andl $0b0101010101010101, %%edx	\n"

		"	2:					\n"
			"	bsf %%edx, %%eax		\n"	// get next bit position
			"	jz  1f				\n"	// no bit set? exit loop
			"					\n"
			"	btr %%eax, %%edx		\n"	// unset bit
			"	shr $1, %%eax			\n"	// divide position by 2
				
				// save registers before invoke strncmp
			"	movl  %%eax, 12(%%esp)		\n"
			"	movl  %%ecx, 16(%%esp)		\n"
			"	movl  %%edx, 20(%%esp)		\n"

				// update function argument
			"	leal -4(%%esi, %%eax), %%eax	\n"	
			"	movl  %%eax, 4(%%esp)		\n"	// s2+4

				// invoke strncmp(s1+4, s2+4, n1-4)
			"	call  strncmp			\n"
			"	test  %%eax, %%eax		\n"	// result == 0?
				
				// restore registers
			"	movl  12(%%esp), %%eax		\n"
			"	movl  16(%%esp), %%ecx		\n"
			"	movl  20(%%esp), %%edx		\n"
			"	jnz 2b				\n"

			"	leal -8(%%eax, %%esi), %%eax	\n"	// eax -- address
			"	jmp 4f				\n"	// of s1's first occurance

		/*** main loop prologue ************************************************/
		"1:					\n"
		"	subl $8, %%ecx			\n"
		"	cmpl $0, %%ecx			\n"
		"	jg   0b				\n"

		"	xorl %%eax, %%eax		\n" // s1 not found, return NULL
		"4:					\n"
		"	addl   $24, %%esp		\n" // and finally restore stack frame
		: "=a" (result),
		  "=S" (dummy),
		  "=c" (dummy)
		: "S" (s2),
		  "c" (n2-n1)
	);

	return result;
}


char* sse4_strstr_max20(char* s1, int n1, char* s2, int n2) {
	// 4 <= n1 <= 20, n2 > 4
	uint32_t dummy __attribute__((unused));
	char* result;
	
	__asm__ volatile ("movdqu (%%eax), %%xmm6" : : "a" (mask[n1-5]));
	__asm__ volatile ("movdqu (%%eax), %%xmm1" : : "a" (s1));
	__asm__ volatile ("movdqu (%%eax), %%xmm2" : : "a" (s1+4));	// xmm2 -- s1 suffix
	__asm__ volatile ("pxor    %%xmm0, %%xmm0" : : );
	__asm__ volatile (
		/*** main loop *********************************************************/
		"0:					\n"
			// load 16 bytes, MPSADBW consider just 8+3 chars at the beggining
		"	movdqu (%%esi), %%xmm7		\n"
		"	addl $8, %%esi			\n" // advance pointer: s1 += 8
							
			// xmm2 - vector of L1 distances between s1's 4-byte prefix
			// and sequence of eight 4-byte subvectors from xmm2
		"	mpsadbw $0, %%xmm1, %%xmm7	\n"

			// xmm2 - word become 0xffff if L1=0, 0x0000 otherwise
		"	pcmpeqw %%xmm0, %%xmm7		\n"

			// any L1=0?  if no, skip comparision inner loop
		"	ptest   %%xmm7, %%xmm0		\n"
		"	jc      1f			\n"

			/*** inner loop ************************************************/
			// comparision inner loop: convert word mask to bitmask
			"	pmovmskb %%xmm7, %%edx		\n"
				// we are interested in **word** positions
			"	andl $0b0101010101010101, %%edx	\n"

		"	2:					\n"
			"	bsf %%edx, %%eax		\n"	// get next bit position
			"	jz  1f				\n"	// no bit set? exit loop
			"					\n"
			"	btr %%eax, %%edx		\n"	// unset bit
			"	shr $1, %%eax			\n"	// divide position by 2
			"	movdqu -4(%%esi, %%eax), %%xmm7	\n"
			"	pcmpeqb %%xmm2, %%xmm7		\n"
			"	ptest	%%xmm6, %%xmm7		\n"
			"	jnc 2b				\n"

			"	leal -8(%%eax, %%esi), %%eax	\n"	// eax -- address
			"	jmp 4f				\n"	// of s1's first occurance

		/*** main loop prologue ************************************************/
		"1:					\n"
		"	subl $8, %%ecx			\n"
		"	cmpl $0, %%ecx			\n"
		"	jg   0b				\n"

		"	xorl %%eax, %%eax		\n" // s1 not found, return NULL
		"4:					\n"
		: "=a" (result),
		  "=S" (dummy),
		  "=c" (dummy)
		: "S" (s2),
		  "c" (n2-n1)
	);

	return result;
}



char* sse4_strstr_max36(char* s1, int n1, char* s2, int n2) {
	// 20 <= n1 <= 36, n2 > 4
	uint32_t dummy __attribute__((unused));
	char* result;
	
	__asm__ volatile ("movdqu (%%eax), %%xmm1" : : "a" (s1));
	__asm__ volatile ("movdqu (%%eax), %%xmm2" : : "a" (s1+4));		// xmm2 - s1[4:20]
	__asm__ volatile ("movdqu (%%eax), %%xmm3" : : "a" (s1+4+16));	// xmm3 - s1[20:] (suffix)
	__asm__ volatile ("movdqu (%%eax), %%xmm6" : : "a" (mask[n1-5-16]));
	__asm__ volatile ("pand    %%xmm6, %%xmm3" : : );
	__asm__ volatile ("pxor    %%xmm0, %%xmm0" : : ); // packed_byte(0x00)
	__asm__ volatile ("pcmpeqb %%xmm5, %%xmm5" : : ); // packed_byte(0xff)
	__asm__ volatile (
		/*** main loop *********************************************************/
		"0:					\n"
			// load 16 bytes, MPSADBW consider just 8+3 chars at the beggining
		"	movdqu (%%esi), %%xmm7		\n"
		"	addl $8, %%esi			\n" // advance pointer: s1 += 8
							
			// xmm2 - vector of L1 distances between s1's 4-byte prefix
			// and sequence of eight 4-byte subvectors from xmm2
		"	mpsadbw $0, %%xmm1, %%xmm7	\n"

			// xmm2 - word become 0xffff if L1=0, 0x0000 otherwise
		"	pcmpeqw %%xmm0, %%xmm7		\n"

			// any L1=0?  if no, skip comparision inner loop
		"	ptest   %%xmm7, %%xmm0		\n"
		"	jc      1f			\n"

			/*** inner loop ************************************************/
			// comparision inner loop: convert word mask to bitmask
			"	pmovmskb %%xmm7, %%edx		\n"
				// we are interested in **word** positions
			"	andl $0b0101010101010101, %%edx	\n"

		"	2:					\n"
			"	bsf %%edx, %%eax		\n"	// get next bit position
			"	jz  1f				\n"	// no bit set? exit loop
			"					\n"
			"	btr %%eax, %%edx		\n"	// unset bit
			"	shr $1, %%eax			\n"	// divide position by 2
			"	movdqu -4(%%esi, %%eax), %%xmm7	\n"
			"	movdqu 12(%%esi, %%eax), %%xmm4	\n"
			"	pand    %%xmm6, %%xmm4		\n"
			"	pcmpeqb %%xmm2, %%xmm7		\n"
			"	pcmpeqb %%xmm3, %%xmm4		\n"
			"	pand    %%xmm7, %%xmm4		\n"
			"	ptest	%%xmm5, %%xmm4		\n"
			"	jnc 2b				\n"

			"	leal -8(%%eax, %%esi), %%eax	\n"	// eax -- address
			"	jmp 4f				\n"	// of s1's first occurance

		/*** main loop prologue ************************************************/
		"1:					\n"
		"	subl $8, %%ecx			\n"
		"	cmpl $0, %%ecx			\n"
		"	jg   0b				\n"

		"	xorl %%eax, %%eax		\n" // s1 not found, return NULL
		"4:					\n"
		: "=a" (result),
		  "=S" (dummy),
		  "=c" (dummy)
		: "S" (s2),
		  "c" (n2-n1)
	);

	return result;
}


char* sse4_strstr_len4(char* s1, int n1, char* s2, int n2) {
	// n1 == 4, n2 > 4
	uint32_t dummy __attribute__((unused));
	char* result;
	
	__asm__ volatile ("movdqu (%%eax), %%xmm1" : : "a" (s1));
	__asm__ volatile ("pxor    %%xmm0, %%xmm0" : : );
	__asm__ volatile (
		/*** main loop *********************************************************/
		"0:					\n"
			// load 16 bytes, we consider just 8+3 chars at the beggining
		"	movdqu (%%esi), %%xmm2		\n"
		"	addl $8, %%esi			\n" // advance pointer: s1 += 8
							
			// xmm2 - vector of L1 distances between s1's 4-byte prefix
			// and sequence of eight 4-byte subvectors from xmm2
		"	mpsadbw $0, %%xmm1, %%xmm2	\n"

			// xmm2 - word become 0xffff if L1=0, 0x0000 otherwise
		"	pcmpeqw %%xmm0, %%xmm2		\n"

			// any L1=0?  if no, skip comparision inner loop
		"	ptest   %%xmm2, %%xmm0		\n"
		"	jnc     1f			\n"

		"	subl $8, %%ecx			\n"
		"	cmpl $0, %%ecx			\n"
		"	jg   0b				\n"

		"	xorl %%eax, %%eax		\n" // s1 not found, return NULL
		"	jmp  2f				\n"

		"1:					\n"
		"	pmovmskb %%xmm2, %%eax		\n"
		"	bsfl      %%eax, %%eax		\n"
		"	shrl         $1, %%eax		\n"
		"	lea -8(%%esi, %%eax), %%eax	\n"
		"2:					\n"
		: "=a" (result),
		  "=S" (dummy),
		  "=c" (dummy)
		: "S" (s2),
		  "c" (n2-n1)
	);

	return result;
}


char* sse4_strstr_len3(char* s1, int n1, char* s2, int n2) {
	// n1 == 4, n2 > 4
	uint32_t dummy __attribute__((unused));
	char* result;
	
	__asm__ volatile ("movdqu (%%eax), %%xmm1" : : "a" (s1));
	__asm__ volatile ("pxor    %%xmm0, %%xmm0" : : );
	__asm__ volatile (
		/*** main loop *********************************************************/
		"0:					\n"
			// load 16 bytes, we consider just 8+3 chars at the beggining
		"	movdqu (%%esi), %%xmm2		\n"
		"	addl $8, %%esi			\n" // advance pointer: s1 += 8
		"	movdqa  %%xmm2, %%xmm3		\n"
		"	psrldq      $3, %%xmm3		\n"
		"	pmovzxbw %%xmm3, %%xmm3		\n"
							
			// xmm2 - vector of L1 distances between s1's 4-byte prefix
			// and sequence of eight 4-byte subvectors from xmm2
		"	mpsadbw $0, %%xmm1, %%xmm2	\n"
		"	psubw   %%xmm3, %%xmm2		\n"

			// xmm2 - word become 0xffff if L1=0, 0x0000 otherwise
		"	pcmpeqw %%xmm0, %%xmm2		\n"

			// any L1=0?  if no, skip comparision inner loop
		"	ptest   %%xmm2, %%xmm0		\n"
		"	jnc     1f			\n"

		"	subl $8, %%ecx			\n"
		"	cmpl $0, %%ecx			\n"
		"	jg   0b				\n"

		"	xorl %%eax, %%eax		\n" // s1 not found, return NULL
		"	jmp  2f				\n"

		"1:					\n"
		"	pmovmskb %%xmm2, %%eax		\n"
		"	bsfl      %%eax, %%eax		\n"
		"	shrl         $1, %%eax		\n"
		"	lea -8(%%esi, %%eax), %%eax	\n"
		"2:					\n"
		: "=a" (result),
		  "=S" (dummy),
		  "=c" (dummy)
		: "S" (s2),
		  "c" (n2-n1)
	);

	return result;
}


// sample
uint8_t buffer[1024*500 + 1];


void help() {
	puts("prog file sse4|libc|verify iter-count string");
	puts("* iter-count > 0");
	exit(1);
}


int main(int argc, char* argv[]) {
	FILE* f;
	int i;
	int size;

	if (argc != 5)
		help();

	f = fopen(argv[1], "r");
	if (!f) {
		printf("can't open '%s'\n", argv[1]);
		return 2;
	}
		
	size = fread(buffer, 1, sizeof(buffer), f);
	buffer[size] = 0;
	fclose(f);

	int fun = -1, iters, n1;
	char* s1;
	if (strcasecmp("sse4", argv[2]) == 0)
		fun = 0;
	else
	if (strcasecmp("libc", argv[2]) == 0)
		fun = 1;
	else
	if (strcasecmp("verify", argv[2]) == 0)
		fun = 2;
	else
		help();

	if (atoi(argv[3]) <= 0 && (fun != 2))
		help();
	else
		iters = atoi(argv[3]);

	s1 = argv[4];
	n1 = strlen(s1);
	if ((n1 < 3))
		help();
	else
		printf("s1(%d)='%s' s2(%d)\n", n1, s1, size);

	char* r1;
	char* r2;

	switch (fun) {
		case 0:
			puts("SSE4");
			for (i=0; i < iters; i++)
				sse4_strstr(s1, n1, (char*)buffer, size);
			break;

		case 1:
			puts("Lib C");
			for (i=0; i < iters; i++) {
				//(unsigned int)strstr((char*)buffer, s1);
				__asm__ volatile (
					"movl $buffer,  (%%esp)\n"
					"movl      %0, 4(%%esp)\n"
					"call strstr\n"
					:
					: "r" (s1)
					: "eax", "ecx", "edx"
				);
			}
			break;
		
		case 2:
			puts("verify");
			r1 = strstr((char*)buffer, s1);
			r2 = sse4_strstr(s1, n1, (char*)buffer, size);
			
			printf("LibC = %u\n", (unsigned int)r1);
			printf("SSE4 = %u %s\n",
				(unsigned int)r2,
				(r1 != r2) ? "FAILED!!!" : "ok"
			);
				
			if (r1 != r2)
				return 1;
	}


	return 0;
}

// eof
