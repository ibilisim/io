#
# An example of how to build a Player plugin with libtool
#

CXX = g++
CXXLD = gcc
LIBTOOL = libtool
PLAYER_CFLAGS = `pkg-config --cflags playercore`
INSTALL_DIR = /tmp

all: libincidriver.la

libincidriver.la: player-driver.lo seriport.lo io.lo pid.lo inci.lo
	$(LIBTOOL) --mode=link $(CXXLD) -rpath $(INSTALL_DIR) -module -export-dynamic -version-info 0:0:0 $^ -o $@

%.lo: %.cc
	$(LIBTOOL) --mode=compile $(CXX) -c $(PLAYER_CFLAGS) $<

clean:
	rm -rf *.o *.lo *.la .libs zozo


zozo: *.cc *.h
	g++ -Wall -O2 -o zozo seriport.cc io.cc pid.cc inci.cc test.cc
