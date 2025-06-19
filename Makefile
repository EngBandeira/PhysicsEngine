buildP = build
includeP = include
srcP = src

executable = $(buildP)/final

source = $(shell find ./$(srcP) -path ./src/steps/utils/dontcompile -prune -o -name '*.cpp' -print)

#objects = $(addprefix $(buildP), $(basename $(addprefix  $(dir $(source)), $(addsuffix .o , $(basename $(notdir $(source)))))))
objects = $(subst $(srcP),$(buildP),$(addsuffix .o ,$(basename $(source))))


libs = GL GLU glfw GLEW pthread

flagsF = -Wall -I./$(includeP) -g -ggdb $(addprefix -l, $(libs))

flagsI = -Wall -I./$(includeP) -I./$(includeP)/modules -g -ggdb $(addprefix -l, $(libs))

CC = g++

%.o :
	$(CC) $(flagsI) $(subst $(buildP),$(srcP),$(addsuffix .cpp ,$(basename $@))) -c -o $@

$(executable):	$(objects)
	$(CC) $(flagsF) $^ -o $@

clean:
	rm $(shell find ./$(buildP) -name '*.o') ./$(buildP)/final

all: $(executable)
	@echo buildado

create:
	$(shell mkdir -p $(dir $(objects)))
	$(shell mkdir -p $(dir $(subst $(srcP),$(includeP), $(source))))

init:
	$(shell mkdir -p $(srcP) $(includeP) $(buildP))
