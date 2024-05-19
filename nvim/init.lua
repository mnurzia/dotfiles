-- disable netrw for nvim-tree
vim.g.loaded_netrw = 1
vim.g.loaded_netrwPlugin = 1

-- set completeopt for nvim-cmp
vim.opt.completeopt = { 'menu', 'menuone', 'noselect' }

local colorscheme = "vscode"

-- bootstrap lazy.nvim
local lazypath = vim.fn.stdpath("data") .. "/lazy/lazy.nvim"
if not (vim.uv or vim.loop).fs_stat(lazypath) then
  vim.fn.system(
    {
      "git",
      "clone",
      "--filter=blob:none",
      "https://github.com/folke/lazy.nvim.git",
      "--branch=stable", -- latest stable release
      lazypath,
    })
end
vim.opt.rtp:prepend(lazypath)

require("lazy").setup(
  {
    {
      'stevearc/profile.nvim',
      init = function()
        local should_profile = os.getenv("NVIM_PROFILE")
        if should_profile then
          require("profile").instrument_autocmds()
          if should_profile:lower():match("^start") then
            require("profile").start("*")
          else
            require("profile").instrument("*")
          end
        end

        local function toggle_profile()
          local prof = require("profile")
          if prof.is_recording() then
            prof.stop()
            vim.ui.input({ prompt = "Save profile to:", completion = "file", default = "profile.json" },
              function(filename)
                if filename then
                  prof.export(filename)
                  vim.notify(string.format("Wrote %s", filename))
                end
              end)
          else
            prof.start("*")
          end
        end
        vim.keymap.set("", "<f2>", toggle_profile)
      end
    },
    'nvim-tree/nvim-web-devicons',
    {
      'nvim-lualine/lualine.nvim',
      dependencies = { 'nvim-tree/nvim-web-devicons' },
      opts = { options = { component_separators = { left = '', right = '' }, section_separators = { left = '', right = '' } } }
    },
    {
      "nvim-tree/nvim-tree.lua",
      dependencies = { 'nvim-tree/nvim-web-devicons' },
      opts = {
        git = {
          enable = true,
          ignore = false,
          timeout = 400
        },
        renderer = {
          highlight_git = "all"
        }
      }
    },
    {
      "folke/which-key.nvim",
      event = "VeryLazy",
      init = function()
        vim.o.timeout = true
        vim.o.timeoutlen = 300
      end,
      opts = {}
    },
    {
      "L3MON4D3/LuaSnip",
      version = "v2.*"
    },
    "onsails/lspkind.nvim",
    {
      "hrsh7th/nvim-cmp",
      dependencies = { "L3MON4D3/LuaSnip", "onsails/lspkind.nvim" },
      config = function()
        local cmp = require('cmp')
        local luasnip = require('luasnip')
        local lspkind = require('lspkind')
        cmp.setup({
          snippet = {
            expand = function(args)
              require('luasnip').lsp_expand(args.body)
            end,
          },
          mapping = cmp.mapping.preset.insert({
            ['<C-b>'] = cmp.mapping.scroll_docs(-4),
            ['<C-f>'] = cmp.mapping.scroll_docs(4),
            ['<C-Space>'] = cmp.mapping.complete(),
            ['<C-e>'] = cmp.mapping.abort(),
            ['<CR>'] = cmp.mapping(function(fallback)
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

            ["<Tab>"] = cmp.mapping(function(fallback)
              if cmp.visible() then
                cmp.select_next_item()
              elseif luasnip.locally_jumpable(1) then
                luasnip.jump(1)
              else
                fallback()
              end
            end, { "i", "s" }),

            ["<S-Tab>"] = cmp.mapping(function(fallback)
              if cmp.visible() then
                cmp.select_prev_item()
              elseif luasnip.locally_jumpable(-1) then
                luasnip.jump(-1)
              else
                fallback()
              end
            end, { "i", "s" }),
          }),
          sources = cmp.config.sources({
            { name = 'nvim_lsp' },
            { name = 'snippy' },
          }, {
            { name = 'buffer' },
          }),
          formatting = {
            format = lspkind.cmp_format({
              mode = 'symbol',
              maxwidth = 50,
              ellipsis_char = '...',
              show_labelDetails = true
            })
          }
        })
      end
    },
    { "hrsh7th/cmp-nvim-lsp", dependencies = { "hrsh7th/nvim-cmp" }
    },
    {
      "neovim/nvim-lspconfig",
      dependencies = { "hrsh7th/cmp-nvim-lsp" },
      config = function()
        require("lspconfig").clangd.setup
        {
          capabilities = require('cmp_nvim_lsp').default_capabilities()
        }
        require("lspconfig").lua_ls.setup
        {
          settings = { Lua = { diagnostics = { globals = { 'vim' } }, format = { enable = true, defaultConfig = { indent_style = "space", indent_size = "2", max_line_length = "120" } } } }
        }
        require("lspconfig").pyright.setup {}
      end
    },
    {
      "ray-x/lsp_signature.nvim",
      event = "VeryLazy",
      opts = {
        bind = true,
        doc_lines = 0,
        padding = ' ',
        max_width = 120,
        wrap = false,
        handler_opts = {
          border = "none"
        },
      },
      config = function(_, opts)
        require("lsp_signature").setup(opts)
      end
    },
    {
      "folke/flash.nvim",
      opts = { modes = { search = { enabled = true } } }
    },
    { "m4xshen/hardtime.nvim", dependencies = { "MunifTanjim/nui.nvim", "nvim-lua/plenary.nvim" }, opts = {}
    },
    {
      "Mofiqul/vscode.nvim",
      opts = {
        style = 'dark',
        italic_comments = true,
        underline_links = true,
        disable_nvimtree_bg = true,
        transparent = true
      },
      init = function()
        vim.cmd.colorscheme("vscode")
      end
    },
    {
      "projekt0n/github-nvim-theme",
      lazy = false,
      priority = 1000,
      config = function()
        require('github-theme').setup({
          options = {
            styles = {
              comments = 'italic'
            }
          },
          groups = {
            all = {
              CursorLine = { bg = '#34393f' }
            }
          }
        })
        -- vim.cmd.colorscheme('github_dark_default')
      end,
    },
    { "windwp/nvim-autopairs",           event = "InsertEnter", config = true },
    { "lewis6991/gitsigns.nvim",         opts = {} },
    { "nvim-treesitter/nvim-treesitter", build = ":TSUpdate" },
    {
      "iamcco/markdown-preview.nvim",
      cmd = { "MarkdownPreviewToggle", "MarkdownPreview", "MarkdownPreviewStop" },
      ft = { "markdown" },
      build = function() vim.fn["mkdp#util#install"]() end,
    }
  },
  { install = { colorscheme = { colorscheme } }
  })


-- format on save
vim.cmd [[autocmd BufWritePre * lua vim.lsp.buf.format()]]

-- use two tabs
vim.cmd [[set tabstop=2]]
vim.cmd [[set shiftwidth=2]]
vim.cmd [[set expandtab]]
vim.cmd [[set number]]
vim.cmd [[set relativenumber]]
vim.cmd [[set cursorline]]
vim.cmd [[set signcolumn=yes]]
vim.cmd [[autocmd InsertEnter * :set norelativenumber]]
vim.cmd [[autocmd InsertLeave * :set relativenumber]]

-- bind ]g and [g to goto errors
-- vim.keymap.set("n", "]g", vim.diagnostic.goto_next)
-- vim.keymap.set("n", "[g", vim.diagnostic.goto_prev)

local wk = require("which-key")

wk.register({
  mode = 'n',
  ["<C-H>"] = { "<cmd>WhichKey<cr>", "which-key: Open" },
  ["\\"] = { function()
    require("flash").treesitter_search()
  end, "Flash: tree-sitter search" },
  ["[g"] = { function()
    vim.diagnostic.goto_next()
  end, "Go to next diagnostic" },
  ["]g"] = { function()
    vim.diagnostic.goto_prev()
  end, "Go to previous diagnostic" }
})

-- bind backslash to open flash in treesitter mode
-- vim.keymap.set("n", "\\", function()
--   require("flash").treesitter()
-- end)
