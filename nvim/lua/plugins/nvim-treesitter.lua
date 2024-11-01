return {
  -- tree-sitter support (used by flash)
  "nvim-treesitter/nvim-treesitter",
  build = ":TSUpdate",
  opts = {},
  init = function()
    require("nvim-treesitter.configs").setup({
      -- ensure that these languages have TS binaries installed
      ensure_installed = { "c", "lua", "python", "cpp", "bash" },
      -- install them asynchronously (don't hang)
      sync_install = false,
      -- don't automatically install parsers for new langs
      auto_install = false,
      highlight = {
        -- enable syntax highlighting
        enable = true,
        -- disable buggy syntax highlighting for help filetype (2024-06-22)
        disable = { "vimdoc" },
        -- don't run vim's syntax highlighting at the same time
        additional_vim_regex_highlighting = false,
      },
      ignore_install = {},
      modules = {},
    })
  end,
}
