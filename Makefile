clog: main.c bin
	gcc main.c -o bin/clog-test -Wall -Wextra

bin:
	mkdir $@