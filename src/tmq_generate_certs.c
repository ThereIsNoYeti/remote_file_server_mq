/* -- About ------------------------------------------------------------------------------------------------------------
  Author: Juston C. Davies

  Run: <<no options accepted>>
  Install:
  Requirements: czmq (=< x.x.x)
                  -> libzmq (=< x.x.x)
                  -> libsodium (=< x.x.x)
----------------------------------------------------------------------------------------------------------------------*/
#include "../include/tmq_server.h"

int l_run(lua_State *lua) {
  zcert_t *new_certificate;
  char certificate_path[256];

 lua_getglobal(lua, "_G");
  assert(lua_istable(lua, -1));

  lua_getfield(lua, -1, "settings");
  assert(lua_istable(lua, -1));

  lua_getfield(lua, -1, "certificate_path");
      assert(lua_isstring(lua, -1));
      strcpy(certificate_path, lua_tostring(lua, -1)); certificate_path[255]='\0';
      lua_remove(lua, -1);

  new_certificate = zcert_new ();

  zcert_save (new_certificate, certificate_path);

  zcert_destroy(&new_certificate);
  return 0;
}

// -- Begin Lua Import -------------------------------------------------------------------------------------------------
static const luaL_Reg lib_tmq_generate[] = {
   {"run",   l_run},
   {NULL, NULL}  /* sentinel */
};


int luaopen_lib_tmq_generate (lua_State *lua)
{
  luaL_openlib(lua, "lib_tmq_generate", lib_tmq_generate, 0);
  return 1;
}
// -- End Lua Import ---------------------------------------------------------------------------------------------------