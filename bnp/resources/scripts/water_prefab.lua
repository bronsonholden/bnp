--[[
Creates a 2D water node with the given params
]]

node:AddComponent("Transform", { x = 0, y = 0, z = 0 })
node:AddComponent("Material", {
	resourceId = "water2d_material",
	vertex = "resources/shaders/water2d_vertex_shader.glsl",
	fragment = "resources/shaders/water2d_fragment_shader.glsl"
})
node:AddComponent("Renderable")
node:AddComponent("Water2D", params)
