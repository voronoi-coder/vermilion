target = $(shell ./target.sh)

default: run-target

COMPILE_COMMANDS = build/compile_commands.json

$(COMPILE_COMMANDS): CMakeLists.txt
	cd build && cmake ../

.PHONY: run-target
run-target: $(COMPILE_COMMANDS)
	cmake --build build --target $(target)
	./build/$(target)

.PHONY: compile_commands
compile_commands:
	cp -rf ./build/compile_commands.json ./
