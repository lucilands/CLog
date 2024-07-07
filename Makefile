CFLAGS=-Wall -Wextra -std=c99



clog: demo.c bin
	gcc demo.c -o bin/clog-demo $(CFLAGS)

bin:
	mkdir $@

clean:
ifeq ($(OS),Windows_NT)
	rmdir ./bin
else
	rm -r ./bin
endif
