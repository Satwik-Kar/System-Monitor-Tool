CXX = g++
CXXFLAGS = -Iexternal/imgui -Iexternal/imgui-sfml -Iinclude -std=c++17 -Wall

IMGUI_SRC = external/imgui/imgui.cpp \
            external/imgui/imgui_draw.cpp \
            external/imgui/imgui_tables.cpp \
            external/imgui/imgui_widgets.cpp \
            external/imgui/imgui_demo.cpp

IMGUI_SFML_SRC = external/imgui-sfml/imgui-SFML.cpp

SRC = src/main.cpp \
      src/chip_renderer.cpp \
      src/ui_header.cpp \
      src/ui_cpu_window.cpp

LIBS = -lsfml-graphics -lsfml-window -lsfml-system -lGL

TARGET = sysmon

all:
	$(CXX) $(CXXFLAGS) $(SRC) $(IMGUI_SRC) $(IMGUI_SFML_SRC) -o $(TARGET) $(LIBS)
	./sysmon

clean:
	rm -f $(TARGET)
