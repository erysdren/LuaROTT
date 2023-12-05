-- ControlPanel.lua

-- create new ControlPanel
local MainPanel = ControlPanel()

-- add new game option and player panel submenu
local ChoosePlayerPanel, NewGameOption = MainPanel:AddSubMenu("NEW GAME")

-- set submenu title
ChoosePlayerPanel:SetTitle("Choose Player")

-- open main panel
MainPanel.Open()
