syntax on

" Beginning/end of line keys.
noremap <C-a> <ESC>^
inoremap <C-a> <ESC>I

noremap <C-e> <ESC>$
inoremap <C-e> <ESC>A

" Beginning/end of text keys.
noremap <C-Home> <ESC>go
inoremap <C-Home> <ESC>goI

noremap <C-End> G$
inoremap <C-End> <ESC>GA

" Kill line.
noremap <C-k> Vd
inoremap <C-k> <ESC>Vdi

" Undo / redo.
noremap <C-z> u
inoremap <C-z> <ESC>ui

noremap <C-y> <C-r>u
inoremap <C-y> <ESC><C-r>i

" Transpose lines.
nnoremap <A-Down> :m .+1<CR>==
inoremap <A-Down> <Esc>:m .+1<CR>==gi
vnoremap <A-Down> :m '>+1<CR>gv=gv

nnoremap <A-Up> :m .-2<CR>==
inoremap <A-Up> <Esc>:m .-2<CR>==gi
vnoremap <A-Up> :m '<-2<CR>gv=gv

" Click to enter insert mode.
nnoremap <LeftMouse> <LeftMouse>i
vnoremap <LeftMouse> <LeftMouse>i

" Focus NVimTree
map <C-b> :NvimTreeFocus<CR>

" Forward word / backward word.
inoremap <M-f> <S-Right>
nnoremap <M-f> w
vnoremap <M-f> w

inoremap <M-b> <S-Left>
nnoremap <M-b> b
vnoremap <M-b> b

" Select character.
inoremap <S-Left> <ESC>vhl
nnoremap <S-Left> vhl
vnoremap <S-Left> h

inoremap <S-Right> <ESC>lvhl
nnoremap <S-Right> lvhl
vnoremap <S-Right> l

" Select word.
inoremap <M-S-Left> <ESC>vb
nnoremap <M-S-Left> vb
vnoremap <M-S-Left> b

inoremap <M-S-Right> <ESC>vw
nnoremap <M-S-Right> vw
vnoremap <M-S-Right> w

" Select line.
inoremap <S-Up> <ESC>vhk
nnoremap <S-Up> vk
vnoremap <S-Up> k

inoremap <S-Down> <ESC>vhj
nnoremap <S-Down> vj
vnoremap <S-Down> j

" Select left pane.
inoremap <C-!> <ESC>:2 wincmd w<CR>
nnoremap <C-!> :2 wincmd w<CR>
vnoremap <C-!> <ESC>:2 wincmd w<CR>

" Select right pane
inoremap <C-@> <ESC>:3 wincmd w<CR>
nnoremap <C-@> :3 wincmd w<CR>
vnoremap <C-@> <ESC>:3 wincmd w<CR>

" Make new windows split vertically right.
autocmd WinNew * wincmd L

" Prevent cursor moving backwards on ESC.
inoremap <silent> <Esc> <Esc>`^

set autoindent
