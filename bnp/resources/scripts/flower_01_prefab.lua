--[[
Creates a flower_01 sprite
]]

node:AddComponent("Transform", { y = 0.5 }):SetScale({ x = 0.5, y = 1.0, z = 1.0 })
node:AddComponent("Texture", { path = "resources/sprites/flower_01/flower_01.png" })
node:AddComponent("Sprite", { path = "resources/sprites/flower_01/flower_01.json" })
node:AddComponent("Material", {
	resourceId = "sprite_material",
	vertex = "resources/shaders/sprite_vertex_shader.glsl",
	fragment = "resources/shaders/sprite_fragment_shader.glsl"
})
node:AddComponent("Renderable")
