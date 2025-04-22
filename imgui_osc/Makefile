LIBS=-ldl `pkgconf --libs sdl3` -L/usr/lib/ -limgui
CXXFLAGS=-std=c++17 -I./include -g -Wall -Wformat
CXXFLAGS+=`pkgconf sdl3 --cflags`
all: 
	g++ $(CXXFLAGS) $(LIBS) -o imgui_osc src/main.cpp src/imgui_impl_sdl3.cpp src/imgui_impl_sdlrenderer3.cpp
