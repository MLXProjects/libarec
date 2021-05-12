# WARNING - HEAVY WIP! NOT EVEN TESTED OUTSIDE OF MY PC, JUST UPLOADED BECAUSE I DON'T TRUST MY HARD DISK

## LIBAREC - Android RECovery in a Library

This is an attempt to make it easier for future recovery devs to implement GUIs, unleashing the real potential Android phones have.  
Libarec is written in C and can be used in two ways, linking to it in a single app or as a server (so multiple apps can see the current task info).

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

### How to use:
For including, it should be enough with #include <arec.h> and passing -I/path/to/libarec/include to the compiler  
For library building, just use build.cmd (by default also builds server, use build.cmd noserver to just build the lib)  
For building an app, just put some source at tests folder and modify the build.cmd either at mkclient or mktest section (wether you want to build a standalone or a client app), then build it using build.cmd test (defaults to client app, use "build.cmd test noserver" if want a standalone app)

### Contents: 
include/ - main library headers  
src/ - main library source  
test/ - some test apps, both standalone and client  
build.cmd - build script for Windows (can be easily ported to Linux tho)  
test.cmd - pushes the standalone test and runs it (may be outdated)

### Goals (functionality): 
Full ADB support - this means both client and sideload  
ZIP installing  
Simple partition management - fstab parse, format, mount (and detect?)

### Currently implemented: 
basic server/client stack (client can connect and request a single message)  
list parser which gets individual paths from a continuous file list  
placeholders for install/wipe  
proof-of-concept partition manager (in another project)  
standalone ADB client (in another project)  

### TODO: 
follow the goals  
separate client from server in lib to allow smaller link size (create libarec-client)  
keep the focus and don't start adding new things before having others done  

### THANKS:
I've used parts of code (either directly or as inspiration) from some other projects:  
[Libaroma](https://github.com/amarullz/libaroma), by Ahmad Amarullah (this made me think about having a separate recovery library, and made me learn a lot about C)  
[UDS](https://github.com/shengkui/uds), by Shengkui Leng (most of the code for the server/client stack is from here)  
  
Special thanks to [Frantisek Kysela](https://github.com/Kysela/) for building [Pterodon recovery](https://github.com/PterodonRecovery/pterodon) being just one guy, him made me know that android recovery development is hard but not entirely dead - respects to him :D
  
### LICENSE: 
This project is licensed under the Apache License, Version 2.0 (the "License"); you may not use this file except in compliance with the License.  
You may obtain a copy of the License at  
  
    http://www.apache.org/licenses/LICENSE-2.0  
  
Unless required by applicable law or agreed to in writing, software distributed under the License is distributed on an "AS IS" BASIS, WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.  
See the License for the specific language governing permissions and limitations under the License.  
