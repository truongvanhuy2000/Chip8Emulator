.PHONY: all clean count
buildCmake:
	cmake -B build
runCmake:
	@cd build/apps &&\
	./emulator