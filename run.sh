#########################################################################
# File Name: run.sh
# Author: mcdragon
# mail: mcdragon@126.com
# Created Time: Sun 02 Jun 2013 01:10:31 AM CST
#########################################################################
#!/bin/bash
gcc -c thread_pool.c -lpthread -Wall -g;
gcc -o tp main.c thread_pool.o -lpthread -Wall -g;
