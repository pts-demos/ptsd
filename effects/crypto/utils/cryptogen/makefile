

all: crypto_c_file

cryptogen: cryptogen.c pts.h
	gcc cryptogen.c -o cryptogen

crypto_c_file: cryptogen
	./cryptogen > crypto_pts_images.c

clean:
	rm -f cryptogen
	rm -f crypto_pts_images.c

.PHONY: clean

