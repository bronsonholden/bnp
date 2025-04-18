use("log")

node:GetComponent("Transform"):SetPosition({ x = 3.0, y = 3.0 })
node2:GetComponent("Transform"):SetPosition({ y = 1.0 })

sprite = node:GetComponent("Sprite")
slice = sprite:GetSlice("Eyes")
log.message(slice.x .. ", " .. slice.y)
