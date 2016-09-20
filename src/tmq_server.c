/* -- About ------------------------------------------------------------------------------------------------------------
  Author: Juston C. Davies
  Purpose:  teliaxmq_file_server - Hard-coded file synchronization with plaintext authentication intended to be used with
            Ivy and Bonsai products for cheap information exchange.NOT TO BE USED OVER THE LONG TERM.

  Run: <<no options accepted>>
  Install:
  Requirements: czmq (=< x.x.x)
                  -> libzmq (=< x.x.x)
                  -> libsodium (=< x.x.x)
----------------------------------------------------------------------------------------------------------------------*/
#include "../include/tmq_server.h"

int l_run(lua_State *lua) {
  //Allocations
  bool development_mode;
  char service_address[256], file_store[256], identity_file[256], guest_directory[256];
  fmq_server_t *server;

 //The time for Lua is the time for today!
  lua_getglobal(lua, "_G");
  assert(lua_istable(lua, -1));

  lua_getfield(lua, -1, "settings");
  assert(lua_istable(lua, -1));

  lua_getfield(lua, -1, "identity");
      assert(lua_isstring(lua, -1));
      strcpy(identity_file, lua_tostring(lua, -1)); identity_file[255]='\0';
      lua_remove(lua, -1);

  lua_getfield(lua, -1, "guest_list");
      assert(lua_isstring(lua, -1));
      strcpy(guest_directory, lua_tostring(lua, -1)); guest_directory[255]='\0';
      lua_remove(lua, -1);

  lua_getfield(lua, -1, "literal_store");
      assert(lua_isstring(lua, -1));
      strcpy(file_store, lua_tostring(lua, -1)); file_store[255]='\0';
      lua_remove(lua, -1);

  lua_getfield(lua, -1, "development_mode");
      //assert(lua_isboolean(lua, -1));  //Allow null, probably stupid practice but it seems fine.
      development_mode= lua_toboolean(lua, -1);
      lua_remove(lua, -1);

  fprintf(stderr, "Starting with Lua configuration values: \n\tIdentity: %s\n\tGuest Keys: %s\n\tFile Store: %s\n", identity_file, guest_directory, file_store);

  //Check versions
  _print_versions();

  //Assemble requisites
  _assemble_uri(service_address, "*", TMQ_SERVER_PORT);

  //Start Server
  server = fmq_server_new();
  fprintf(stderr, "Setting up server...\n");                                                                          //Using the 'http://rfc.zeromq.org/spec:4' via the czmq zconfig api (http://czmq.zeromq.org/manual:zconfig)
    //-- Zconf config short-circuit ----------
    fmq_server_setoption(server, "/server/monitor", "1");
    fmq_server_setoption(server, "/server/heartbeat", "1");

    fprintf(stderr, "Applying configuration... \n");
      fmq_server_apply_configuration(server);

    //-- Server Options ----------
    fprintf(stderr, "Flipping server switches... \n");
    fmq_server_apply_certificate(server, identity_file);
    fmq_server_configure_curve(server, "*", guest_directory);


    //-- Publish ----------
    fprintf(stderr, "Publishing virtual streams... \n");
    fmq_server_publish(server, file_store, SITE_VIRTUAL_PATH );

  fprintf(stderr, "Going live, binding on: %s\n", service_address);
    assert(fmq_server_bind (server, service_address) == TMQ_SERVER_PORT);

  //-- Living Loop ---------- (while reactor runs in background)
  fprintf(stderr, "Running...\n");
  while (!zctx_interrupted){ zclock_sleep (1000);}

  //-- Teardown ----------
  fprintf(stderr, "Interrupted\nShutting down...\n");
      lua_remove(lua, -1); //_G
      lua_remove(lua, -1); //settings
  fmq_server_destroy (&server);
  return 0;
}

// -- Begin Lua Import -------------------------------------------------------------------------------------------------
static const luaL_Reg lib_tmq_server[] = {
   {"run",   l_run},
   {NULL, NULL}  /* sentinel */
};


int luaopen_lib_tmq_server (lua_State *lua)
{
  luaL_openlib(lua, "lib_tmq_server", lib_tmq_server, 0);
  return 1;
}
// -- End Lua Import ---------------------------------------------------------------------------------------------------