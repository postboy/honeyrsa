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
	tmp = x;	//at the beginning, current result is x
	//multiply (modulo p) x and current result y-1 times
	for (i = 0; i < y-1; i++)
		tmp = (tmp*x) % p;
		
	return tmp;
}

extern int main(void)
{
	
	const uint16_t p[]=
	//different modulos p with at least 1 suitable y
	{5, 6, 7, 10, 11, 13, 14, 15, 17, 19, 21, 22, 23, 26, 29, 30, 31, 33, 34, 35, 37, 38, 39,
	41, 42, 43, 46, 47, 51, 53, 55, 57, 58, 59, 61, 62, 65, 66, 67, 69, 70, 71, 73, 74, 77, 78, 79,
	82, 83, 85, 86, 87, 89, 91, 93, 94, 95, 97};
	//different modulos p without any suitable y
	/*{4, 8, 9, 12, 16, 18, 20, 24, 25, 27, 28, 32, 36, 40, 44, 45, 48, 49, 50, 52, 54, 56,
	60, 63, 64, 68, 72, 75, 76, 80, 81, 84, 88, 90, 92, 96, 98, 99, 100};*/

	uint16_t i, j, k, y;	//cycle counters
	//flag variable that shows did we had at least one reiteration for this power and this modulo
	uint8_t flag;
	
	
	//for every element of modulo array p
	for (i = 0; i < sizeof(p)/sizeof(uint16_t); i++) {
	
		//print number of current iteration and overall number of iterations of external cycle
		printf("%i/%i\n", i+1, sizeof(p)/sizeof(uint16_t));
		
		//for different powers that we will use in current iteration [2; p]
		for (y = 2; y <= 40; y++) {
			flag = 0;	//at the beginning we don't have any reiterations
		
			//for very possible number modulo p [0; p-1]
			for (j = 0; j < p[i]; j++)
				//for every possible number more than j modulo p [j+1; p-1]
				for (k = j+1; k < p[i]; k++)
					//if j^y = k^y mod p then print information about it
					if ( pow_mod(j, y, p[i]) == pow_mod(k, y, p[i]) ) {					//always works, slow
					//if ( (uint64_t)pow(j, y) % p[i] == (uint64_t)pow(k, y) % p[i] ) {	//not always works, fast
						//printf("%i^%i = %i^%i = %llu (mod %i)\n", j, y, k, y, (uint64_t)pow(k, y) % p[i], p[i]);
						flag = 1;	//this combination is not working
						}
			
			//if there wasn't any reiterations then print information about it
			if (!flag)
				printf("p = %i, y = %i\n", p[i], y);
			}
			
		}
		
	/*
	//testing pow_mod() function
	for (k = 2; k < 10; k++)
		for (i = 2; i < 4; i++)
			for (j = 1; j < 4; j++)
				printf("%i^%i mod %i = %i\n", i, j, k, pow_mod(i, j, k));
	*/

	return 0;
}
