
NAME = watersim

SRC = *.cpp voxGL/*.cpp

FRAMEWORKS = -framework OpenGl

BREW_INC = -I /nfs/2017/t/twalton/.brew/include

GLFW_LINK = -L/nfs/2017/t/twalton/.brew/lib -lglfw

MY_INC = -I . -I ./voxGL

FLAGS = -Wfatal-errors -std=c++11 -O3

make:
	g++ -g $(FLAGS) $(SRC) -o $(NAME) $(FRAMEWORKS) $(MY_INC) $(BREW_INC) $(GLFW_LINK)

fclean:
	rm $(NAME)
