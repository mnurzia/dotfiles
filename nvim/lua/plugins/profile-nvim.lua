return {
  -- simple profiler (run nvim with NVIM_PROFILE=1)
  "stevearc/profile.nvim",
  config = function()
    local should_profile = os.getenv("NVIM_PROFILE")
    if should_profile then
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
        vim.keymap.set("", "<f2>", toggle_profile)
      end
      require("profile").instrument_autocmds()
      if should_profile:lower():match("^start") then
        require("profile").start("*")
      else
        require("profile").instrument("*")
      end
      vim.keymap.set("", "<f2>", toggle_profile)
    end
  end,
}
