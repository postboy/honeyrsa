//License: BSD 2-Clause

#include <stdio.h>
#include <inttypes.h>
#include <math.h>

//computing x^y mod p
static uint16_t pow_mod(uint16_t x, uint16_t y, uint16_t p) {

	uint16_t i, tmp;

	//any number in 0-th power is 1
	if (y == 0)
		 return 1;

	x = x % p;	//first let's take x modulo p
	tmp = x;	//at the beginning current result is x
	//multiply (modulo p) x and current result y-1 times
	for (i = 0; i < y-1; i++)
		tmp = (tmp*x) % p;
		
	return tmp;
}

extern int main(void)
{
	
	const uint16_t p[] = {2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15, 16, 17, 18, 19,
	20, 21, 22, 23, 24, 25, 26, 27, 28, 29, 30, 31, 32, 33, 34, 35, 36, 37, 38, 39,
	40, 41, 42, 43, 44, 45, 46, 47, 48, 49, 50};

	uint16_t i, x, y1, y2;	//cycle counters
	//flag variable that shows did we had at least one reiteration for this power and this modulo
	uint8_t flag;
	
	
	//for every element of modulo array p
	for (i = 0; i < sizeof(p)/sizeof(uint16_t); i++) {
	
		//print number of current iteration and overall number of iterations of external cycle
		printf("%i/%i\n", i+1, sizeof(p)/sizeof(uint16_t));
		
		//for very possible number modulo p [1; p-1]
		for (x = 1; x < p[i]; x++)  {
			flag = 0;	//at the beginning we don't have any reiterations
			
			/*
			//print numbers
			for (y1 = 0; y1 < p[i]; y1++)
				printf("%i^%i = %i (mod %i)\n", x, y1, pow_mod(x, y1, p[i]), p[i]);
			*/
			
			//for different powers that we will use in current iteration [2; p]
			for (y1 = 0; y1 < p[i]-2; y1++) {
				//for every possible power that bigger than y
				for (y2 = y1+1; y2 < p[i]-1; y2++)
					//if x^y1 = x^y2 mod p or x^y1 = 0 or x^y2 =0, then print information about it
					if ( pow_mod(x, y1, p[i]) == pow_mod(x, y2, p[i]) || !pow_mod(x, y1, p[i]) || !pow_mod(x, y2, p[i]) ) {
						//printf("%i^%i = %i^%i = %i (mod %i)\n", x, y1, x, y2, pow_mod(x, y2, p[i]), p[i]);
						flag = 1;	//this combination is not working
						}
				}
				
			//if there wasn't any reiterations then print information about it
			if (!flag)
				printf("p = %i, x = %i\n", p[i], x);
					
			}
			
		}
		

	return 0;
}
