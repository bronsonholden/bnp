#include <bnp/bindings/transform.h>
#include <bnp/components/script.h>
#include <bnp/components/graphics.h>
#include <bnp/bindings/node.h>

#include <iostream>
using namespace std;

extern "C" {
#include <lauxlib.h>
#include <lualib.h>
}

namespace bnp {

	// sprite:getSlice("SliceName")
	int l_sprite_get_slice(lua_State* L) {
		// [sprite, name]
		Node node = l_pop_script_node(L, 1);

		Sprite& sprite = node.get_component<Sprite>();

		int current_frame = 0;

		if (node.has_component<SpriteAnimator>()) {
			current_frame = node.get_component<SpriteAnimator>().current_framelist_index;
		}

		std::string slice_name = luaL_checkstring(L, -1);

		auto& slice_keys = sprite.slices.at(slice_name);

		if (slice_keys.find(current_frame) != slice_keys.end()) {
			glm::ivec4 slice = slice_keys.at(current_frame);
			lua_newtable(L);
			lua_pushnumber(L, slice.x);
			lua_setfield(L, -2, "x");
			lua_pushnumber(L, slice.y);
			lua_setfield(L, -2, "y");
			lua_pushnumber(L, slice.z);
			lua_setfield(L, -2, "z");
			lua_pushnumber(L, slice.w);
			lua_setfield(L, -2, "w");
		}
		else {
			lua_pushnil(L);
		}


		return 1;
	}

}
