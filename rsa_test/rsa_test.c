//License: BSD 2-Clause

#include <stdio.h>
#include <inttypes.h>
#include <string.h>
#include <math.h>

#include <openssl/rsa.h>
#include <openssl/rand.h>
#include <openssl/err.h>
#include <openssl/conf.h>

static void error_handler(void)
{
	ERR_print_errors_fp(stderr);
	exit(1);
}

static int8_t print_uint8_array(const uint8_t *array, const uint64_t size)
{
	uint64_t i;	//cycle counter
	
	//wrong input value
	if (size < 1) {
		fprintf(stderr, "test: print_uint8_array error: size < 1\n");
		return 1;
		}
	
	for (i = 0; i < size; i++)
		printf("%i ", array[i]);
	printf("\n");
	return 0;
}

//get a number of occurences of different bytes in array
static int8_t stats_uint8_array(const unsigned char *in_array, const uint64_t size, uint64_t *stats)
{
	uint64_t i;			//cycle counter
	unsigned char elt;	//current processing element
	
	//wrong input value
	if (size < 1) {
		fprintf(stderr, "test: stats_uint8_array error: size < 1\n");
		return 1;
		}
	
	for (i = 0; i < size; i++) {
		elt = in_array[i];		//read a current element
		++stats[elt];			//increment the corresponding number in output array
		}
		
	return 0;
}

