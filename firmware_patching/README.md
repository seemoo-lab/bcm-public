# firmware_patching

This directory contains all firmware patches in separate 
directories. The patches generally consist of ...

... a C-file (patch.c) containing the code that should be 
    written to the firmware, 

... a Linker-file (patch.ld) that describes where the patched 
    functions should be placed in the firmware,

... a Python-file (patcher.py) that builds the patches into a
    firmware binary

... a Makefile that handles the compilation and linking of the
    C-code as well as the execution of the patcher,

... a driver directory (bcmdhd) that contains the driver 
    corresponding to the firmware patch,

... and a README file that describes what the patch does and how
    it can be executed. The commands given in the README files
    generally have to be run in the repositories root directory.

## Generate a new project:

To generate a new firmware patching project, you can simply run
```
make newproject NEWPROJECT=name_of_new_project
```
in this directory. It will copy the template_project to a new 
directory called name_of_new_project and adjust the Makefile of
the driver to find the correct include files. After creating a
new project, we advice you to add the files to a git repository.
After that, you can start working on your patches. This way, you
can always see what changed in your project compared to the 
original template files.

## Compile a already existing project:

For example to compile the `hello_world_example`, you can simply run
```
make reloadfirmware FWPATCH=hello_world_example
```
from the **root** directory

## Currently, the following example projects exist:

* d11_example: Interaction with the D11 core
* debugger_example: Using the ARM Debugging interface
* hello_world_example: Prints hello world to the chips console
* hello_beacon_example: send a single beacon frame on firmware start 
* ioctl_example: Exchanges ioctl messages with the firmware
* mapaddr_example: Analysis of stack traces
* udp_to_android_example: Send UDP frames over SDIO to Android

