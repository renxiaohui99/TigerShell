cc = gcc
target = shellexe.x
shellexe.x: main.c execute.c sly.c searchfile.c
	$(cc) -c ./*.c
	$(cc) -o $(target) ./*.o /lib/x86_64-linux-gnu/libreadline.so.7 -I -lreadline -g
	rm -rf *.o
	
clean:
	rm -rf *.o
