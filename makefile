CXX			= clang++
EMXX		= em++

NAME		= game

SRCS		= \
			main.cpp \
			src/Scene.cpp \
			src/Entity.cpp \
			src/Engine.cpp \
			src/Backend.cpp \
			src/Resources.cpp \
			src/Renderer.cpp \
			src/Events.cpp \

BUILDIR		= build
OBJS		= $(SRCS:%.cpp=$(BUILDIR)/%.o)
DEPS		= $(SRCS:%.cpp=$(BUILDIR)/%.d)

CPPFLAGS	= -Iinc -Iinc/lua
CXXFLAGS	= -std=c++23 -O3 -g -Wall -Wextra -Wpedantic -MMD -MP
LDLIBS		= -lwebgpu_dawn -lSDL3 -lSDL3_ttf -lSDL3_image -llua5.4

.PHONY: 	all web run clean

all: $(NAME)

$(NAME): $(OBJS)
	@mkdir -p $(dir $@)
	$(CXX) $(OBJS) -o $@ $(LDLIBS)

$(BUILDIR)/%.o: %.cpp
	@mkdir -p $(dir $@)
	$(CXX) $(CPPFLAGS) $(CXXFLAGS) -MMD -MP -MF $(BUILDIR)/$*.d -c $< -o $@

web: $(SRCS)
	em++ -g $(SRCS) -sASYNCIFY=1 --use-port=sdl3 --use-port=emdawnwebgpu -sALLOW_MEMORY_GROWTH=1 --preload-file res -o out/web/index.html

run:
	emrun out/web/

clean:
	rm -rf $(BUILDIR) $(NAME)

-include $(DEPS)
