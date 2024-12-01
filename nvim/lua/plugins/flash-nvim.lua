return {
  -- quickly jump around using keys
  "folke/flash.nvim",
  event = "VeryLazy",
  dependencies = {
    "nvim-treesitter/nvim-treesitter",
  },
  opts = {
    modes = {
      search = {
        enabled = true,
      },
    },
    label = {
      rainbow = { -- highlight ranges with colors
        enabled = true,
        shade = 5,
      },
    },
  },
  keys = {
    {
      "\\",
      function()
        require("flash").treesitter_search()
      end,
      desc = "flash: tree-sitter search",
    },
  },
}
