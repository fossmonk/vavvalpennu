# Detect the Operating System
# On Windows, the OS environment variable is typically set to "Windows_NT"
ifeq ($(OS),Windows_NT)
    TARGET_MAKEFILE := Makefile.win
    RM := del /Q
else
    # On Unix-like systems, we query 'uname'
    UNAME_S := $(shell uname -s)
    ifeq ($(UNAME_S),Darwin)
        TARGET_MAKEFILE := Makefile.osx
    else
        # Optional: Handle Linux or fallback
        TARGET_MAKEFILE := Makefile.linux
    endif
    RM := rm -f
endif

.PHONY: all clean debug

# Default target forwards all arguments to the OS-specific Makefile
all:
	@echo "Detected OS. Forwarding build to $(TARGET_MAKEFILE)..."
	$(MAKE) -f $(TARGET_MAKEFILE)

# Clean target forwards to the OS-specific Makefile
clean:
	@echo "Forwarding clean to $(TARGET_MAKEFILE)..."
	$(MAKE) -f $(TARGET_MAKEFILE) clean

# Debug target forwards to the OS-specific Makefile
debug:
	@echo "Forwarding test to $(TARGET_MAKEFILE)..."
	$(MAKE) -f $(TARGET_MAKEFILE) debug