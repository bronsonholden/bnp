--[[
Creates a bee sprite
]]

node:AddComponent("Transform", { x = 0, y = 1, z = 0 })
    :SetScale({ x = 0.25, y = 0.25, z = 0.25 })
node:AddComponent("Texture", { path = "resources/sprites/bee/bee.png" })
node:AddComponent("Sprite", { path = "resources/sprites/bee/bee.json" })
node:AddComponent("Material", {
	resourceId = "sprite_material",
	vertex = "resources/shaders/sprite_vertex_shader.glsl",
	fragment = "resources/shaders/sprite_fragment_shader.glsl"
})
node:AddComponent("Renderable")
