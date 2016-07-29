OBJ=net_link_user

$(OBJ):$(OBJ).o
	gcc $(OBJ).o -o $(OBJ)

$(OBJ).o:$(OBJ).c
	gcc -c $(OBJ).c

clean:
	rm *.o $(OBJ)
