#!/bin/bash

PROMPT_COMMAND=_prompt
_prompt() {
  PS1="$(~/.config/dotfiles/bin/prompt $?)"
}

# misc aliases
alias ghostscript="/usr/bin/env gs"
alias cls="clear"
alias reload="source ~/.config/dotfiles/source.sh"
alias hexdump="hexdump -C"
alias remake="make -C ~/.config/dotfiles; reload"

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
recent() {
  D=`_recent`
  cd $D
}

# git aliases
alias gp="git push"
alias ga="git add"
alias gc="git commit"
alias gs="git status"

# custom binaries
PATH="$PATH:~/.config/dotfiles/bin"

# editor modifications
if !command -v nvim &> /dev/null
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

# signifies correct loading
fetch
