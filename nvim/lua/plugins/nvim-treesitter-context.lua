return {
  -- show context lines at the top of screen for code blocks
  "nvim-treesitter/nvim-treesitter-context",
  dependencies = { "nvim-treesitter/nvim-treesitter" },
  opts = {
    -- only show five lines of context (usually more than enough)
    max_lines = 5,
  },
}
