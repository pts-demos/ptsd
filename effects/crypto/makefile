GENDEV=/opt/gendev

all: src

src: crypto_pts_images
	make -f $(GENDEV)/sgdk/mkfiles/makefile.gen

cryptogen:
	make -C utils/cryptogen all

crypto_pts_images: cryptogen
	cp utils/cryptogen/crypto_pts_images.* src/

clean:
	make -C utils/cryptogen clean
	rm -rf out
	rm -rf src/boot
	rm -f src/crypto_pts_images.*

.PHONY: cryptogen crypto_pts_images src clean
