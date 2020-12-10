NAME = "Frog"

all:
	@echo "Compiling..."
	g++ -o $(NAME) $(NAME).cpp -L/opt/X11/lib -lX11 -lstdc++
run: all
	@echo "Running..."
	./$(NAME) 
clean:
	-rm *o
