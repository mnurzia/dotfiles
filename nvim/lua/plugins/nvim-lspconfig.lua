return {
  -- autoconfigure lsp servers
  "neovim/nvim-lspconfig",
  dependencies = { "hrsh7th/cmp-nvim-lsp" },
  event = "BufEnter",
  config = function()
    local lspconfig = require("lspconfig")
    local caps = require("cmp_nvim_lsp").default_capabilities()
    lspconfig.clangd.setup({ -- clangd (C/C++)
      capabilities = caps,
    })
    lspconfig.lua_ls.setup({ -- lua_ls (Lua)
      on_init = function(client)
        local path = client.workspace_folders[1].name
        if
          vim.loop.fs_stat(path .. "/.luarc.json")
          or vim.loop.fs_stat(path .. "/.luarc.jsonc")
        then
          return
        end

        client.config.settings.Lua =
          vim.tbl_deep_extend("force", client.config.settings.Lua, {
            runtime = {
              -- Tell the language server which version of Lua you're using
              -- (most likely LuaJIT in the case of Neovim)
              version = "LuaJIT",
            },
            -- Make the server aware of Neovim runtime files
            workspace = {
              checkThirdParty = false,
              library = {
                vim.env.VIMRUNTIME,
                -- Depending on the usage, you might want to add additional paths here.
                "${3rd}/luv/library",
                -- "${3rd}/busted/library",
              },
              -- or pull in all of 'runtimepath'. NOTE: this is a lot slower
              -- library = vim.api.nvim_get_runtime_file("", true)
            },
          })
      end,
      capabilities = caps,
      settings = {
        Lua = {
          diagnostics = {
            globals = { "vim" }, -- predefined global variables
          },
        },
      },
    })
    lspconfig.pyright.setup({ -- pyright (Python)
      capabilities = caps,
    })
    lspconfig.bashls.setup({ -- bash-language-server (shell dialects)
      capabilities = caps,
    })
    lspconfig.rust_analyzer.setup({ -- rust_analyzer (rust)
      settings = {
        ["rust-analyzer"] = {
          diagnostics = {
            enable = true,
          },
        },
      },
    })
    lspconfig.cmake.setup({ capabilities = caps }) -- cmake-language-server (CMake)
    lspconfig.pbls.setup({ -- pbls (protobuf)
      capabilities = caps,
    })
    lspconfig.ocamllsp.setup({ capabilities = caps }) -- ocaml-lsp (OCaml)
    vim.cmd("highlight! LspInlayHint guibg=#303030")
    vim.lsp.inlay_hint.enable(true)
    for _, method in ipairs({
      "textDocument/diagnostic",
      "workspace/diagnostic",
    }) do
      local default_diagnostic_handler = vim.lsp.handlers[method]
      vim.lsp.handlers[method] = function(err, result, context, config)
        if err ~= nil and err.code == -32802 then
          return
        end
        return default_diagnostic_handler(err, result, context, config)
      end
    end
  end,
  keys = {
    {
      "Ds",
      function()
        vim.diagnostic.open_float()
      end,
      desc = "lsp: Open diagnostic...",
    },
    {
      "<F3>a",
      function()
        vim.lsp.buf.code_action()
      end,
      desc = "lsp: Code Action...",
    },
    {
      "<F3>h",
      function()
        vim.lsp.inlay_hint.enable(not vim.lsp.inlay_hint.is_enabled())
      end,
      desc = "lsp: Toggle Inlay Hints",
    },
    {
      "<F3>x",
      "<cmd>ClangdSwitchSourceHeader<cr>",
      desc = "clangd: Switch between source/header",
    },
  },
}
