SRC = ${wildcard source/*.c}
OBJ = ${patsubst %.c, %.o, $(SRC)}
INC = -I include
CFLAGS = -g -Wall

all : $(OBJ)
	$(CC) $(CFLAGS) -o tp-test $^ $(INC) -lpthread
$(OBJ):%.o : %.c
		 $(CC) $(CFLAGS) -c $^ $(INC) -o $@ -lpthread

clean:
	rm -f $(OBJ) tp-test

.PHONY:clean
