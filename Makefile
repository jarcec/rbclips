all:  clips rbclips test

clips:
	make -C src/clips -f makefile.lib

rbclips:
	ruby -C src/ extconf.rb
	make -C src/
	
test:
	make -C tests/

thesis:
	make -C thesis/cz/

.PHONY: thesis

clean:
	make -C src/ clean
	make -C thesis/cz/ clean
	rm -rf src/clips/*.o libclips.a
