objects = obj/main.o
headers = header/axis.h header/color_models.h header/image.h header/pixel.h header/plot2d.h

all: bplot

bplot: $(objects) $(headers)
	g++ -o bplot $(objects)

obj/main.o: src/main.cpp header/axis.h header/color_models.h header/image.h header/pixel.h header/plot2d.h
	g++ -c -w -std=c++20 -o obj/main.o src/main.cpp

clean:
	rm -rf $(objects)
