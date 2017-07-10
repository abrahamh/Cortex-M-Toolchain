# What is it
Makefile based project to build an custom toolchain for Cortex-M based on llvm/clang. 

# Features
- [x] makefile based project to build own custom toolchain for ARM Cortex-M
- [x] one makefile to define used versions (Makefile.def)
- [x] download required source (i.e. binutils, llvm ...)
- [x] support Cortex-M0, Cortex-M3 and Cortex-M4 (with hard-float)
- [x] pack results as *.tar.gz
- [x] support for build, analyze, editor plugins and code navigation 

# Usage
0. Adapt PROXY_SETTINGS_HTTP (Makefile.def) if you want to use proxy settings.
1. Download required sources by command `make download`.
2. Build toolchain by command `make`, as result you get an *tar.gz package.
3. Install tarball.

# Tests
- Tested with CentOS 7