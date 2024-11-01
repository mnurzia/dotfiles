return {
  "lukas-reineke/indent-blankline.nvim",
  main = "ibl",
  config = function(_, _)
    require("ibl").setup({
      indent = {
        char = "\u{258F}",
        highlight = {
          "@boolean",
          "@string",
          "@module",
          "@attribute",
        },
      },
      scope = {
        enabled = true,
        show_start = false,
        show_end = false,
      },
    })
  end,
}
