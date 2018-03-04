
NAME = voxGL

SRC = *.cpp

FRAMEWORKS = -framework OpenGl

GLFW_INC = -I /nfs/2017/t/twalton/.brew/include

GLFW_LINK = -L/nfs/2017/t/twalton/.brew/lib -lglfw

MY_INC = -I .

FLAGS = -Wfatal-errors -std=c++11 -O3

make:
	g++ -g $(FLAGS) $(SRC) -o $(NAME) $(FRAMEWORKS) $(MY_INC) $(GLFW_INC) $(GLFW_LINK)

fclean:
	rm $(NAME)
