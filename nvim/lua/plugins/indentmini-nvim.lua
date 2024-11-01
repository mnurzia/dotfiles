return {
  -- show indentation level as a line
  "nvimdev/indentmini.nvim",
  opts = {
    char = "\u{258F}",
  },
  enabled = false,
  config = function(_, opts)
    require("indentmini").setup(opts)
    vim.cmd("highlight! IndentLineCurrent guifg=#999999")
    vim.cmd("highlight! link IndentLine VertSplit")
  end,
}
