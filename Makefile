ALLEGRO_VERSION=5.0.10
MINGW_VERSION=4.7.0
FOLDER=C:\TP

FOLDER_NAME=\allegro-$(ALLEGRO_VERSION)-mingw-$(MINGW_VERSION)
PATH_ALLEGRO=$(FOLDER)$(FOLDER_NAME)
LIB_ALLEGRO=\lib\liballegro-$(ALLEGRO_VERSION)-monolith-mt.a
INCLUDE_ALLEGRO=\include

all: ffantasy.exe


ffantasy.exe: ffantasy.o
	gcc -o ffantasy.exe ffantasy.o $(PATH_ALLEGRO)$(LIB_ALLEGRO)

ffantasy.o: ffantasy.c
	gcc -I $(PATH_ALLEGRO)$(INCLUDE_ALLEGRO) -c ffantasy.c	
	
	
clean:
	del ffantasy.o
	del ffantasy.exe

