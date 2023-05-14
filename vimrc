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
nnoremap <C-k> Vd
inoremap <C-k> <ESC>Vdi
vnoremap <C-k> d

" Kill selection.
vnoremap <BS> d

" Duplicate line downwards.
nnoremap <M-S-Down> i<ESC>yyp`^j
inoremap <M-S-Down> <ESC>i<ESC>yyp`^ji

" Duplicate line upwards.
nnoremap <M-S-Up> i<ESC>yyP`^k
inoremap <M-S-Up> <ESC>i<ESC>yyP`^ki

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
inoremap <C-b> <ESC>:NvimTreeFocus<CR>
nnoremap <C-b> :NvimTreeFocus<CR>
vnoremap <C-b> :NvimTreeFocus<CR>

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
inoremap <M-!> <ESC>:2 wincmd w<CR>
nnoremap <M-!> :2 wincmd w<CR>
vnoremap <M-!> <ESC>:2 wincmd w<CR>

" Select right pane
inoremap <M-@> <ESC>:3 wincmd w<CR>
nnoremap <M-@> :3 wincmd w<CR>
vnoremap <M-@> <ESC>:3 wincmd w<CR>

" Make new windows split vertically right.
" autocmd WinNew * wincmd L

" Prevent cursor moving backwards on ESC.
inoremap <silent> <Esc> <Esc>`^

" Debugging.

" Start/continue
nnoremap <F5> :lua require'dap'.continue()<CR>
inoremap <F5> <ESC>:lua require'dap'.continue()<CR>

" Stop (F17 = S-F5)
nnoremap <F17> :OverseerRun build<CR>:lua require'dap'.close(); require'dapui'.close()<CR>:NvimTreeResize 30<CR>
inoremap <F17> <ESC>:OverseerRun build<CR>:lua require'dap'.close(); require'dapui'.close()<CR>:NvimTreeResize 30<CR>

" Step over (next)
nnoremap <F10> :lua require'dap'.step_over()<CR>
inoremap <F10> <ESC>:lua require'dap'.step_over()<CR>

" Step into
nnoremap <F11> :lua require'dap'.step_into()<CR>
inoremap <F11> <ESC>:lua require'dap'.step_into()<CR>

" Step out (F23 = S-F11)
nnoremap <F23> :lua require'dap'.step_out()<CR>
inoremap <F23> <ESC>:lua require'dap'.step_out()<CR>

" Stack up
nnoremap <F9> :lua require'dap'.up()<CR>
inoremap <F9> <ESC>:lua require'dap'.up()<CR>

" Stack down (F21 = S-F9)
nnoremap <F21> :lua require'dap'.down()<CR>
inoremap <F21> <ESC>:lua require'dap'.down()<CR>

" Togg breakpoint
nnoremap <F8> :DapToggleBreakpoint<CR>
inoremap <F8> <ESC>:DapToggleBreakpoint<CR>i

set autoindent expandtab tabstop=2 shiftwidth=2
" set colorcolumn=80
set splitright

" Show stuff in window title.
set title

" Highlight current line.
set cursorline

" Format.
nnoremap <M-F> :Format<CR>
inoremap <M-F> <ESC>:Format<CR>i
vnoremap <M-F> :Format<CR>

" Save.
nnoremap <M-C-S> :w<CR>
inoremap <M-C-S> <ESC>:w<CR>i
vnoremap <M-C-S> :w<CR>
