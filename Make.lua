local srcPath,includePath,vendorPath = "src","include", "vendor"
local commands = {"find " .. srcPath .. " " .. (vendorPath) .. " -name '*.cpp' -print",
                   "find " .. srcPath .. " " .. (vendorPath) .. " -name '*.c' -print"}
local libs = {"GL","GLU","glfw"}
local formatedLibs = ""
for _,lib in ipairs(libs) 
do
  formatedLibs = formatedLibs .. " -l".. lib
end
local outFiles = ""
local rn = "g++ -Wall -fsanitize=address -I" .. includePath .. " -I" ..vendorPath .. " -g -ggdb " .. formatedLibs .." "
for _,command in ipairs(commands) do
  local handle = io.popen(command)
  if not handle then
    print("Find command error")
    break
  end
  local file = handle:read()
  while file do
    local p = string.match(string.match(file, "([^/]+)$"),"([^.]+)")
    local out = "build/"..p..'.o'
    outFiles = outFiles.. out .. " "
    local k =os.execute( rn .. file  .." -c -o".. out)
    if k == nil then
      print("\27[31m" .. "        Cannot Compile          " .. "\27[0m".."\n\n\n")
      os.exit(1, true)
      end
      file = handle:read()

      end
  handle:close()
end
os.execute(rn .. outFiles .. " -o build/final")
