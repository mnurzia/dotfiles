#!/bin/bash

PROMPT_COMMAND=_prompt
_prompt() {
  PS1="$(~/.config/dotfiles/bin/prompt $?)"
}

# misc aliases
alias ghostscript="/usr/bin/env gs"
alias cls="clear"
alias reload="source ~/.config/dotfiles/source.sh"

# ls aliases
alias ls="ls --color=auto"
alias l="ls"
alias ll="ls -l"
alias lal="ls -al"
alias up="cd .."
alias la="ls -a"

# git aliases
alias gp="git push"
alias ga="git add"
alias gc="git commit"
alias gs="git status"

# custom binaries
PATH="$PATH:~/.config/dotfiles/bin"

# editor modifications
alias nano="vim"
export EDITOR=vim
export GIT_EDITOR=vim
alias vsc=code

# make history infinite
HISTSIZE=-1
HISTFILESIZE=-1

# inputrc modifications
export INPUTRC=~/.config/dotfiles/inputrc

# signifies correct loading
fetch
