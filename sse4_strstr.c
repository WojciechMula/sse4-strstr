#include <stdint.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include "dump_xmm.c"

#define packed_byte(x) {x, x, x, x, x, x, x, x, x, x, x, x, x, x, x, x}

int Result;

void print_bin(uint32_t x) {
	int i;
	for (i=0; i < 32; i++)
		if (x & (1 << i))
			putchar('1');
		else
			putchar('0');
	
	putchar('\n');
}

int find(char* s1, int n1, char* s2, int n2) {
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
	// n1 = 4..20
	// n2 > 16
	asm volatile ("movdqu (%%eax), %%xmm1" : : "a" (s1));
	asm volatile ("pxor    %%xmm0, %%xmm0" : : );
	asm volatile (
		"0:					\n"
		"	movdqu (%%esi), %%xmm2		\n"
		"	mpsadbw $0, %%xmm1, %%xmm2	\n"
		"	pcmpeqw %%xmm0, %%xmm2		\n"
		"					\n"
//		"	pusha				\n"
//		"	pushl $1			\n"
//		"	call dump_xmm			\n"
//		"	popl %%eax			\n"
//		"	pushl $3			\n"
//		"	call dump_xmm			\n"
//		"	popl %%eax			\n"
//		"	pushl $2			\n"
//		"	call dump_xmm_word		\n"
//		"	popl %%eax			\n"
//		"	pushl $0			\n"
//		"	call dump_xmm			\n"
//		"	popl %%eax			\n"
//		"	popa				\n"
		"					\n"
			
		"	ptest   %%xmm2, %%xmm0		\n"
		"	jc      1f			\n"

		"	pmovmskb %%xmm2, %%ebx		\n"
//		"	pusha				\n"
//		"	pushl %%ebx			\n"
//		"	call print_bin			\n"
//		"	popl %%eax			\n"
//		"	popa				\n"
		"	andl $0b0101010101010101, %%ebx	\n"
		"2:\n"
		"	bsf %%ebx, %%eax		\n"
		"	jz  1f				\n"
		"					\n"
		"	btr %%eax, %%ebx		\n"
		"	shr $1, %%eax			\n"
		"	movdqu (%%esi, %%eax), %%xmm7	\n"
		"	pcmpeqb %%xmm1, %%xmm7		\n"
		"	pmovmskb %%xmm7, %%edx		\n"
		"	andl %%edi, %%edx		\n"
		"	cmpl %%edi, %%edx		\n"
		"	jne 2b				\n"
		"	jmp 4f				\n"

		"1:					\n"
		"	addl $8, %%esi			\n"
		"	subl $8, %%ecx			\n"
		"	jns  0b				\n"

		"	movl $0, Result			\n"
		"	jmp  5f				\n"
		"					\n"
		"4:					\n"
		"	addl %%esi, %%eax		\n"
		"	movl %%eax, Result		\n"
		"					\n"
		"					\n"
		"5:					\n"
		:
		: "S" (s2),
		  "D" (mask[n1-5]),
		  "c" (n2)

		: "eax", "ebx", "edx"
	);
}

uint8_t buffer[1024*200 + 1];

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
	char s1[32];
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

	n1 = strlen(argv[3]);
	if ((n1 < 4))
		help();
	else {
		if (n1 > 16) n1 = 16;
		strncpy(s1, argv[3], 16);
		s1[16] = 0;
		printf("'%s', %d chars\n", s1, n1);
	}

	switch (fun) {
		case 0:
			puts("SSE4");
			for (i=0; i < iters; i++)
				find(s1, n1, buffer, size);
			break;

		case 1:
			puts("Lib C");
			k = 0;
			for (i=0; i < iters; i++)
				k += (unsigned int)strstr(buffer, s1);
			break;
		
		case 2:
			puts("verify");
			find(s1, n1, buffer, size);
			printf("SSE4 = %d\n", Result);
			printf("LibC = %d\n", strstr(buffer, s1));
			if (Result != (int)strstr(buffer, s1))
				return 1;
	}


	return 0;
}
