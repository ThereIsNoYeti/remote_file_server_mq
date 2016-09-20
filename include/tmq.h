#ifndef __TMQ_H_INCLUDED__
  #define __TMQ_H_INCLUDED__

  //-- Dependencies ----------
  #include <sodium.h>
  #include <czmq.h>
  #include "fmq.h"
  #include <stdio.h>
  #include <stdarg.h>
  #include <string.h>
  #include <syslog.h>

  // -- Constants ----------
  #define TMQ_VERSION_MAJOR 0
  #define TMQ_VERSION_MINOR 7
  #define TMQ_VERSION_PATCH 0

  #define TMQ_SERVER_PORT 5670
  #define TMQ_SERVER_URL "server.files.nexus.teliax.net"
  #define DEVMQ_SERVER_URL "development.nexus.teliax.net"

  #define SITE_VIRTUAL_PATH "/ivy/sites"

  //-- Structures ----------

  //-- Utility Functions ----------
  #define TMQ_MAKE_VERSION(major, minor, patch) \
    ((major) * 10000 + (minor) * 100 + (patch))
  #define TMQ_VERSION \
    TMQ_MAKE_VERSION(TMQ_VERSION_MAJOR, TMQ_VERSION_MINOR, TMQ_VERSION_PATCH)

  void _assemble_uri(char *container, char* address, int port_number)
  {
    if (port_number > 0)
      sprintf(container, "tcp://%s:%d\0", address, port_number);
    else
      sprintf(container, "tcp://%s:*\0", address);
  }

  void _print_versions()
  {
      fprintf(stderr, "Checking versions...\n");
      fprintf(stderr, "\tSodium:   %s\n",       SODIUM_VERSION_STRING);
      fprintf(stderr, "\t0MQ:      %d.%d.%d\n", ZMQ_VERSION_MAJOR, ZMQ_VERSION_MINOR, ZMQ_VERSION_PATCH);
      fprintf(stderr, "\tCZMQ:     %d.%d.%d\n", CZMQ_VERSION_MAJOR, CZMQ_VERSION_MINOR, CZMQ_VERSION_PATCH);
      fprintf(stderr, "\tFileMQ:   %d.%d.%d\n", FMQ_VERSION_MAJOR, FMQ_VERSION_MINOR, FMQ_VERSION_PATCH);
      fprintf(stderr, "\tTeliaxMQ: %d.%d.%d\n", TMQ_VERSION_MAJOR, TMQ_VERSION_MINOR, TMQ_VERSION_PATCH);
  }

#endif