
OPTIONS_FILE ?= Makefile.options.me

-include $(OPTIONS_FILE)
-include Makefile.options

MODULE_ROLES  = $(addsuffix .%, $(MODULES)) $(MODULES)
KMODULE_ROLES = $(addsuffix .%, $(KMODULES)) $(KMODULES)
ROLES = $(MODULE_ROLES) $(KMODULE_ROLES)

.PHONY: all clean rebuild distclean run restart test test.%
.PHONY: $(ROLES)

OPTIONS       ?=

# Basic
GLOBAL        ?= default
ARCH		  ?= x86_64
OS			  ?= linux

# Paths
BASE_PATH	  ?= $(shell pwd)
MODULES_PATH  ?= $(BASE_PATH)/modules
KMODULES_PATH ?= $(BASE_PATH)
INC_PATH      ?= $(shell find $(BASE_PATH)/include -maxdepth 1 -type d)
ARCH_PATH     ?= $(BASE_PATH)/arch/$(ARCH)
OS_PATH		  ?= $(BASE_PATH)/os/$(OS)
GLOBAL_PATH   ?= $(BASE_PATH)/global/$(GLOBAL)
BIN_PATH	  ?= $(BASE_PATH)/bin
LIBS_PATH     ?= $(BASE_PATH)/libs
TEST_PATH     ?= $(BASE_PATH)/test

# Args
__ARGS  = BASE_PATH="$(BASE_PATH)" INC_PATH="$(INC_PATH)"
__ARGS += ARCH=$(ARCH) OS=$(OS) GLOBAL=$(GLOBAL)
__ARGS += ARCH_PATH="$(ARCH_PATH)" OS_PATH="$(OS_PATH)" GLOBAL_PATH="$(GLOBAL_PATH)"
__ARGS += BIN_PATH="$(BIN_PATH)" LIBS_PATH="$(LIBS_PATH)"
__ARGS += GLOBAL_LDFLAGS="$(GLOBAL_LDFLAGS)" GLOBAL_CPPFLAGS="$(GLOBAL_CPPFLAGS)"
__ARGS += GLOBAL_LIBS="$(GLOBAL_LIBS)"
__ARGS += KMODULES="$(KMODULES)"
__ARGS += RUN_ARGS="$(RUN_ARGS)"

__DEFAULT_TARGET_TYPE = static
__BUILD_MODULE_ARGS   = DEFAULT_TARGET_TYPE=$(__DEFAULT_TARGET_TYPE) $(__ARGS)
__BUILD_KMODULE_ARGS  = DEFAULT_TARGET_TYPE=$(__DEFAULT_TARGET_TYPE) $(__ARGS)
__BUILD_TEST_ARGS     = DEFAULT_TARGET_TYPE=execute MODULES="$(MODULES) $(KMODULES)" $(__ARGS)
__CONFIGS             = CONFIG_NULLPTR_SUPPORTED=$(CONFIG_NULLPTR_SUPPORTED) \
                        CONFIG_BASE_NAMESPACE=$(CONFIG_BASE_NAMESPACE)

# all
all: $(KMODULES) $(MODULES)

$(KMODULES):
	$(MAKE) $(OPTIONS) -C $(KMODULES_PATH)/$@ all MODULE=$@ $(__BUILD_KMODULE_ARGS) $(__CONFIGS);

$(addsuffix .%, $(KMODULES)):
	PROJECT=$(shell echo "$@" | awk -F '.' '{print $$1}');  					\
	METHOD=$(shell echo "$@" | awk -F '.' '{print $$2}');						\
	$(MAKE) $(OPTIONS) -C $(KMODULES_PATH)/$$PROJECT $$METHOD MODULE=$$PROJECT 	\
			$(__BUILD_KMODULE_ARGS) $(__CONFIGS);

$(MODULES):
	$(MAKE) $(OPTIONS) -C $(MODULES_PATH)/$@ all MODULE=$@ $(__BUILD_MODULE_ARGS) $(__CONFIGS);

$(addsuffix .%, $(MODULES)):
	PROJECT=$(shell echo "$@" | awk -F '.' '{print $$1}');  					\
	METHOD=$(shell echo "$@" | awk -F '.' '{print $$2}');						\
	$(MAKE) $(OPTIONS) -C $(MODULES_PATH)/$$PROJECT $$METHOD MODULE=$$PROJECT 	\
			$(__BUILD_MODULE_ARGS) $(__CONFIGS);

# clean distclean rebuild restart
clean distclean rebuild restart:
	@for module in $(KMODULES); do				                        		\
		$(MAKE) $(OPTIONS) -C $(KMODULES_PATH)/$$module $@              		\
                MODULE=$$module $(__BUILD_MODULE_ARGS) $(__CONFIGS); 			\
	done;
	@for module in $(MODULES); do				                        		\
		$(MAKE) $(OPTIONS) -C $(MODULES_PATH)/$$module $@               		\
                MODULE=$$module $(__BUILD_MODULE_ARGS) $(__CONFIGS);    		\
	done;

# test
test: test.default

# test.%
test.%:
	PROJECT=$(shell echo "$@" | awk -F '.' '{print $$2}');  					\
	METHOD=$(shell echo "$@" | awk -F '.' '{print $$3}');						\
	$(MAKE) $(OPTIONS) -C $(TEST_PATH)/$$PROJECT $$METHOD MODULE=test$$PROJECT 	\
            $(__BUILD_TEST_ARGS) $(__CONFIGS);

