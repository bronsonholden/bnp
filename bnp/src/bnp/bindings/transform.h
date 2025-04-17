#pragma once

extern "C" {
#include <lua.h>
}

namespace bnp {

	int l_transform_get_position(lua_State* L);
	int l_transform_set_position(lua_State* L);

}
