--[[
Creates a toad sprite
]]

node:AddComponent("Transform", { x = 0, y = 0, z = 0 })
node:AddComponent("Texture", { path = "resources/sprites/toad/toad.png" })
node:AddComponent("Sprite", { path = "resources/sprites/toad/toad.json" })
node:AddComponent("Material", {
	resourceId = "sprite_material",
	vertex = "resources/shaders/sprite_vertex_shader.glsl",
	fragment = "resources/shaders/sprite_fragment_shader.glsl"
})
node:AddComponent("Renderable")
node:AddComponent("Identity", "Toad")
node:AddComponent("Motility", { speed = 1.1, flying = true })
node:AddComponent("Perception", { Bee = 2 })
node:AddComponent("FlowField2D", {})
body = node:AddComponent("PhysicsBody2D", { dynamic = false })
body:CreateBoxFixture({ w = 0.4, h = 0.3, restitution = 0.0 })

--sensor = node:CreateChild()
--sensor:AddComponent("Transform", { y = 0.2 })
--sensorBody = sensor:AddComponent("PhysicsBody2D", { dynamic = true, gravityScale = 0 })
--sensorBody:CreateBoxFixture({ w = 0.4, h = 0.1, sensor = true })
--sensor:AddComponent("Renderable")
