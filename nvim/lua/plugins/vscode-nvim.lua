return {
  -- use vscode color scheme
  "Mofiqul/vscode.nvim",
  opts = {
    style = "dark",
    italic_comments = true,
    underline_links = true,
    disable_nvimtree_bg = true,
    transparent = true,
  },
  init = function()
    vim.cmd.colorscheme("vscode")
  end,
}
