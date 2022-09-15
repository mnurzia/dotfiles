#!/bin/bash

PROMPT_COMMAND=_prompt
_prompt() {
  PS1="$(~/.config/dotfiles/bin/prompt $?)"
}

alias ls="ls --color=auto"
alias l="ls"
alias ll="ls -l"
alias up="cd .."
