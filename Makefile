all: bin bin/prompt bin/colors

bin:
	mkdir -p bin

bin/prompt: prompt/prompt.c
	gcc prompt/prompt.c -o bin/prompt

bin/colors: colors/colors.py
	cp colors/colors.py bin/colors
	chmod +x bin/colors

clean:
	rm -rf bin
