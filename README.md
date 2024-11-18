Depends on curl and clang.

TODO: Extend this file with more explanations.

# Computational Geometry

This is a repository meant for the assignments, given in the subject of **Computational Geometry**, in the masters degree **Computer graphics**.
Its main purpose are:
 - public study
 - notes keeping
 - knowledge sharing

---

## Repository Structure
<ul>
    <li>build</li>
    <li>dep</li>
    <ul>
        <li>sp</li>
        <li>vecta.h</li>
    </ul>
    <li>README.md</li>
    <li>src</li>
    <li>notebook</li>
    <ul>
        <li>inputs</li>
        <li>platform</li>
        <ul>
            <li>linux</li>
        </ul>
    </ul>
    <li>tools</li>
    <ul>
        <li>linux</li>
    </ul>
</ul>

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

**Usage**:
```bash
cd tools/linux
./build.sh            # Newest files Compilation
./build.sh all        # Full build
