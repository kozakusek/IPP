CC		 = gcc
OBJ		 = main.o myforests.o mystring.o mytree.o
OBJ2	 = mytree.o mystring.o 
CFLAGS	 = -Wall -Wextra -std=c11 -O2
BIN		 = forests
RM		 = rm -f

.PHONY: all clean 

all: $(BIN) 

clean:
	${RM} $(OBJ) $(BIN)

$(BIN): $(OBJ)
	$(CC) $(CFLAGS) -o $(BIN) $(OBJ)

# TODO powinno być myforests.h - w tym miejscu interesuje nas jedynie zmiana
#  pliku nagłówkowego, to samo tyczy się wszystkich pozostałych celów *.o
main.o: main.c $(OBJ2) myforests.o
	$(CC) $(CFLAGS) -c main.c -o main.o 

myforests.o: myforests.c myforests.h $(OBJ2)
	$(CC) $(CFLAGS) -c myforests.c -o myforests.o 

mytree.o: mytree.c mytree.h mystring.o
	$(CC) $(CFLAGS) -c mytree.c -o mytree.o 

mystring.o: mystring.c mystring.h
	$(CC) $(CFLAGS) -c mystring.c -o mystring.o
