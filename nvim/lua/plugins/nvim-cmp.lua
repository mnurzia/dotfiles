return {
  -- code completion and suggestions
  "hrsh7th/nvim-cmp",
  dependencies = {
    "hrsh7th/cmp-nvim-lsp", -- lsp symbols
    "hrsh7th/cmp-cmdline", -- command line
    "hrsh7th/cmp-buffer", -- buffer words
    "hrsh7th/cmp-omni", -- nvim's omnifunc
    "hrsh7th/cmp-nvim-lua", -- nvim lua API
    "L3MON4D3/LuaSnip",
    "saadparwaiz1/cmp_luasnip", -- luasnip snippets
    "onsails/lspkind.nvim",
    "hrsh7th/cmp-nvim-lsp-signature-help",
  },
  config = function()
    local cmp = require("cmp")
    local luasnip = require("luasnip")
    local lspkind = require("lspkind")
    local lspkind_fmt_func =
      lspkind.cmp_format({ -- use lspkind to add nice symbol icons
        mode = "symbol",
        maxwidth = 50,
        ellipsis_char = "...",
        show_labelDetails = true,
      })
    -- set completeopt for nvim-cmp
    vim.opt.completeopt = { "menu", "menuone", "noselect" }
    cmp.setup({
      snippet = {
        expand = function(args)
          -- use luasnip to expand snippets
          require("luasnip").lsp_expand(args.body)
        end,
      },
      mapping = cmp.mapping.preset.insert({
        ["<C-b>"] = cmp.mapping.scroll_docs(-4), -- scroll docs up
        ["<C-f>"] = cmp.mapping.scroll_docs(4), -- scroll docs down
        ["<C-Space>"] = cmp.mapping.complete(), -- complete unconditionally
        ["<C-e>"] = cmp.mapping.abort(), -- abort completion
        ["<CR>"] = cmp.mapping(function(fallback) -- complete snippet
          if cmp.visible() then
            if luasnip.expandable() then
              luasnip.expand()
            else
              cmp.confirm({
                select = true,
              })
            end
          else
            fallback()
          end
        end),
        ["<Tab>"] = cmp.mapping(
          function(fallback) -- use tab to move thru snippet
            if cmp.visible() then
              cmp.select_next_item()
            elseif luasnip.locally_jumpable(1) then
              luasnip.jump(1)
            else
              fallback()
            end
          end,
          { "i", "s" }
        ),

        ["<S-Tab>"] = cmp.mapping(
          function(fallback) -- use shift-tab to move back thru snippet
            if cmp.visible() then
              cmp.select_prev_item()
            elseif luasnip.locally_jumpable(-1) then
              luasnip.jump(-1)
            else
              fallback()
            end
          end,
          { "i", "s" }
        ),
      }),
      sources = cmp.config.sources(
        {
          { name = "lazydev" }, -- provide workspace lib completions
        },
        {
          { name = "nvim_lua" },
        },
        {
          {
            name = "luasnip",
            option = { use_show_condition = false, show_autosnippets = true },
          },
        },
        {
          { name = "nvim_lsp_signature_help" },
        },
        {
          { name = "nvim_lsp" },
        } --[[, {
        { name = "buffer" }, -- buffer completions have lower priority
      }--]]
      ),
      formatting = {
        fields = { "abbr", "kind", "menu" },
        expandable_indicator = true,
        format = function(entry, vim_item)
          vim_item.menu = entry.source.name
          if entry.source.name == "buffer" then
            vim_item.abbr_hl_group = "NvimTreeGitIgnoredIcon"
          end
          return lspkind_fmt_func(entry, vim_item)
        end,
      },
    })
    cmp.setup.cmdline({ "/", "?" }, {
      mapping = cmp.mapping.preset.cmdline(),
      sources = {
        { name = "buffer" },
      },
    })
    cmp.setup.cmdline(":", {
      mapping = cmp.mapping.preset.cmdline(),
      sources = cmp.config.sources({
        { name = "cmdline" },
      }),
    })
  end,
}
