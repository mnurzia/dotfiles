all: bin bin/prompt

bin:
	mkdir -p bin

bin/prompt:
	gcc prompt/prompt.c -o bin/prompt

clean:
	rm -rf bin
