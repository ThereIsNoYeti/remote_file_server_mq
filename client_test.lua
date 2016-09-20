--
-- Created by IntelliJ IDEA.
-- User: juston
-- Date: 8/25/14
-- Time: 4:43 PM
-- To change this template use File | Settings | File Templates.
--

install_directory = "."
temporary_store = "/tmp/bonsai"
director = {
  env = {
    nexus_update_path = "/tmp/bonsai",
    identity        = install_directory.."/certificates/client.cert_secret",
    server_key      = install_directory.."/certificates/server.cert",
    development_mode  = true
  },
  process_site_update = function(name)  print("I got an update to process in directory: ".. temporary_store .."/" ..name) end
}

_G.package.cpath="./build/?.so"
client = require("lib_tmq_client")
client:run()

print("Wowowowowowowowow")