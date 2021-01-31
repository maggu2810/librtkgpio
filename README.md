# C library for RTK.GPIO

## Dependencies

This library requires the [libserialport](https://sigrok.org/wiki/Libserialport) for serial port access.

## Build

To compile this library you need the header file and the shared object of libserialport.

### Linux

You need to ensure that the header file and the shared library of libserialport is present on your system.
The installation depends on your specific linux distribution (e.g. on Fedora you can install it by `sudo dnf install libserialport-devel`).

The build instruction for Linux system are the "common" ones for cmake projects.

* clone the sources
* create a build directory
* enter the build directory
* call `cmake path/to/the/sources`
* call `make`
* call `make install` (or something similar to place the library to your desired location)
 
## Windows

This depends a little bit on the build environment you are using.

There the instructions for MSYS with MinGW 64 bit.

### libserialport

The stuff that is necessary to be present in your MSYS installation can be taken from the official libserialport development section.

We will build the library and install the dynamic library to 'c:\tools\cdev\lib' and the header file to 'c:\tools\cdev\include'.

```
git clone git://sigrok.org/libserialport
cd libserialport
./autogen.sh
./configure
make

mkdir -p /c/tools/cdev/lib
mkdir -p /c/tools/cdev/include

find -name "*.dll" -exec cp -v '{}' /c/tools/cdev/lib \;
find -name "libserialport.h" -exec cp -v '{}' /c/tools/cdev/include \;
```

### librtkgpio

Now let's build the library and install the dynamic library to 'c:\tools\cdev\lib' and the header file to 'c:\tools\cdev\include'.

We inform the build system about the location the shared library and header file for libserialport can be found. 

```
git clone https://github.com/maggu2810/librtkgpio.git
cd librtkgpio
mkdir build
cd build/

cmake ../rtkgpio/ -G "MSYS Makefiles" -DSERIALPORT_INCLUDE_DIRS=/c/tools/cdev/include -DSERIALPORT_LIBRARIES=/c/tools/cdev/lib/libserialport-0.dll
make

cp -vax ../rtkgpio/include/rtkgpio /c/tools/cdev/include
find -name "*.dll" -exec cp -v '{}' /c/tools/cdev/lib \;
```

