-- main.lua

-- define class
local main = {}

-- init function
function main.init()
	menu.settitle("Main Menu")
end

-- draw function
function main.draw()
	menu.drawpic(0, 0, "quitpic")
	menu.setfont(2)
	menu.drawstring(64, 64, "hello world!")
end

-- return class
return main