extern int main(void)
{
	
	RSA *keypair, *bfpair;	//variables for RSA algorithm
	BIGNUM *e;
	
	/*Buffer for ciphertext. Ensure the buffer is long enough for the ciphertext which may be
	longer than the plaintext, dependant on the algorithm and mode.*/
	unsigned char ciphertext[512];
	uint32_t decryptedmsg_len, ciphertext_len;		//their lengths
	
	uint32_t i;										//cycle counter
	uint64_t out_stats[256] = {}, firstbyte_stats[256] = {};
	//statistics on output data and on first bytes of messages, filled with 0's
	uint8_t orig_msg[256], decrypted_msg[256];
	FILE *fp;										//file variable
	
	//initialise the crypto library and everything we'll need--------------------------------------
	ERR_load_crypto_strings();
	OPENSSL_config(NULL);
	if (!RAND_status()) {
		fprintf(stderr, "test: RAND_status error: PRNG hasn't been seeded with enough data\n");
    	return 1;
		}

	if ( (e = BN_new()) == NULL)		//set an exponent value
		error_handler();

	if (BN_set_word(e, RSA_F4) != 1)	//RSA_F4 = 65537
		error_handler();

	if ( (keypair = RSA_new()) == NULL)	//create RSA object for original keypair
		error_handler();
	
	if ( (bfpair = RSA_new()) == NULL)	//for bruteforce keypair
		error_handler();
	
	RSA_blinding_off(keypair);			//we don't fear timing attacks in this example
	RSA_blinding_off(bfpair);
	
	if (RSA_generate_key_ex(keypair, 2048, e, NULL) != 1)	//generate original keypair
		error_handler();
		
	//create a message, encrypt and decrypt it-----------------------------------------------------
   
   	orig_msg[0] = 0;	//first byte is zero, so message will be always less than modulus
   	//then goes padding - 32 pseudorandom bytes
   	if (!RAND_bytes((unsigned char *)(orig_msg+1), 32))
		error_handler();
   	//finally the data - our array contains only one value
   	memset((unsigned char *)(orig_msg+1+32), 0, sizeof(orig_msg)-1-32);	//change current test here!
   	print_uint8_array(orig_msg, sizeof(orig_msg));	//show the message

	ciphertext_len = RSA_public_encrypt(sizeof(orig_msg), orig_msg, ciphertext, keypair, RSA_NO_PADDING);
	decryptedmsg_len = RSA_private_decrypt(ciphertext_len, ciphertext, decrypted_msg, keypair, RSA_NO_PADDING);
	
	//compare result of decryption and original array
	if ( memcmp(orig_msg, decrypted_msg, sizeof(orig_msg)) || (decryptedmsg_len != sizeof(orig_msg)) ) {
		fprintf(stderr, "test: memcmp error: orig_msg and decrypted_msg are not the same.\n");
		printf("sizeof(orig_msg) = %i, decryptedmsg_len = %i\n", sizeof(orig_msg), decryptedmsg_len);
		if (decryptedmsg_len == -1)
			error_handler();
		print_uint8_array(orig_msg, sizeof(orig_msg));
		print_uint8_array(decrypted_msg, decryptedmsg_len);
		return 1;
		}
	
	//attack test (complexity equals 2^8) with statistics collection-------------------------------
	
	memset(out_stats, 0, sizeof(out_stats));					//initialize statistics array
	for (i = 0; i < 8192; i++) {
		
		//generate new keypair for bruteforce
		if (RSA_generate_key_ex(bfpair, 2048, e, NULL) != 1)
			error_handler();
		
		//try to decrypt a ciphertext
		decryptedmsg_len = RSA_private_decrypt(ciphertext_len, ciphertext, decrypted_msg, bfpair, RSA_NO_PADDING);
		if (decryptedmsg_len != sizeof(orig_msg)) {
			fprintf(stderr, "test: error: sizeof(orig_msg) and decryptedmsg_len are not the equal.\n");
			printf("sizeof(orig_msg) = %i, decryptedmsg_len = %i\n", sizeof(orig_msg), decryptedmsg_len);
			if (decryptedmsg_len == -1)
				ERR_print_errors_fp(stderr);
			/*sometimes we get an error "3073947324:error:04065084:rsa routines:
			RSA_EAY_PRIVATE_DECRYPT:data too large for modulus:rsa_eay.c:532:"; in this case, we
			should just repeat this iteration to get correct statistics*/
			--i;
			continue;
			}

		//here we ignore that if first byte isn't 0 then key is wrong
		/*if (decrypted_msg[0] != 0) {
			printf("Wrong key!\n");
			--i;
			continue;
			}*/
		//get a statistics on current bruteforce iteration - only on data, not on padding!
	    stats_uint8_array((unsigned char *)(decrypted_msg+1+32), (decryptedmsg_len-1-32), out_stats);
	    stats_uint8_array((unsigned char *)decrypted_msg, 1, firstbyte_stats);
	    printf("%i/8192\n", i+1);
		}
	
	//write overall bruteforce statistics to file
	//try to open file 'rsa_decrypted.xls' for writing
	if ((fp = fopen("rsa_decrypted.xls", "w")) == NULL) {
		fprintf(stderr, "test: fopen error: can't open file 'rsa_decrypted.xls' for writing.\n");
	    return 1;
		}
		
	//compare actual vs. ideal distributions of output array
	if (fprintf(fp, "=CHITEST(A2:A257;B2:B257)\n") < 0) {
		fprintf(stderr, "test: fwrite error: cannot write to 'rsa_decrypted.xls' file.\n");
		if (fclose(fp) == EOF)
			perror("test: fclose error");
		return 1;
		}
	//write four columns to file: actual and ideal distributions for CHITEST
	for (i = 0; i < 256; i++) {
		/*7136 =  8192 (number of steps in brutforce) * 223 (size of each decrypted text in
		elements) / 256 (number of possible array values from 0 to 255) - expected result in
		out_stats*/
		if (fprintf(fp, "%llu\t%i\n", out_stats[i], 7136) < 0) {
			fprintf(stderr, "test: fwrite error: cannot write to 'rsa_decrypted.xls' file.\n");
			if (fclose(fp) == EOF)
				perror("test: fclose error");
			return 1;
			}
		}
		
	//close file
	if (fclose(fp) == EOF) {
		perror("test: fclose error");
		return 1;
		}
	
	//write statistics on first bytes to file
	//try to open file 'rsa_firstbyte.xls' for writing
	if ((fp = fopen("rsa_firstbyte.xls", "w")) == NULL) {
		fprintf(stderr, "test: fopen error: can't open file 'rsa_firstbyte.xls' for writing.\n");
	    return 1;
		}
		
	//compare actual vs. ideal distributions of output array
	if (fprintf(fp, "=CHITEST(A2:A257;B2:B257)\n") < 0) {
		fprintf(stderr, "test: fwrite error: cannot write to 'rsa_firstbyte.xls' file.\n");
		if (fclose(fp) == EOF)
			perror("test: fclose error");
		return 1;
		}
	//write four columns to file: actual and ideal distributions for CHITEST
	for (i = 0; i < 256; i++) {
		/*32 =  8192 (number of steps in brutforce) * 1 (size of each decrypted text in
		elements) / 256 (number of possible array values from 0 to 255) - expected result in
		out_stats*/
		if (fprintf(fp, "%llu\t%i\n", firstbyte_stats[i], 32) < 0) {
			fprintf(stderr, "test: fwrite error: cannot write to 'rsa_firstbyte.xls' file.\n");
			if (fclose(fp) == EOF)
				perror("test: fclose error");
			return 1;
			}
		}
		
	//close file
	if (fclose(fp) == EOF) {
		perror("test: fclose error");
		return 1;
		}
	
	//clean up
	RAND_cleanup();
	ERR_free_strings();
	RSA_free(keypair);
	RSA_free(bfpair);
	BN_clear_free(e);
	
	//getchar();	//for debugging purposes
	
	return 0;
}
