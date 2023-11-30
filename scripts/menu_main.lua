-- menu_main.lua

-- define class
local menu_main = {}

-- draw function
function menu_main.draw()
	io.print("menu_main: draw")
	menu.drawstring(0, 0, "hello world!")
end

-- quit function
function menu_main.quit()
	io.print("menu_main: quit")
end

-- init function
function menu_main.init()
	io.print("menu_main: init")
end

-- return class
return menu_main
