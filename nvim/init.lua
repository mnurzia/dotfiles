-- set this variable to change color scheme
local colorscheme = "vscode"

-- bootstrap lazy.nvim
local lazypath = vim.fn.stdpath("data") .. "/lazy/lazy.nvim"
if not (vim.uv or vim.loop).fs_stat(lazypath) then
  vim.fn.system({
    "git",
    "clone",
    "--filter=blob:none",
    "https://github.com/folke/lazy.nvim.git",
    "--branch=stable", -- latest stable release
    lazypath,
  })
end
vim.opt.rtp:prepend(lazypath)

-- register keybindings
local function keys()
  local wk = require("which-key")

  wk.register({
    mode = "n",
    -- bind C-H to open WhichKey
    ["<C-H>"] = { "<cmd>WhichKey<cr>", "which-key: Open" },
    -- bind backslash to open tree-sitter search
    ["\\"] = {
      function()
        require("flash").treesitter_search()
      end,
      "Flash: tree-sitter search",
    },
    ["D"] = {
      ["s"] = {
        function()
          vim.diagnostic.open_float()
        end,
        "Open diagnostic popup",
      },
    },
    ["<F2>"] = {
      function()
        require("renamer").rename()
      end,
      "LSP: Rename",
    },
    ["<F4>"] = {
      "<cmd>Telescope lsp_document_symbols<cr>",
      "LSP: Go to Symbol...",
    },
  })
end

-- format on save (handled by stevearc/conform.nvim)
-- vim.cmd [[autocmd BufWritePre * lua vim.lsp.buf.format()]]

-- use two tabs
vim.cmd([[set tabstop=2]])
vim.cmd([[set shiftwidth=2]])
vim.cmd([[set expandtab]])

-- show line numbers in insert mode and relative line numbers in other modes
vim.cmd([[set number]])
vim.cmd([[set relativenumber]])
vim.cmd([[autocmd InsertEnter * :set norelativenumber]])
vim.cmd([[autocmd InsertLeave * :set relativenumber]])

-- highlight the line the cursor is on
vim.cmd([[set cursorline]])

-- always show the sign/problems column instead of only when there are problems
vim.cmd([[set signcolumn=yes]])

-- speed up python file opening by giving nvim's python provider an explicit
-- path
vim.g.python3_host_prog = "~/..pyenv/shims/python3"

-- update diagnostics while in insert mode
vim.diagnostic.config({ update_in_insert = true })

