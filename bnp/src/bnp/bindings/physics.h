extern "C" {
#include <lua.h>
}

namespace bnp {

	int l_physics_body_2d_get_position(lua_State* L);
	int l_physics_body_2d_set_position(lua_State* L);
	int l_physics_body_2d_get_velocity(lua_State* L);
	int l_physics_body_2d_create_box_fixture(lua_State* L);
}
