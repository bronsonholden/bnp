node:GetComponent("Sprite"):SetLayerVisible("Straw", true)

sprite = node:GetComponent("Sprite")
slice = sprite:GetSlice("Eyes")

water = prefab.load("resources/scripts/water_prefab.lua", {
	columns = 100,
	columnWidth = 0.1,
	height = 0.15
})

for i=1,20 do
	prefab.load("resources/scripts/bee_prefab.lua")
end
