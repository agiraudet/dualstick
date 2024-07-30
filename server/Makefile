NAME            := s

DEPS            := -lSDL2 -lenet

CMN_DIR         := ../common
INC_DIR         := ./inc $(CMN_DIR)/inc

CMN_BUILD_DIR   := $(CMN_DIR)/build
CMN_SRCS_DIR    := $(CMN_DIR)/src
CMN_SRCS        := Player.cpp \
									 Vector.cpp
CMN_OBJS        := $(addprefix $(CMN_BUILD_DIR)/, $(CMN_SRCS:%.cpp=%.o))

BUILD_DIR       := ./build
SRCS_DIR        := ./src
SRCS            := Server.cpp \
                   User.cpp \
                   main.cpp
OBJS            := $(addprefix $(BUILD_DIR)/, $(SRCS:%.cpp=%.o))

INC_FLAGS       := $(addprefix -I, $(INC_DIR))
CXXFLAGS        := -MD -Wall -Wextra $(INC_FLAGS) -g
CXX             := g++

$(NAME): $(OBJS) $(CMN_OBJS)
	$(CXX) $(CXXFLAGS) $(OBJS) $(CMN_OBJS) $(DEPS) -o $@

$(BUILD_DIR):
	mkdir -p $(BUILD_DIR)

$(CMN_BUILD_DIR):
	mkdir -p $(CMN_BUILD_DIR)

$(BUILD_DIR)/%.o: $(SRCS_DIR)/%.cpp | $(BUILD_DIR)
	$(CXX) $(CXXFLAGS) -c $< -o $@

$(CMN_BUILD_DIR)/%.o: $(CMN_SRCS_DIR)/%.cpp | $(CMN_BUILD_DIR)
	$(CXX) $(CXXFLAGS) -c $< -o $@

all: $(NAME)

clean:
	rm -rf $(BUILD_DIR)
	rm -rf $(CMN_BUILD_DIR)

fclean: clean
	rm -f $(NAME)

re: fclean all

-include $(OBJS:.o=.d)
-include $(CMN_OBJS:.o=.d)

.PHONY: all clean fclean re
