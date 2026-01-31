.PHONY: all clean

all: client
	./client

client: main.o
	g++ $^ -o $@

clean:
	rm -f main.o client
