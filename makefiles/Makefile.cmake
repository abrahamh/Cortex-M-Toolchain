#
# Makefile - build xzcat
# Copyright (C) 2015-2016 Heiko Abraham
# All rights reserved.
#

include Makefile.def

.PHONY: build
build:
	env
	$(MAKE) -f $(MAKEFILENAME) $(PREFIX_HOST)/bin/cmake


.PHONY: clean
clean:
	$(RM) -f $(PREFIX_HOST)/bin/cmake


.PHONY: install
install:


$(PREFIX_HOST)/bin/cmake: $(MAKEFILE_LIST)
	$(MAKE) -f $(MAKEFILENAME) build_host_cmake


.PHONY: build_host_cmake
build_host_cmake: $(BUILD_DIR)
	$(RM) -rf $(BUILD_DIR)/$(CMAKE_VERSION)
	tar -xzf $(DOWNLOAD_DIR)/$(CMAKE_VERSION).tar.gz       -C $(BUILD_DIR)
	( cd $(BUILD_DIR)/$(CMAKE_VERSION) && ./configure \
		--prefix=$(PREFIX_HOST) \
		--no-system-libs \
		--no-qt-gui \
		--parallel=$(NUM_CPUS) )
	( cd $(BUILD_DIR)/$(CMAKE_VERSION) && $(MAKE) $(MJOBS) )
	( cd $(BUILD_DIR)/$(CMAKE_VERSION) && $(MAKE) install )
	$(RM) -rf $(BUILD_DIR)/$(CMAKE_VERSION)
