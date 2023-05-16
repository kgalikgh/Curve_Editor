LIBS=-lsfml-graphics -lsfml-window -lsfml-system -ltgui

CXX := g++

all: CurveEditor 

tinyfiledialogs.o:
	gcc -c src/tinyfiledialogs.c -o tinyfiledialogs.o

%.o: src/%.cpp
	$(CXX) -c $< -o $@

%.o: src/%.hpp
	$(CXX) -c $< -o $@

CurveEditor: tinyfiledialogs.o curve_functions.o point.o curve.o main.o
	@echo "** Building editor"
	$(CXX) -O2 -Wall -Wextra -o CurveEditor main.o point.o curve.o curve_functions.o tinyfiledialogs.o $(LIBS)

clean:
	@echo "** Removing object files and executable..."
	rm -f CurveEditor *.o

 
