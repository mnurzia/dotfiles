return {
  -- lua statusline support
  "nvim-lualine/lualine.nvim",
  dependencies = { "nvim-tree/nvim-web-devicons" },
  config = function(_, _)
    local theme = require("lualine.themes.auto")
    require("lualine").setup({
      options = {
        theme = theme,
        component_separators = {
          left = "",
          right = "", -- don't use powerline symbols
        },
        section_separators = {
          left = "",
          right = "", -- don't use powerline symbols
        },
      },
    })
  end,
}
