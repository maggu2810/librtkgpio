# C library for RTK.GPIO

## Requirements

This library requires the [libserialport](https://sigrok.org/wiki/Libserialport) for serial port access.

On Fedora you can install the library (for development):

```
sudo dnf install libserialport-devel
```

## Build on Windows

I am using MSYS for MinGW 64 bit

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
