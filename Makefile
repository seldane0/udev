SRC=myudev.c
OBJ=myudev.o
OUT=myudev

CFLAGS=-Wall -g

$(OUT): $(OBJ)
	gcc -o $(OUT) $(OBJ)

$(OBJ): $(SRC)
	gcc $(CFLAGS) -c $(SRC)

clean:
	rm -f $(OUT) $(OBJ)
