LIBS=-lsfml-graphics -lsfml-window -lsfml-system -ltgui

CXX := g++

all: CurveEditor 

%.o: src/%.cpp
	$(CXX) -c $< -o $@

%.o: src/%.hpp
	$(CXX) -c $< -o $@

CurveEditor: point.o curve.o main.o
	@echo "** Building editor"
	$(CXX) -o CurveEditor main.o point.o curve.o $(LIBS)

clean:
	@echo "** Removing object files and executable..."
	rm -f CurveEditor *.o

 