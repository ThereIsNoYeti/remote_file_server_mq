#ifndef NEXUS_UTILITY
#define NEXUS_UTILITY

#include <lua5.2/lua.h>
#include <lua5.2/lualib.h>
#include <lua5.2/lauxlib.h>

#include <czmq.h>

#include <stdio.h>
#include <stdarg.h>
#include <string.h>

#define HELPER_SCOPE "director"

// -- Begin Utility Methods --------------------------------------------------------------------------------------------
void _populate_uri(char *container, char* address, int port_number)
{
  if (port_number > 0)
    sprintf(container, "%s:%d\0", address, port_number);
  else
    sprintf(container, "%s:*\0", address);
}

static void _error(lua_State *lua, const char *message_format, ...)
{
  va_list args;
  va_start(args, message_format);
  fprintf(stderr, message_format, args);

  va_end(args);

  //...eh, I'm not so sure about this in the context of ZMQ. I may just want to return.
  //lua_close(lua);
  //exit(EXIT_FAILURE);
}

static void _lua_stack_dump (lua_State *L) {
  int i;
  int top = lua_gettop(L);
  for (i = 1; i <= top; i++) {
    int t = lua_type(L, i);
    switch (t)
    {
      case LUA_TSTRING:  {printf("\"%s\"", lua_tostring(L, i));           break;}
      case LUA_TBOOLEAN: {printf(lua_toboolean(L, i) ? "true" : "false"); break;}
      case LUA_TNUMBER:  {printf("%g", lua_tonumber(L, i));               break;}
      default:           {printf("%s", lua_typename(L, t));               break;}
    }
    fprintf(stderr, " => ");
  }
  fprintf(stderr, "\n");
}

static void _invoke_lua_function(lua_State *lua, const char *helper_scope, const char *function_name, const char *signature, ...)
{
  va_list args;
  int num_args, res_num;
  va_start (args, signature);

  //lua_getglobal(lua, function_name); if (!lua_isfunction(lua, -1)) _error(lua, "Invalid function (%s)\n", function_name);
  lua_getglobal(lua, "_G"); if (!lua_istable(lua, -1)) _error(lua, "Unable to get globabl environment!\n");
  lua_getfield(lua, -1, helper_scope); if (!lua_istable(lua, -1)) _error(lua, "Invalid scope (%s)\n", helper_scope);
  lua_getfield(lua, -1, function_name); if (!lua_isfunction(lua, -1)) _error(lua, "Invalid function (%s)\n", function_name);
    lua_remove(lua, -3); lua_remove(lua, -2);


  for(num_args = 0; *signature; num_args++)
  {
     luaL_checkstack(lua, 1, "The stack is full! Too many arguments!");
     switch(*signature++)
     {
        case 'd': {lua_pushnumber (lua, va_arg(args, double)); break;}
        case 'i': {lua_pushinteger(lua, va_arg(args, int));    break;}
        case 's': {lua_pushstring (lua, va_arg(args, char *)); break;}
        case 'b': {lua_pushboolean(lua, va_arg(args, int));    break;}

        case '>': {goto endargs;}
        default: {_error(lua, "Invalid option (%c)\n", *(signature-1));}
     }
  }
  endargs:
  res_num = strlen(signature);

  if (lua_pcall(lua, num_args, res_num, 0) != 0)//Stack Change
      fprintf(stderr, "_error calling '%s': %s\n", function_name, lua_tostring(lua, -1));

  res_num = -res_num;
  while(*signature)
  {
    switch (*signature++)
    {
      case 'd':
      {
        if(!lua_isnumber(lua, res_num)) _error(lua, "Wrong result type!\n");
        *va_arg(args, double *) =  lua_tonumber (lua, res_num);
        break;
      }
      case 'i':
      {
        if(!lua_isnumber(lua, res_num)) _error(lua, "Wrong result type!\n");
        *va_arg(args, int *) =  lua_tointeger (lua, res_num);
        break;
      }
      case 's':
      {
        if(!lua_isstring(lua, res_num)) _error(lua, "Wrong result type!\n");
        *va_arg(args, const char **) =  lua_tostring(lua, res_num);
        break;
      }
      case 'b':
      {
        if(!lua_isboolean(lua, res_num)) _error(lua, "Wrong result type!\n");
        *va_arg(args, int *) =  lua_tointeger (lua, res_num);
        break;
      }

      default: {_error(lua, "Invalid option (%c)\n", *(signature-1));}
    }
    res_num++;
  }

  va_end(args); //Returns args as listed
}

// -- End Utility Methods ----------------------------------------------------------------------------------------------

#endif