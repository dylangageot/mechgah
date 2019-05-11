# NES Emulator

## Dependencies

Our NES emulator depends on various library, which are:

- libsdl1.2-dev
- libsdl-gfx1.2-dev
- cmocka

## Make

```bash
make            # build main file and unit test
make run-test   # build unit test and run it with Valgrind
                # and code coverage feature
make clean      # clean every generated file (including coverage dir)
```

## User manual

### Synopis
```bash
./main [OPTIONS]... [ROM]
```

### Description

Launches the nes-emulator on the given ROM filename and with the given OPTIONS. If no ROM filename is given, the emulator won't launch. If multiple filenames are given, the emulator will launch with the first one.

### Options

    -s [scale]
      Defines the scaling factor. Must be between 1 (genuine resolution) and 15 (4K resolution).
      If not specified, scaling factor is set to 2.
