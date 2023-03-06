CC=gcc

all: bin bin/prompt bin/colors bin/get_ip bin/get_ip6 bin/fetch ~/.vimrc

bin:
	mkdir -p bin

bin/prompt: prompt/prompt.c
	$(CC) prompt/prompt.c -o bin/prompt

bin/colors: colors/colors.py
	cp colors/colors.py bin/colors
	chmod +x bin/colors

bin/get_ip: get_ip/get_ip.sh
	cp get_ip/get_ip.sh bin/get_ip
	chmod +x bin/get_ip

bin/get_ip6: get_ip6/get_ip6.sh
	cp get_ip6/get_ip6.sh bin/get_ip6
	chmod +x bin/get_ip6

bin/fetch: fetch/fetch.c
	$(CC) fetch/fetch.c -o bin/fetch

~/.vimrc:
	cp vimrc ~/.vimrc

clean:
	rm -rf bin
	rm -rf ~/.vimrc
