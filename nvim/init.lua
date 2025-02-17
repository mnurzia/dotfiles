-- use two spaces for a tab
vim.cmd([[set tabstop=2]])
vim.cmd([[set shiftwidth=2]])
vim.cmd([[set expandtab]])
vim.cmd([[let no_ocaml_maps=1]])

-- show tab characters and trailing spaces
vim.cmd([[set list]])
vim.cmd([[set listchars=tab:>\ ,trail:-,extends:>,precedes:<,nbsp:+]])

-- ...but use eight for Makefiles
vim.cmd([[autocmd FileType make setlocal shiftwidth=8 tabstop=8 noexpandtab]])
vim.cmd(
  [[autocmd FileType kconfig setlocal shiftwidth=8 tabstop=8 noexpandtab]]
)

-- show line numbers in insert mode and relative line numbers in other modes
vim.cmd([[set number]])
vim.cmd([[set relativenumber]])
vim.cmd([[autocmd InsertEnter * :set norelativenumber]])
vim.cmd([[autocmd InsertLeave * :set relativenumber]])

-- highlight the line the cursor is on
vim.cmd([[set cursorline]])

-- always show the sign/problems column instead of only when there are problems
vim.cmd([[set signcolumn=yes]])

-- show an 80 column ruler
vim.cmd([[set colorcolumn=81]])

-- automatically reload files from disk if nvim hasn't changed them
vim.cmd([[set autoread]])

-- speed up python file opening by giving nvim's python provider an explicit
-- path
vim.g.python3_host_prog = "~/.pyenv/shims/python3"

-- update diagnostics while in insert mode
vim.diagnostic.config({ update_in_insert = true })

require("config.lazy")
