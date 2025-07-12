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
local rn = "g++ -Wall -I" .. includePath .. " -I" ..vendorPath .. " -g -ggdb " .. formatedLibs .." "
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
   file = handle:read()

  end
  handle:close()
end
os.execute(rn .. outFiles .. " -o build/final")
