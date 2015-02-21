rwildcard		=	$(foreach d,$(wildcard $1*),$(call rwildcard,$d/,$2)\
					$(filter $(subst *,%,$2),$d))

NAME			=	webserver_test

COMPILER		=	g++-4.9
BOOST_DIR		=	~/.brew/Cellar/boost/1.57.0/include/
BOOST_LIB_DIR	=	~/.brew/Cellar/boost/1.57.0/lib
CFLAGS			=	-Wall -Wextra -Werror -std=c++1y -O3 -c\
					-I $(BOOST_DIR) -I .
LFLAGS			=	-L $(BOOST_LIB_DIR) -lboost_system -o

PCH				=	stdafx.hpp
PCH_SUFFIX		=	.gch
PCH_DIR			=	/tmp/pch_$(NAME)
PCH_TARGET		=	$(PCH_DIR)$(PCH_SUFFIX)/pch
PCH_FLAG		=	-include $(PCH_DIR)

OBJ_DIR			=	objs
SRC				=	$(call rwildcard, ./, *.cpp)
OBJ				=	$(addprefix $(OBJ_DIR)/, $(SRC:.cpp=.o))

OBJ_SUB_DIRS	=	$(dir $(OBJ))

all: $(NAME)

$(NAME): $(OBJ)
	@echo "linking $@"
	@$(COMPILER) $(OBJ) $(LFLAGS) $@

$(PCH_TARGET): $(PCH)
	@$(MAKE) $(OBJ_DIR)
	@mkdir -p $(PCH_DIR)$(PCH_SUFFIX)
	@echo "precompiling headers"
	@$(COMPILER) $(CFLAGS) $< -o $@

$(OBJ): | $(OBJ_DIR)

$(OBJ_DIR):
	@$(foreach dir, $(OBJ_SUB_DIRS), mkdir -p $(dir);)

$(OBJ_DIR)/%.o: %.cpp $(PCH_TARGET)
	@echo "compiling $(notdir $<)"
	@$(COMPILER) $(CFLAGS) $(PCH_FLAG) $< -o $@

depend: .depend

.depend: $(SRC)
	@echo "building dependencies"
	@rm -f ./.depend
	@$(foreach src, $^, $(COMPILER) $(CFLAGS) -MM -MT $(OBJ_DIR)/./$(src:.cpp=.o) $(src) >> ./.depend;)

-include .depend

clean:
	@echo "cleaning objects"
	@rm -rf $(OBJ_DIR)
	@rm -f ./.depend

fclean: clean
	@echo "cleaning $(NAME)"
	@rm -f $(NAME)

re: fclean all
