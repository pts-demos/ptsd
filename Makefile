GENDEV=/opt/gendev
export GENDEV
.PHONY: clean all
CLEANFILES=	src/crypto_pts_images.c

all: out/rom.bin

clean:
	$(MAKE) -f $(GENDEV)/sgdk/mkfiles/makefile.gen clean
	rm -f $(CLEANFILES)

out/rom.bin: src/*.c src/crypto_pts_images.c
	$(MAKE) -C utils/cryptogen
	$(MAKE) -f $(GENDEV)/sgdk/mkfiles/makefile.gen all

src/crypto_pts_images.c: utils/cryptogen/cryptogen
	utils/cryptogen/cryptogen > $@

utils/cryptogen/cryptogen:
	$(MAKE) -C utils/cryptogen cryptogen
