#!/bin/bash

PROMPT_COMMAND=_prompt
_prompt() {
  PS1="$(~/.config/dotfiles/bin/prompt $?)"
}

# misc aliases
alias ghostscript="/usr/bin/env gs"

# ls aliases
alias ls="ls --color=auto"
alias l="ls"
alias ll="ls -l"
alias up="cd .."

# git aliases
alias gp="git push"
alias ga="git add"
alias gc="git commit"
alias gs="git status"

# custom binaries
PATH="$PATH:~/.config/dotfiles/bin"

# editor modifications
export EDITOR=nano
export GIT_EDITOR=nano

# make history infinite
HISTSIZE=-1
HISTFILESIZE=-1

# inputrc modifications
export INPUTRC=~/.config/dotfiles/inputrc
