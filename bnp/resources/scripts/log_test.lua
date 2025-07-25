sprite = node:GetComponent("Sprite")
sprite:SetLayerVisible("EyesClosed", true)
sprite:SetLayerVisible("EyesOpen", true)
sprite:SetLayerVisible("Straw", false)


water = prefab.load("resources/scripts/water_prefab.lua", {
	columns = 81,
	columnWidth = 0.1,
	height = 0.18
})
water:GetComponent("Transform"):SetPosition({ x = 13.125, y = 0.7 })

--[[
toad = prefab.load("resources/scripts/toad_prefab.lua")
toad:GetComponent("Transform"):SetPosition({ x = 1.0, y = 0.25 })
]]

nest = prefab.load("resources/scripts/behavior_nest_prefab.lua")
nest:GetComponent("Transform"):SetPosition({ x = 4, y = 1.5 })

for i=1,30 do
	if i == 1 then
		queen = prefab.load("resources/scripts/queen_bee_prefab.lua")
		queen:AddComponent("BehaviorNest", nest)
		queen:GetComponent("Transform")
		     :SetPosition({ x = 3 + math.random(-200, 200) * 0.001, y = 1 + math.random(40, 50) * 0.01, z = 0 })
	else
		bee = prefab.load("resources/scripts/bee_prefab.lua")
		bee:AddComponent("BehaviorNest", nest)
		bee:GetComponent("Transform"):SetPosition({ x = 3 + math.random(-200, 200) * 0.001, y = 1 + math.random(40, 50) * 0.01, z = 0 })
	end
end
