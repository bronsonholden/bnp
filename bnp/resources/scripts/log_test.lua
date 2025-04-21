node:GetComponent("Transform"):SetPosition({ x = 3.0, y = 3.0 })

sprite = node:GetComponent("Sprite")
slice = sprite:GetSlice("Eyes")

child = node:CreateChild()
child:AddComponent("Transform")
child:AddComponent("Sprite", { path = "resources/sprites/squirrel/squirrel.json" })
child:AddComponent("Texture", { path = "resources/sprites/squirrel/squirrel.png" })
child:AddComponent("Material", { vertex = "resources/shaders/vertex_shader.glsl", fragment = "resources/shaders/fragment_shader.glsl" })
child:AddComponent("Renderable")

water = prefab.load("resources/scripts/water_prefab.lua")
