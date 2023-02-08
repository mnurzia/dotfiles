all: bin bin/prompt bin/colors bin/get_ip

bin:
	mkdir -p bin

bin/prompt: prompt/prompt.c
	gcc prompt/prompt.c -o bin/prompt

bin/colors: colors/colors.py
	cp colors/colors.py bin/colors
	chmod +x bin/colors

bin/get_ip: get_ip/get_ip.sh
	cp get_ip/get_ip.sh bin/get_ip
	chmod +x bin/get_ip

clean:
	rm -rf bin
