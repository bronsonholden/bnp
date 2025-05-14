## TODO

* [ ] Collision manager to allow collision callbacks in Lua scripts
* [ ] Add script update hook
* [ ] Create design document
* [ ] Consistent naming of `2D` classes & files (should be e.g. `water_2d_manager.h` for `Water2DManager`)
* [x] Fix blocked cell checks for `FlowField2D` (sampled directions can point into a blocked cell diagonally without correction)
* [ ] Bee behavior: snap current velocity based on blocked cells so flight response can be reimplemented
* [ ] Create separate layer for squirrel tail if there's an obstruction behind so it doesn't clip too much

## Four Pillars

1. Creature Interactions
2. Problem Solving
3. Exploration
4. Collection

## Credits

### Xantoor

For game design guidance and direction.
