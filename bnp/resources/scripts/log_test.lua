node:GetComponent("Transform"):SetPosition({ x = 3.0, y = 3.0 })

sprite = node:GetComponent("Sprite")
slice = sprite:GetSlice("Eyes")

water = prefab.load("resources/scripts/water_prefab.lua", {
	columns = 100,
	columnWidth = 0.1,
	height = 0.15
})
