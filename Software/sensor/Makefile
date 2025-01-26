# Attention for editors: You have to use tab-characters for command indentation!
# vim: set noexpandtab:

DEVICE_TTY      ?= /dev/ttyACM1

BAUDRATE        := 115200
PROCESSOR       := teensy40
BOARD           := teensy:avr:$(PROCESSOR):usb=serial2
HEXFILE         ?= build/sensor.ino.hex

BUILD_DIR_ARD   := build/

.PHONY: default
default: build

.PHONY: help
help:
	@echo "Help:"
	@echo ""
	@echo "$(MAKE) build         - just build"
	@echo "$(MAKE) deploy        - build and upload to teensy"
	@echo "$(MAKE) deployNoBuild - just upload to teensy"
	@echo "$(MAKE) monitor       - monitor $(DEVICE_TTY)"

.PHONY: clean
clean:
	rm -rf $(BUILD_DIR_ARD)

.PHONY: build
build:
	arduino-cli compile --jobs 8 --build-path $(BUILD_DIR_ARD) --build-cache-path /tmp/arduino \
	--fqbn $(BOARD) \
	--build-property compiler.cpp.flags="-Wall" \
	-v . && echo "Build OK"

.PHONY: deploy
deploy: build
	teensy_loader_cli -v --mcu=teensy40 -w $(HEXFILE)
.PHONY: deployNoBuild
deployNoBuild:
	teensy_loader_cli -v --mcu=teensy40 -w $(HEXFILE)

# get info via arduino-cli board list
.PHONY: monitor
monitor:
	arduino-cli monitor --fqbn teensy:avr:teensy40 --port usb3/3-4/3-4.2 --protocol teensy

.PHONY: testSetup
testSetup:
	@echo -n "Testing for arduino-cli: " && (which arduino-cli) || (echo "FAILED - Please install arduino-cli" && exit 1)
	@echo "Testing board files " && (arduino-cli config get board_manager.additional_urls | grep -q "package_teensy_index.json") || echo ("FAILED - Please run darduino-cli config add board_manager.additional_urls https://www.pjrc.com/teensy/package_teensy_index.json" && exit 1)
	@echo "Testing for BOARD platform " && (arduino-cli core list | grep -q "teensy:avr") || (echo "FAILED - Please run arduino-cli core install teensy:avr" && exit 1)
	@echo "Testing for DebugLog " && (arduino-cli lib list | grep -q "DebugLog") || (echo "FAILED - Please run arduino-cli lib install DEBUGLOG" && exit 1)
	@echo "Setup seems to be ok."

.PHONY: setTime
setTime:
	@date -u +T%s > /dev/ttyACM0
