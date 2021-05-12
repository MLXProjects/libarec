WARNING - HEAVY WIP! NOT EVEN TESTED OUTSIDE OF MY PC, JUST UPLOADED BECAUSE I DON'T TRUST MY HARD DISK

LIBAREC - Android RECovery in a Library

This is an attempt to make it easier for future recovery devs to 
implement GUIs, unleashing the real potential Android phones have.
libarec can be used in two ways, linking to it in a single app or
as a server (so multiple apps can see the current task info).

1) linking to it in a single app: 
libarec can also be used without a server/client approach, by just using 
it's functions (arec_install, arec_wipe, etc). I want to write some documentation later.

2) as a server: 
libarec includes a server/client implementation, and all you need to do
in order to use it is linking the client library (libarec-client), use 
arec_connect(timeout) and then the functions you need (arec_install, arec_wipe, etc).

If standalone (no server), libarec handles requests in the exact moment.
If being used as client, libarec sends requests to the server.
This is done so the function names don't change between server/client and standalone.

Goals (functionality): 
Full ADB support - this means both client and sideload
ZIP installing
Simple partition management - fstab parse, format, mount (and detect?)

Currently implemented: 
basic server/client stack (client can connect and request a single message)
list parser which gets individual paths from a continuous file list
placeholders for install/wipe
proof-of-concept partition manager (in another project)
standalone ADB client (in another project)

TODO: 
follow the goals
separate client from server in lib to allow smaller link size (create libarec-client)
keep the focus and don't start adding new things before having others done
