This Plugin Needs LibPng which depends on ZLip (compression library).

To make it work:

- Download and build ZLib: https://zlib.net/
  - Configure with CMake and be mindful of selecting x64 platform
    if you intend to use this Menge Plugin on that platform
  - Build and install in a location of your chosing
  - Add the directory where ZLib is installed to the system path
    This is needed so that CMAKE will find in when building LibPng
    In the next step

- Download and build LibPng: http://www.libpng.org/
  - Configure with CMake and be mindful of selecting x64 platform
    if you intend to use this Menge Plugin on that platform.
    You should not get any errors if you have ZLib in your
    system path. If you still get errors, maybe close and re-open
    CMake, to update the system variables
  - Build and install in a location of your chosing
  - Create a new system variable called LibPng and set the
    value to the install directory. This variable is used by 
    Visual Studio to determine additiona compiler include
    directories and linker .lib files.
