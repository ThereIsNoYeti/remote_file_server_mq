/* -- About ------------------------------------------------------------------------------------------------------------
  Author: Juston C. Davies

  Run: <<no options accepted>>
  Install:
  Requirements: FileMQ (=<x.x.x)
                 -> czmq (=< x.x.x)
                   -> libzmq (=< x.x.x)
                   -> libsodium (=< x.x.x)
                Lua 5.2 (<= 5.2.0)
----------------------------------------------------------------------------------------------------------------------*/
#include "../include/tmq_client.h"

int l_run(lua_State *lua) {
  //Allocations
  bool development_mode;
  char service_address[256], identity_file[256], server_key[256], file_store[256];
  fmq_client_t *client;

  //The time for Lua is the time for today!
  lua_getglobal(lua, "_G");
    assert(lua_istable(lua, -1));

  lua_getfield(lua, -1, "director");
    assert(lua_istable(lua, -1));

  lua_getfield(lua, -1, "env");
    assert(lua_istable(lua, -1));

  lua_getfield(lua, -1, "identity");
      assert(lua_isstring(lua, -1));
      strcpy(identity_file, lua_tostring(lua, -1)); identity_file[255]='\0';
      lua_remove(lua, -1);

  lua_getfield(lua, -1, "server_key");
      assert(lua_isstring(lua, -1));
      strcpy(server_key, lua_tostring(lua, -1)); server_key[255]='\0';
      lua_remove(lua, -1);

  lua_getfield(lua, -1, "nexus_update_path");
      assert(lua_isstring(lua, -1));
      strcpy(file_store, lua_tostring(lua, -1)); file_store[255]='\0';
      lua_remove(lua, -1);

  lua_getfield(lua, -1, "development_mode");
      //assert(lua_isboolean(lua, -1));  //Allow null, probably stupid practice but it seems fine.
      development_mode= lua_toboolean(lua, -1);
      lua_remove(lua, -1);

      lua_remove(lua, -1); //_G
      lua_remove(lua, -1); //director
      lua_remove(lua, -1); //env

  fprintf(stderr, "Starting with Lua configuration values: \n\tIdentity: %s\n\tServer Key: %s\n\tFile Store: %s\n", identity_file, identity_file, file_store);

  //Check versions
  _print_versions();

  //Assemble requisites
  if(development_mode == true)
    _assemble_uri(service_address, DEVMQ_SERVER_URL, TMQ_SERVER_PORT);
  else
    _assemble_uri(service_address, TMQ_SERVER_URL, TMQ_SERVER_PORT);

  //Start Client
  client = fmq_client_new ();
  fprintf(stderr, "Setting up client...\n");                                                                          //Using the 'http://rfc.zeromq.org/spec:4' via the czmq zconfig api (http://czmq.zeromq.org/manual:zconfig)
    //-- Zconf config short-circuit ----------
    fmq_client_setoption(client, "/client/heartbeat", "1");

    fprintf(stderr, "Applying configuration... \n");
    fmq_client_apply_configuration(client);

    //-- Client Options ----------
    fprintf(stderr, "Flipping client switches... \n");
    fmq_client_set_resync (client, true);
    fmq_client_set_inbox(client, file_store);
    fmq_client_apply_certificate(client, identity_file);

    //-- Subscribe ----------
    fprintf(stderr, "Subscribing to virtual streams... \n");
    fmq_client_subscribe (client, SITE_VIRTUAL_PATH);

  fprintf(stderr, "Going live, connecting to: %s\n", service_address);
    fmq_client_connect (client, service_address, server_key);

  //-- Living Loop ---------- (while reactor runs in background)
  fprintf(stderr, "Running...\n");
    //while (!zctx_interrupted){ zclock_sleep (1000);} //OLD LOOP
  while(true){
    zmsg_t *msg = fmq_client_recv (client);

    if (!msg) break;              //  Interrupted

    char *command = zmsg_popstr (msg);

    if (streq (command, "DELIVER")) {
        char *name = zmsg_popstr (msg);
        char *path = zmsg_popstr (msg);
        _invoke_lua_function(lua, "director", "process_site_update", "s",  name);
        free (name);
        free (path);
    }

    free (command);
    zmsg_destroy (&msg);
  }

  //-- Teardown ----------
  fprintf(stderr, "Interrupted\nShutting down...\n");
  fmq_client_destroy(&client);
  return 0;
}

// -- Begin Lua Import -------------------------------------------------------------------------------------------------
static const luaL_Reg lib_tmq_client[] = {
   {"run",   l_run},
   {NULL, NULL}  /* sentinel */
};


int luaopen_lib_tmq_client (lua_State *lua)
{
  luaL_openlib(lua, "lib_tmq_client", lib_tmq_client, 0);
  return 1;
}
// -- End Lua Import ---------------------------------------------------------------------------------------------------