CC=gcc
CFLAGS=--std=c89 -Wall -Wpedantic -Werror -Wextra -Wshadow -Iinclude -g
LN=ln -sfn

all: bin bin/prompt bin/colors bin/get_ip bin/get_ip6 bin/mfetch bin/name_weight bin/ascii bin/bviz bin/cprec bin/wordington bin/keyprobe bin/wgpro ~/.config/tmux ~/.config/nvim ~/.inputrc ~/.config/gdb/gdbinit ~/.config/alacritty.toml

bin:
	mkdir -p bin

bin/prompt: prompt.c include/aparse.c
	$(CC) $(CFLAGS) --std=gnu89 prompt.c include/aparse.c -o $@ 

bin/colors: colors.c
	$(CC) $(CFLAGS) colors.c -o $@ 

bin/get_ip: get_ip.sh
	cp get_ip.sh $@ 
	chmod +x $@

bin/get_ip6: get_ip6.sh
	cp get_ip6.sh $@
	chmod +x $@

bin/mfetch: mfetch.c
	$(CC) $(CFLAGS) mfetch.c -o $@

bin/name_weight: name_weight.py
	cp name_weight.py $@
	chmod +x $@

bin/ascii: ascii.c
	$(CC) $(CFLAGS) ascii.c -o $@

bin/keyprobe: keyprobe.c
	$(CC) $(CFLAGS) -std=gnu99 keyprobe.c -o $@

bin/bviz: bviz.c include/aparse.c
	$(CC) $(CFLAGS) -std=c99 bviz.c include/aparse.c -o $@

bin/cprec: cprec.c
	$(CC) $(CFLAGS) -std=c99 cprec.c -o $@

bin/wordington: wordington.py
	cp wordington.py $@
	chmod +x $@

bin/wgpro: wgpro/wgpro.py
	cp wgpro/wgpro.py $@
	chmod +x $@

~/.config/tmux: tmux/tmux.conf
	$(LN) "$$(realpath ./tmux)" $@

~/.config/nvim: nvim/init.lua
	$(LN) "$$(realpath ./nvim)" $@

~/.inputrc: inputrc
	$(LN) "$$(realpath ./inputrc)" $@

~/.config/gdb/gdbinit: gdbinit
	mkdir -p ~/.config/gdb
	$(LN) "$$(realpath ./gdbinit)" $@

~/.config/alacritty.toml: alacritty.toml
	$(LN) "$$(realpath ./alacritty.toml)" $@

compile_commands.json:
	$(MAKE) clean
	bear -- $(MAKE) 

clean:
	rm -rf bin
