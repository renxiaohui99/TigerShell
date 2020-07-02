cc = gcc
target = shellexe.x
7sopath = /lib/x86_64-linux-gnu/libreadline.so.7 
8sopath = /lib/x86_64-linux-gnu/libreadline.so.8 
shellexe.x: main.c execute.c sly.c searchfile.c
	$(cc) -c ./*.c
ifeq ($(shell [ -f $(7sopath) ];echo $$?), 0)
	$(cc) -o $(target) ./*.o $(7sopath) -I -lreadline -g
else
	$(cc) -o $(target) ./*.o $(8sopath) -I -lreadline -g
endif
	rm -rf *.o
clean:
	rm -rf *.o
