# Mechgah README

## Dependencies

Our NES emulator depends on various library, which are:

- libsdl1.2-dev
- libsdl-gfx1.2-dev
- [cmocka](https://cmocka.org/)

You can install libsdl1.2-dev and libsdl-gfx1.2-dev with this command on Ubuntu :
```bash
sudo apt install libsdl1.2-dev libsdl-gfx1.2-dev
```

## Makefile

```bash
make            # build mechgah executable (you can precise mechgah)
make run-test   # build unit test and run it with Valgrind
                # and code coverage feature
make doc        # build Doxygen documentation from doxyfile
make clean      # clean every generated file (including coverage and doc dir)
```

## CMake

Run the following command to compile this project through CMake:
```bash
cmake . -Bbuild
cd build
make mechgah    # build mechgah executable
make utest      # build unit test
```
If you want to launch unit tests, run ./build/utest from the root of the repository (that is because our unit tests use relative path for file opening). If you are using CLion, precise executables working directory to root path.

## User manual

### Synopsis
```bash
./mechgah [OPTION]... [ROM]
```

### Description

Launches the NES emulator on the given ROM filename and with the given OPTION. If no ROM filename is given, the emulator won't launch. If multiple filenames are given, the emulator will launch with the first one.

### Options

    -s [scale]
      Defines the scaling factor. Must be between 1 (genuine resolution) and 15 (4K resolution).
      If not specified, scaling factor is set to 2.
