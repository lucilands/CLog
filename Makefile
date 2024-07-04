clog: demo.c bin
	gcc demo.c -o bin/clog-demo -Wall -Wextra -std=c99

bin:
	mkdir $@
