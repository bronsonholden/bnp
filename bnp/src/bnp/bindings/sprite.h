#pragma once

extern "C" {
#include <lua.h>
}

namespace bnp {

	int l_sprite_get_slice(lua_State* L);
	int l_sprite_set_layer_visible(lua_State* L);

}
