return {
  -- snippet engine
  "L3MON4D3/LuaSnip",
  version = "v2.*",
  build = "make install_jsregexp",
  config = function(_, opts)
    local luasnip = require("luasnip")
    luasnip.setup(opts)
    local t = luasnip.text_node
    local i = luasnip.insert_node
    local r = require("luasnip.extras").rep
    luasnip.add_snippets("cpp", {
      luasnip.snippet("#guard", {
        t("#ifndef "),
        i(1, "ident"),
        t({ "", "#define " }),
        r(1),
        t({ "", "", "" }),
        i(0),
        t({ "", "", "#endif /* " }),
        r(1),
        t(" */"),
      }),
      luasnip.snippet("class", {
        t("class "),
        i(1),
        t({ " {", "" }),
        i(0),
        t({ "", "};" }),
      }),
      luasnip.snippet("namespace", {
        t("namespace "),
        i(1),
        t({ " {", "" }),
        i(0),
        t({ "", "};" }),
      }),
      luasnip.snippet("#include <", {
        t("#include <"),
        i(1),
        t(">"),
      }),
      luasnip.snippet('#include "', {
        t('#include "'),
        i(1),
        t('"'),
      }),
    })
  end,
}
