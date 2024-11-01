return {
  -- automatically insert pairs of (), [], etc.
  "windwp/nvim-autopairs",
  dependencies = { "hrsh7th/nvim-cmp" },
  event = "InsertEnter",
  config = function()
    local npairs = require("nvim-autopairs")
    local Rule = require("nvim-autopairs.rule")
    local cond = require("nvim-autopairs.conds")
    npairs.setup({
      check_ts = true,
      ts_config = {},
    })
    local cmp_autopairs = require("nvim-autopairs.completion.cmp")
    local cmp = require("cmp")
    cmp.event:on("confirm_done", cmp_autopairs.on_confirm_done())
    local rule2 = function(a1, ins, a2, lang)
      npairs.add_rule(Rule(ins, ins, lang)
        :with_pair(function(opts)
          return a1 .. a2 == opts.line:sub(opts.col - #a1, opts.col + #a2 - 1)
        end)
        :with_move(cond.none())
        :with_cr(cond.none())
        :with_del(function(opts)
          local col = vim.api.nvim_win_get_cursor(0)[2]
          return a1 .. ins .. ins .. a2 == opts.line:sub(col - #a1 - #ins + 1, col + #ins + #a2) -- insert only works for #ins == 1 anyway
        end))
    end
    rule2("/", "*", "/", { "c", "cpp" })
    rule2("/*", " ", "*/", { "c", "cpp" })
    rule2("/", " ", "/", { "c", "cpp" })
    npairs.add_rule(Rule("/*", "*/", { "c", "cpp" }))
  end,
}
