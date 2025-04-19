use("log")

node:GetComponent("Transform"):SetPosition({ x = 3.0, y = 3.0 })

sprite = node:GetComponent("Sprite")
slice = sprite:GetSlice("Eyes")
log.message(slice.x .. ", " .. slice.y)

child = node:CreateChild()
child:AddComponent("Transform", { x = 10 })
