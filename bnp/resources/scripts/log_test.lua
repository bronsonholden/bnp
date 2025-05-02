--node:GetComponent("Sprite"):SetLayerVisible("Straw", true)

sprite = node:GetComponent("Sprite")
slice = sprite:GetSlice("Eyes")

water = prefab.load("resources/scripts/water_prefab.lua", {
	columns = 100,
	columnWidth = 0.1,
	height = 0.15
})

toad = prefab.load("resources/scripts/toad_prefab.lua")
toad:GetComponent("Transform"):SetRotation({ radians = -0.3 })

nest = prefab.load("resources/scripts/behavior_nest_prefab.lua")
nest:GetComponent("Transform"):SetPosition({ y = 0.5 })

for i=1,30 do
	if i == 1 then
		queen = prefab.load("resources/scripts/queen_bee_prefab.lua")
		queen:AddComponent("BehaviorNest", nest)
	end
	bee = prefab.load("resources/scripts/bee_prefab.lua")
	bee:AddComponent("BehaviorNest", nest)
end

