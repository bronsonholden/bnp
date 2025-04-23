# Lua bindings

The scripting API is component based, but userdata values
passed between C++ and Lua are all `ScriptNode` objects
that serve as a proxy for `Node`s. When pushed onto the stack,
a metatable is assigned that provides the appropriate API, e.g.
transform bindings to update the corresponding node's `Transform`.

This is done to avoid allowing scripts to directly mutate the
underlying components, in case they have been updated or even
removed elsewhere.
