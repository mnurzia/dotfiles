#!/bin/sh

# default directories (define these outside to override)
DOTFILES_DIR=${DOTFILES_DIR:-~/.config/dotfiles}
PASS_DIR=${PASS_DIR:-~/.password-store}
NOTES_DIR=${NOTES_DIR:-~/Documents/notes}

# install custom binaries into PATH
case :$PATH:
  in 
  *:$DOTFILES_DIR/bin:*) # already added to path
    ;;
  *) 
    PATH="$PATH:$DOTFILES_DIR/bin";;
esac

# reload/rebuild configuration
alias reload='source $DOTFILES_DIR/source.sh'
alias remake='make -C $DOTFILES_DIR; reload'

# clear screen
alias cls="clear"

# start a root login shell (- argument)
alias ssu="sudo su -"

# Alternative to tset(1) without archaic delay
reset() {
  # https://unix.stackexchange.com/questions/335648/why-does-the-reset-command-include-a-delay
  stty sane
  tput reset
}

# kill all background processes
reap() {
  for job in $(jobs -l | awk '/(S|s)(topped|uspended)/ { print $2 }'); do
    kill -9 "$job"
  done
  # give prompt a moment to catch up (otherwise SIGCHILD doesn't hit bash til' after prompt)
  sleep 0.1
}

# inspect parts of $PATH
showpath() {
  echo "$PATH" | sed 's/:/\n/g'
}

# ls aliases
alias ls="ls --color=auto --classify=auto"
alias l="ls"
alias ll="ls -l"
alias lal="ls -al"
alias la="ls -a"

# cd aliases
alias up="cd .."

# cd into the most recently modified directory (useful for grading)
recent () {
  cd "$(find . -mindepth 1 -maxdepth 1 -type d -printf "%T+\t%p\n" | sort | head -n1 | cut -s -f2)" || return
}

# generate a random port number for a network service
randport () {
  shuf -i 1024-49151 | head -n1
}

# git aliases
alias gp="git push"
alias ga="git add"
alias gc="git commit"
alias gs="git status"

# editor modifications
# if command -v nvim > /dev/null 2>&1
# then
#   alias nano="nvim"
#   export EDITOR=nvim
#   export GIT_EDITOR=nvim
# fi

export EDITOR=vim

# vscode integration
alias vsc=code
vsc_pipe () {
  tmp=$(mktemp)
  cat -- > "$tmp"
  vsc "$tmp"
  # give vsc a cuppa seconds to load it up
  sleep 1.5
  rm -rf "$tmp"
}

# make history infinite
export HISTSIZE=
export HISTFILESIZE=

# inputrc modifications
export INPUTRC="$DOTFILES_DIR/inputrc"

# pass mods
export PASSWORD_STORE_ENABLE_EXTENSIONS=true
passhow() {
  pass show "$(find ~/.password-store -name '*.gpg' | sed -e "s:$PASS_DIR/::gi" -e "s:.gpg$::gi" | fzf)"
}
alias passshow=passhow

# note taking
notes() {
  $EDITOR "$(find "$NOTES_PATH" -mindepth 1 -maxdepth 1 | fzf)"
}

# git setup
git config --global user.email "7797957+mnurzia@users.noreply.github.com"

# prompt setup
PROMPT_COMMAND=_prompt
_prompt () {
  _PROMPT_EXIT_STATUS=$?
  history -a
  PS1="$("$DOTFILES_DIR"/bin/prompt -x $_PROMPT_EXIT_STATUS -j "$(jobs | wc -l)")"
}

# pythonrc
export PYTHONSTARTUP="$DOTFILES_DIR/pythonrc"

# open man page in browser
bman () (
  MAN_PAGE="$(man -w " $1 ")"
  case "$MAN_PAGE" in
    *.gz) # need to unzip man pages ending in .gz
      FILTER="gunzip"
      ;;
    *)
      FILTER="cat"
      ;;
  esac
  MAN_TMP="$(mktemp)"
  "$FILTER" < "$MAN_PAGE" | groff -t -e -mandoc -Thtml > "$MAN_TMP"
  firefox "$MAN_TMP"
  sleep 0.5
  rm -f "$MAN_TMP"
)

# signifies correct loading
fetch