require("lazy").setup({
  {
    -- simple profiler (run nvim with NVIM_PROFILE=1)
    "stevearc/profile.nvim",
    config = function()
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
          vim.ui.input({
            prompt = "Save profile to:",
            completion = "file",
            default = "profile.json",
          }, function(filename)
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
    end,
  },
  {
    -- devicon support for nvim-tree
    "nvim-tree/nvim-web-devicons",
  },
  {
    -- lua statusline support
    "nvim-lualine/lualine.nvim",
    dependencies = { "nvim-tree/nvim-web-devicons" },
    opts = {
      options = {
        component_separators = {
          left = "",
          right = "", -- don't use powerline symbols
        },
        section_separators = {
          left = "",
          right = "", -- don't use powerline symbols
        },
      },
    },
  },
  {
    -- vscode-like tree plugin
    "nvim-tree/nvim-tree.lua",
    dependencies = { "nvim-tree/nvim-web-devicons" },
    opts = {
      git = {
        enable = true, -- enable git support
        ignore = false, -- don't hide .gitignored files
        timeout = 400,
      },
      renderer = {
        highlight_git = "all", -- dim out .gitignored files
      },
    },
    init = function()
      -- disable netrw for nvim-tree
      vim.g.loaded_netrw = 1
      vim.g.loaded_netrwPlugin = 1
    end,
  },
  {
    -- documentation and help for key bindings
    "folke/which-key.nvim",
    event = "VeryLazy",
    init = function()
      vim.o.timeout = true
      vim.o.timeoutlen = 300
      keys()
    end,
    opts = {},
  },
  {
    -- snippet engine
    "L3MON4D3/LuaSnip",
    version = "v2.*",
  },
  {
    -- symbol type icons for completions
    "onsails/lspkind.nvim",
  },
  {
    -- code completion and suggestions
    "hrsh7th/nvim-cmp",
    dependencies = {
      "L3MON4D3/LuaSnip",
      "onsails/lspkind.nvim",
      "hrsh7th/cmp-nvim-lsp-signature-help",
    },
    config = function()
      local cmp = require("cmp")
      local luasnip = require("luasnip")
      local lspkind = require("lspkind")
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
        sources = cmp.config.sources({
          { name = "nvim_lsp" }, -- provide lsp completions
          { name = "luasnip" }, -- provide luasnip completions
          { name = "nvim_lsp_signature_help" }, -- show signature help
        }, {
          { name = "buffer" }, -- buffer completions have lower priority
        }),
        formatting = {
          format = lspkind.cmp_format({ -- use lspkind to add nice symbol icons
            mode = "symbol",
            maxwidth = 50,
            ellipsis_char = "...",
            show_labelDetails = true,
          }),
        },
      })
    end,
  },
  {
    -- plugs lsp into our nvim-cmp
    "hrsh7th/cmp-nvim-lsp",
    dependencies = { "hrsh7th/nvim-cmp" },
  },
  {
    -- autoconfigure lsp servers
    "neovim/nvim-lspconfig",
    dependencies = { "hrsh7th/cmp-nvim-lsp" },
    config = function()
      require("lspconfig").clangd.setup({ -- clangd (C/C++)
        capabilities = require("cmp_nvim_lsp").default_capabilities(),
      })
      require("lspconfig").lua_ls.setup({ -- lua_ls (Lua)
        capabilities = require("cmp_nvim_lsp").default_capabilities(),
        settings = {
          Lua = {
            diagnostics = {
              globals = { "vim" }, -- predefined global variables
            },
          },
        },
      })
      require("lspconfig").pyright.setup({ -- pyright (Python)
        capabilities = require("cmp_nvim_lsp").default_capabilities(),
      })
      require("lspconfig").bashls.setup({ -- bash-language-server (shell dialects)
        capabilities = require("cmp_nvim_lsp").default_capabilities(),
      })
    end,
  },
  {
    -- show signatures when writing function arguments
    "ray-x/lsp_signature.nvim",
    event = "VeryLazy",
    enabled = false,
    opts = {
      bind = true,
      doc_lines = 0,
      padding = " ",
      max_width = 120,
      wrap = false,
      handler_opts = {
        border = "none",
      },
    },
  },
  {
    -- quickly jump around using keys
    "folke/flash.nvim",
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
  },
  {
    -- disable usage of arrow keys (force good vim ettiquette)
    "m4xshen/hardtime.nvim",
    dependencies = {
      "MunifTanjim/nui.nvim",
      "nvim-lua/plenary.nvim",
    },
    opts = {},
  },
  {
    -- use vscode color scheme
    "Mofiqul/vscode.nvim",
    opts = {
      style = "dark",
      italic_comments = true,
      underline_links = true,
      disable_nvimtree_bg = true,
      transparent = true,
    },
    init = function()
      vim.cmd.colorscheme("vscode")
    end,
  },
  {
    -- automatically insert pairs of (), [], etc.
    "windwp/nvim-autopairs",
    dependencies = { "hrsh7th/nvim-cmp" },
    event = "InsertEnter",
    config = function()
      local npairs = require("nvim-autopairs")
      npairs.setup({
        check_ts = true,
        ts_config = {},
      })
      local cmp_autopairs = require("nvim-autopairs.completion.cmp")
      local cmp = require("cmp")
      cmp.event:on("confirm_done", cmp_autopairs.on_confirm_done())
    end,
  },
  {
    -- show git status in signcolumn
    "lewis6991/gitsigns.nvim",
    opts = {},
  },
  {
    -- tree-sitter support (used by flash)
    "nvim-treesitter/nvim-treesitter",
    build = ":TSUpdate",
    opts = {},
    init = function()
      require("nvim-treesitter.configs").setup({
        -- ensure that these languages have TS binaries installed
        ensure_installed = { "c", "lua", "python" },
        -- install them asynchronously (don't hang)
        sync_install = false,
        -- don't automatically install parsers for new langs
        auto_install = false,
        highlight = {
          -- enable syntax highlighting
          enable = true,
          -- don't run vim's syntax highlighting at the same time
          additional_vim_regex_highlighting = false,
        },
      })
    end,
  },
  {
    -- show context lines at the top of screen for code blocks
    "nvim-treesitter/nvim-treesitter-context",
    dependencies = { "nvim-treesitter/nvim-treesitter" },
    opts = {
      -- only show five lines of context (usually more than enough)
      max_lines = 5,
    },
  },
  {
    -- preview markdown in firefox
    "iamcco/markdown-preview.nvim",
    cmd = { "MarkdownPreviewToggle", "MarkdownPreview", "MarkdownPreviewStop" },
    ft = { "markdown" },
    build = function()
      vim.fn["mkdp#util#install"]()
    end,
  },
  {
    -- formatter support
    "stevearc/conform.nvim",
    opts = {
      formatters_by_ft = {
        lua = { "stylua" },
        python = { "black" },
        c = { "clang-format" },
      },
      format_on_save = {
        timeout_ms = 500,
        lsp_fallback = false,
      },
    },
  },
  {
    -- show indentation level as a line
    "nvimdev/indentmini.nvim",
    opts = {
      char = "┆",
    },
    config = function(_, opts)
      require("indentmini").setup(opts)
      vim.cmd("highlight! IndentLineCurrent guifg=#999999")
      vim.cmd("highlight! link IndentLine VertSplit")
    end,
  },
  {
    -- show progress for LSP loading (good for python)
    "j-hui/fidget.nvim",
    opts = {},
  },
  {
    "nvim-telescope/telescope.nvim",
    tag = "0.1.6",
    dependencies = { "nvim-lua/plenary.nvim" },
  },
  {
    "filipdutescu/renamer.nvim",
    opts = {},
  },
}, { install = { colorscheme = { colorscheme } } })
