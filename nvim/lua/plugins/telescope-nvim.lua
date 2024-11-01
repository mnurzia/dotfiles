return {
  "nvim-telescope/telescope.nvim",
  tag = "0.1.6",
  dependencies = { "nvim-lua/plenary.nvim" },
  keys = {

    {
      "<F3>d",
      "<cmd>Telescope lsp_definitions<cr>",
      desc = "lsp: Definitions...",
    },
    {
      "<F3>r",
      "<cmd>Telescope lsp_references<cr>",
      desc = "lsp: References...",
    },
    {
      "<F3>i",
      "<cmd>Telescope lsp_incoming_calls<cr>",
      desc = "lsp: Incoming Calls...",
    },
    {
      "<F3>o",
      "<cmd>Telescope lsp_outgoing_calls<cr>",
      desc = "lsp: Outgoing Calls...",
    },
    {
      "<F3>f",
      "<cmd>Telescope find_files<cr>",
      desc = "Find Files...",
    },
    {
      "<F3>g",
      "<cmd>Telescope live_grep<cr>",
      desc = "Live Grep...",
    },
    {
      "<F4>",
      "<cmd>Telescope lsp_document_symbols<cr>",
      desc = "lsp: Document Symbols...",
    },
    {
      "<F3>b",
      "<cmd>Telescope buffers<cr>",
      desc = "Buffers...",
    },
  },
}
