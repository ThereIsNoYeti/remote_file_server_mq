--
-- Created by IntelliJ IDEA.
-- User: juston
-- Date: 9/4/14
-- Time: 6:04 PM
-- To change this template use File | Settings | File Templates.
--

--install_directory = "."
--settings = {
--  identity      = install_directory.."/certificates/tmq_server.cert_secret",
--  guest_list    = install_directory.."/certificates",
--  literal_store = "/tmp/ivy",
--
--  development_mode  = true
--}
--
--_G.package.cpath="./build/?.so"
--client = require("lib_tmq_server")
--client:run()
--
--print("Wowowowowowowowow")

--
-- Created by IntelliJ IDEA.
-- User: juston
-- Date: 9/4/14
-- Time: 6:04 PM
-- To change this template use File | Settings | File Templates.


install_directory = "."
settings = {
  identity      = install_directory.."/certificates/tmq_server.cert_secret",
  guest_list    = install_directory.."/certificates",
  literal_store = install_directory.."/nonsense",

  development_mode  = true
}

_G.package.cpath="./build/?.so"
client = require("lib_tmq_server")
client:run()

print("Wowowowowowowowow")
