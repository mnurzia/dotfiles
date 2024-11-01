return {
  "filipdutescu/renamer.nvim",
  opts = {},
  dependencies = {
    "neovim/nvim-lspconfig",
  },
  keys = {
    {
      "<F2>",
      function()
        require("renamer").rename({})
      end,
      desc = "lsp: Rename",
    },
  },
}
