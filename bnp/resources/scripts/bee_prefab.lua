--[[
Creates a bee sprite
]]

node:AddComponent("Transform", { x = math.random(-200, 200) * 0.001, y = math.random(20, 50) * 0.01, z = 0 })
    :SetScale({ x = 0.25, y = 0.25, z = 0.25 })
node:AddComponent("Texture", { path = "resources/sprites/bee/bee.png" })
node:AddComponent("Sprite", { path = "resources/sprites/bee/bee.json" })
node:AddComponent("Material", {
	resourceId = "sprite_material",
	vertex = "resources/shaders/sprite_vertex_shader.glsl",
	fragment = "resources/shaders/sprite_fragment_shader.glsl"
})
node:AddComponent("Renderable")
node:AddComponent("Identity", "Bee")
node:AddComponent("Motility", { speed = 1.1, flying = true })
body = node:AddComponent("PhysicsBody2D", { gravityScale = 0, dynamic = true })
body:CreateBoxFixture({ w = 0.1, h = 0.1, sensor = true })
