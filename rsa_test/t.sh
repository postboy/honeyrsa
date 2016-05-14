#!/bin/sh
#compilation and run for testing
#lcrypto is for linking against OpenSSL crypto library, Wall to see all compiler warnings
gcc rsa_test.c -o rsa_test -lcrypto -Wall
./rsa_test
