#!/usr/bin/env zsh

# enable vi mode
bindkey -v

# redraw prompt
_prompt_hook() {
  _PROMPT_EXIT_STATUS=$?
  PROMPT="$("$DOTFILES_DIR"/bin/prompt -x $_PROMPT_EXIT_STATUS -j "$(jobs | wc -l)" -s zsh)"
}

# add prompt as precmd hook
precmd_functions+=(_prompt_hook)

# redraw right prompt when mode changes
function zle-line-init zle-keymap-select {
  # show block/beam cursor depending on mode
  # https://unix.stackexchange.com/questions/433273/changing-cursor-style-based-on-mode-in-both-zsh-and-vim
  if [[ ${KEYMAP} == vicmd ]] ||
     [[ $1 = 'block' ]]; then
    echo -ne '\e[1 q'
  elif [[ ${KEYMAP} == main ]] ||
       [[ ${KEYMAP} == viins ]] ||
       [[ ${KEYMAP} = '' ]] ||
       [[ $1 = 'beam' ]]; then
    echo -ne '\e[5 q'
  fi
  RPS1="$("$DOTFILES_DIR"/bin/prompt -p right -s zsh -m ${KEYMAP:-viins})"
  zle reset-prompt
}

# redraw right prompt (without mode) when command is submitted
function zle-line-finish {
  RPS1=""
  zle reset-prompt
}

# redraw right prompt when interrupt is triggered
TRAPINT() {
  if zle
  then
    RPS1=""
    zle reset-prompt
  fi
  return $((128+$1))
}

# install above hooks
zle -N zle-line-init
zle -N zle-keymap-select
zle -N zle-line-finish

# minimize time to wait to disambiguate ESC keypresses
KEYTIMEOUT=1

# bind DEL to backward-delete-char
bindkey -v '^?' backward-delete-char

# save history
ZSH_DATA_DIR=${XDG_DATA_HOME:-~/.local/share}/zsh
mkdir -p $ZSH_DATA_DIR
HISTFILE=$ZSH_DATA_DIR/history
HISTSIZE=10000000
SAVEHIST=10000000
setopt EXTENDED_HISTORY   # save duration, timestamp, etc
setopt INC_APPEND_HISTORY # write to history file immediately
setopt SHARE_HISTORY      # share history between zsh sessions
setopt HIST_IGNORE_DUPS   # don't write the same line twice
setopt APPENDHISTORY      # append to history file
