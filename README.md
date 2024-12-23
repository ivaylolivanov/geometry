# Computational Geometry

This is a repository meant for the assignments, given in the subject of **Computational Geometry**, in the masters degree **Computer graphics**.\
Its main purposes are:
 - public learning
 - notes keeping
 - knowledge sharing

---

## Dependencies

1. **Vecta Library**
   Minor library with vectors utilities.
   `vecta.h` can be found [here](http://www.math.bas.bg/bantchev/vecta/vecta.h).
   The solutions in the notebook directory expect the 'vecta.h' file under 'dep'

2. **SP Drawing Tool**
   The code will work wihtout it, but it is used to get the graphical representations.
   SP tool can be downloaded from [here](http://www.math.bas.bg/bantchev/sp/sp).
   Place it in the `dep/sp` directory.

### Build Script

The repository includes a `build.sh` script for compiling solutions. It has two modes of operation:
- **Newest files Compilation**: By default, only files modified in the last 30 minutes are compiled.
- **Full Compilation**: Pass any argument to the script to compile all files.

**Building**:
```bash
cd tools/linux
./build.sh            # Newest files Compilation
./build.sh all        # Full build
```

**Usage**
```bash
cat src/notebook/inputs/triangle-and-point/valid-triangle-on-side-segment.txt |
    ./build/triangle-and-point -f sp                                          |
    ./dep/sp svg > build/test.svg
```
