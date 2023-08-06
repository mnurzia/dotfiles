#!/bin/bash

PROMPT_COMMAND=_prompt
_prompt () {
  _PROMPT_EXIT_STATUS=$?
  history -a
  PS1="$(~/.config/dotfiles/bin/prompt $_PROMPT_EXIT_STATUS)"
}

# misc aliases
alias ghostscript="/usr/bin/env gs"
alias cls="clear"
alias reload="source ~/.config/dotfiles/source.sh"
alias hexdump="hexdump"
alias remake="make -C ~/.config/dotfiles; reload"
alias ssu="sudo su -"

_reap() {
  kill -9 `jobs -ps`
}

# ls aliases
alias ls="ls --color=auto"
alias l="ls"
alias ll="ls -l"
alias lal="ls -al"
alias la="ls -a"

# cd aliases
alias up="cd .."

_recent () {
  cd $(ls -1t | head -n1);
}
alias recent=_recent

randport () {
  shuf -i 1024-49151 | head -n1
}

# git aliases
alias gp="git push"
alias ga="git add"
alias gc="git commit"
alias gs="git status"

# custom binaries
PATH="$PATH:~/.config/dotfiles/bin"

# editor modifications
if command -v nvim > /dev/null 2>&1
then
  alias nano="nvim"
  export EDITOR=nvim
  export GIT_EDITOR=nvim
fi
alias vsc=code

# make history infinite
HISTSIZE=-1
HISTFILESIZE=-1

# inputrc modifications
export INPUTRC=~/.config/dotfiles/inputrc

# pass mods
export PASSWORD_STORE_ENABLE_EXTENSIONS=true
passhow() {
  pass show $(find ~/.password-store -name '*.gpg' | sed -e "s:/Users/maxnu/.password-store/::gi" -e "s:.gpg$::gi" | fzf)
}

# note taking
notes() {
  $EDITOR $(ls -1 $NOTES_PATH | fzf)
}

# git setup
git config --global user.email "7797957+mnurzia@users.noreply.github.com"

# signifies correct loading
fetch
