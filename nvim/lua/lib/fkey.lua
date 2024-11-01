-- convert a function key combination to a raw function key (S-F5 -> F17)
return function(key)
  local mods, snum = string.match(key, "<([CS]*)-F(%d+)>")
  local num = tonumber(snum)
  if string.find(mods, "S") then
    num = num + 12
  end
  if string.find(mods, "C") then
    num = num + 24
  end
  return "<F" .. tostring(num) .. ">"
end
