-- menu_main.lua

-- define class
local menu_main = {}

-- init function
function menu_main.init()
	menu.settitle("Main Menu")
end

-- draw function
function menu_main.draw()
	menu.drawpic(0, 0, "quitpic")
	menu.setfont(2)
	menu.drawstring(64, 64, "hello world!")
end

-- return class
return menu_main
