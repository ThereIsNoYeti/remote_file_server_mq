--
-- Created by IntelliJ IDEA.
-- User: juston
-- Date: 9/4/14
-- Time: 6:04 PM
-- To change this template use File | Settings | File Templates.
--

install_directory = "."
settings = {
  certificate_path = install_directory.."/certificates/new_certificate.cert",
}

_G.package.cpath="./build/?.so"
client = require("lib_tmq_generate")
client:run()

print("Wowowowowowowowow")