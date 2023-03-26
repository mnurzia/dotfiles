syntax on
startinsert
map <C-a> <ESC>^
imap <C-a> <ESC>I
map <C-e> <ESC>$
imap <C-e> <ESC>A
map <C-Home> <ESC>go
imap <C-Home> <ESC>goI
map <C-End> G$
imap <C-End> <ESC>GA
map <C-k> Vd
imap <C-k> <ESC>Vdi
map <C-z> u
imap <C-z> <ESC>ui
map <C-y> u
imap <C-y> <ESC><C-r>i
nnoremap <A-Down> :m .+1<CR>==
nnoremap <A-Up> :m .-2<CR>==
inoremap <A-Down> <Esc>:m .+1<CR>==gi
inoremap <A-Up> <Esc>:m .-2<CR>==gi
vnoremap <A-Down> :m '>+1<CR>gv=gv
vnoremap <A-Up> :m '<-2<CR>gv=gv
