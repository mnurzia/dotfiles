return {
  -- documentation and help for key bindings
  "folke/which-key.nvim",
  event = "VeryLazy",
  init = function()
    vim.o.timeout = true
    vim.o.timeoutlen = 300
  end,
  config = function(_, _)
    local wk = require("which-key")
    wk.setup({
      preset = "modern",
    })
  end,
  keys = {
    { "<C-_>", "<cmd>WhichKey<cr>", desc = "which-key: Open..." },
    {
      "<C-h>",
      "<CMD>NavigatorLeft<CR>",
      desc = "Left Pane",
    },
    {
      "<C-j>",
      "<CMD>NavigatorDown<CR>",
      desc = "Down Pane",
    },
    {
      "<C-k>",
      "<CMD>NavigatorUp<CR>",
      desc = "UpPane",
    },
    {
      "<C-l>",
      "<CMD>NavigatorRight<CR>",
      desc = "Right Pane",
    },
  },
  opts = {},
}
