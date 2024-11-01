return {
  -- vscode-like tree plugin
  "nvim-tree/nvim-tree.lua",
  dependencies = { "nvim-tree/nvim-web-devicons" },
  opts = {
    git = {
      enable = true, -- enable git support
      ignore = false, -- don't hide .gitignored files
      timeout = 400,
    },
    renderer = {
      highlight_git = "all", -- dim out .gitignored files
    },
  },
  init = function()
    -- disable netrw for nvim-tree
    vim.g.loaded_netrw = 1
    vim.g.loaded_netrwPlugin = 1
  end,
}
