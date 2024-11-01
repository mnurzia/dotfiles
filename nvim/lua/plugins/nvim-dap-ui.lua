return {
  "rcarriga/nvim-dap-ui",
  dependencies = { "mfussenegger/nvim-dap", "nvim-neotest/nvim-nio" },
  config = function()
    local dap, dapui = require("dap"), require("dapui")
    dapui.setup()
    dap.listeners.before.attach.dapui_config = function()
      vim.cmd("NvimTreeClose")
      dapui.open({ reset = true })
    end
    dap.listeners.before.launch.dapui_config = function()
      vim.cmd("NvimTreeClose")
      dapui.open({ reset = true })
    end
    dap.listeners.before.event_terminated.dapui_config = function()
      dapui.close()
      vim.cmd("NvimTreeOpen")
    end
    dap.listeners.before.event_exited.dapui_config = function()
      dapui.close()
      vim.cmd("NvimTreeOpen")
    end
  end,
}
