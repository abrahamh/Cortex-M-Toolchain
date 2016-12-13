# What is it
Xcode project to build an custom toolchain for Cortex-M based on llvm/clang. 

# Features
- [x] makefile based project to build own custom toolchain for ARM Cortex-M
- [x] one makefile to define used versions (Makefile.def)
- [x] download required source (i.e. binutils, llvm ...)
- [x] support Cortex-M0, Cortex-M3 and Cortex-M4 (with hard-float)
- [x] compatibility with Xcode SourceKit
- [x] pack results as *.tar.gz
- [x] support for build, analyze, editor plugins and code navigation 
- [ ] debug by using Segger Ozone as external debugger
- [ ] project template for static library (planed for future)
- [ ] project template for libc (planed for future)
- [ ] project template for compiler_rt (planed for future)
- [ ] project template for CMSIS and CMSIS drivers (planed for future)
- [ ] project template for simple application (planed for future)
- [ ] support for semihosting (planed for future)
- [ ] support for unit testing (planed for future)
- [ ] enable debug with xcode by using lldb plugin (planed for future)

# Usage
0. Adapt PROXY_SETTINGS_HTTP (Makefile.dlf) if you want to use proxy settings.
1. Select scheme `download` and build to download required source.
2. Select scheme `build-toolchain` and build to build custom toolchain, as result you get an *tar.gz package.
3. Select scheme `install-toolchain` and build to install toolchain for local user.

# Tests
- Tested with macOS 10.11 and Xcode 8.0