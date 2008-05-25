#include <stdint.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include "dump_xmm.c"

#define packed_byte(x) {x, x, x, x, x, x, x, x, x, x, x, x, x, x, x, x}

void print_bin(uint32_t x) {
	int i;
	for (i=0; i < 32; i++)
		if (x & (1 << i))
			putchar('1');
		else
			putchar('0');
}

#if 0
	static uint32_t mask[] = {
		/*  5 */ 0x0001f,
		/*  6 */ 0x0003f,
		/*  7 */ 0x0007f,
		/*  8 */ 0x000ff,
		/*  9 */ 0x001ff,
		/* 10 */ 0x003ff,
		/* 11 */ 0x007ff,
		/* 12 */ 0x00fff,
		/* 13 */ 0x01fff,
		/* 14 */ 0x03fff,
		/* 15 */ 0x07fff,
		/* 16 */ 0x0ffff,
		/* 17 */ 0x1ffff,
		/* 18 */ 0x3ffff,
		/* 19 */ 0x7ffff,
		/* 20 */ 0xfffff
	};
#endif


char* sse4_strstr(char* s1, int n1, char* s2, int n2) {
	char* result;
	
	// n1 = 4..20
	// n2 > 16
	asm volatile ("movdqu (%%eax), %%xmm1" : : "a" (s1));
	asm volatile ("pxor    %%xmm0, %%xmm0" : : );
	asm volatile (
		/*** initialization ****************************************************/
		// we have to save 3 registers: eax, ecx and edx
		// also strncmp needs three arguments, thus esp -= (3+3)*4 = 
		"	addl   $-24, %%esp		\n"

		// we invoke strncmp(s1+4, s2+4, n1-4) -- s1+4 and n1-4 are constant
		// across all iterations, thus stack frame can be partially initialize
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
			// load 16 bytes, we consider just 8+3 at the beggining
		"	movdqu (%%esi), %%xmm2		\n"
							
			// xmm2 - vector of L1 distances between s1 4-byte prefix
			// and sequence of eight 4-byte subvectors of xmm2
		"	mpsadbw $0, %%xmm1, %%xmm2	\n"

			// xmm2 - word become 0xffff if L1=0, 0x0000 otherwise
		"	pcmpeqw %%xmm0, %%xmm2		\n"

			// any L1=0?  if no, skip comparision inner loop
		"	ptest   %%xmm2, %%xmm0		\n"
		"	jc      1f			\n"

			/*** inner loop ************************************************/
			// comparision inner loop: convert word mask to bitmask
			"	pmovmskb %%xmm2, %%ebx		\n"
				// we are interested in **word** positions
			"	andl $0b0101010101010101, %%ebx	\n"

		"	2:					\n"
			"	bsf %%ebx, %%eax		\n"	// get next bit position
			"	jz  1f				\n"	// no bit set? exit loop
			"					\n"
			"	btr %%eax, %%ebx		\n"	// unset bit
			"	shr $1, %%eax			\n"	// divide position by 2
				
				// save registers before invoke strncmp
			"	movl  %%eax, 12(%%esp)		\n"
			"	movl  %%ecx, 16(%%esp)		\n"
			"	movl  %%edx, 20(%%esp)		\n"

				// update function argument
			"	leal 4(%%esi, %%eax), %%eax	\n"	
			"	movl  %%eax, 4(%%esp)		\n"	// s2+4

				// invoke strncmp(s1+4, s2+4, n1-4)
			"	call  strncmp			\n"
			"	test  %%eax, %%eax		\n"	// result == 0?
				
				// restore registers
			"	movl  12(%%esp), %%eax		\n"
			"	movl  16(%%esp), %%ecx		\n"
			"	movl  20(%%esp), %%edx		\n"
			"	jnz 2b				\n"

			"	leal (%%eax, %%esi), %%eax	\n"	// eax -- address
			"	jmp 4f				\n"	// of s1's first occurance

		/*** main loop prologue ************************************************/
		"1:					\n"
		"	addl $8, %%esi			\n"
		"	subl $8, %%ecx			\n"
		"	cmpl $0, %%ecx			\n"
		"	jg   0b				\n"

		"	xorl %%eax, %%eax		\n"
		"4:					\n"
		"	addl   $24, %%esp		\n"
		: "=a" (result)
		: "S" (s2),
		  "c" (n2-n1)

		: "ebx"
	);

	return result;
}

uint8_t buffer[1024*500 + 1];

void help() {
	puts("prog sse4|libc|verify iter-count string");
	puts("* len(string) = 16");
	puts("* iter-count > 0");
	exit(1);
}

int main(int argc, char* argv[]) {
	FILE* f;
	int i, k;
	int size;

	f = fopen("funny.txt", "r");
	size = fread(buffer, 1, sizeof(buffer), f);
	buffer[size] = 0;
	fclose(f);

	if (argc != 4)
		help();

	int fun, iters, n1;
	char* s1;
	if (strcasecmp("sse4", argv[1]) == 0)
		fun = 0;
	else
	if (strcasecmp("libc", argv[1]) == 0)
		fun = 1;
	else
	if (strcasecmp("verify", argv[1]) == 0)
		fun = 2;
	else
		help();

	if (atoi(argv[2]) <= 0 && (fun != 2))
		help();
	else
		iters = atoi(argv[2]);

	s1 = argv[3];
	n1 = strlen(s1);
	if ((n1 < 4))
		help();
	else
		printf("s1(%d)='%s'\n", n1, s1);

	char* r1;
	char* r2;

	switch (fun) {
		case 0:
			puts("SSE4");
			for (i=0; i < iters; i++)
				sse4_strstr(s1, n1, buffer, size);
			break;

		case 1:
			puts("Lib C");
			k = 0;
			for (i=0; i < iters; i++)
				k += (unsigned int)strstr(buffer, s1);
			break;
		
		case 2:
			puts("verify");
			r1 = strstr(buffer, s1);
			r2 = sse4_strstr(s1, n1, buffer, size);
			
			printf("LibC = %d\n", r1);
			printf("SSE4 = %d %s\n",
				r2,
				(r1 != r2) ? "FAILED!!!" : ""
			);
				
			if (r1 != r2)
				return 1;
	}


	return 0;
}
