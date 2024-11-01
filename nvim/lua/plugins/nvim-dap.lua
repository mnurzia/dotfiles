return {
  "mfussenegger/nvim-dap",
  config = function()
    local dap = require("dap")
    dap.adapters.lldb = {
      type = "executable",
      command = vim.fn.exepath("lldb-dap"),
      name = "lldb",
    }
  end,
  keys = {
    {
      "<F5>",
      function()
        require("dap").continue()
      end,
      desc = "dap: Continue",
    },
    {
      require("lib.fkey")("<S-F5>"),
      function()
        require("dap").terminate()
      end,
      desc = "dap: Terminate",
    },
    {
      "<F8>",
      function()
        require("dap").toggle_breakpoint()
      end,
      desc = "dap: Toggle Breakpoint",
    },
    {
      "<F9>",
      function()
        local dap = require("dap")
        dap.toggle_breakpoint()
        dap.continue()
        dap.toggle_breakpoint()
      end,
      desc = "dap: Run To Cursor",
    },
    {
      "<F10>",
      function()
        require("dap").step_over()
      end,
      desc = "dap: Step Over",
    },
    {
      "<F11>",
      function()
        require("dap").step_into()
      end,
      desc = "dap: Step Into",
    },
    {
      "<F12>",
      function()
        require("dap").step_out()
      end,
      desc = "dap: Step Out",
    },
  },
}
