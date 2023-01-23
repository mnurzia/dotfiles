#!/bin/bash

PROMPT_COMMAND=_prompt
_prompt() {
  PS1="$(~/.config/dotfiles/bin/prompt $?)"
}

alias ls="ls --color=auto"
alias l="ls"
alias ll="ls -l"
alias up="cd .."
alias colors=~/.config/dotfiles/bin/colors

HISTSIZE=-1
HISTFILESIZE=-1
INPUTRC=~/.config/dotfiles/inputrc
