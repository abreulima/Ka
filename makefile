SRCS	:=	main.cpp \
			src/Scene.cpp \
			src/Entity.cpp \
			src/Engine.cpp \
			src/Backend.cpp \
			src/Resources.cpp \
			src/Renderer.cpp \
			src/Events.cpp

			
all: main.cpp
	clang++ -g $(SRCS) -o game -Iinc -lwebgpu_dawn -lSDL3 -lSDL3_ttf -lSDL3_image

web: $(SRCS)
	em++ -g $(SRCS) -sASYNCIFY=1 --use-port=sdl3 --use-port=emdawnwebgpu -sALLOW_MEMORY_GROWTH=1 --preload-file res -o out/web/index.html

run:
	emrun out/web/