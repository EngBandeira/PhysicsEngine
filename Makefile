buildP = build
includeP = include
vendorP = vendor
srcP = src

executable = $(buildP)/final

sourceCPP = $(shell find ./$(srcP) ./$(vendorP) -path ./src/steps/utils/dontcompile -prune -o -name '*.cpp' -print) 
sourceC = $(shell find ./$(srcP) ./$(vendorP) -path ./src/steps/utils/dontcompile -prune -o -name '*.c' -print)
#objects = $(addprefix $(buildP), $(basename $(addprefix  $(dir $(sourceCPP)), $(addsuffix .o , $(basename $(notdir $(sourceCPP)))))))
objects = $(subst $(srcP),$(buildP),$(addsuffix .o ,$(basename $(sourceCPP))))


libs = GL GLU glfw GLEW pthread

flagsF = -Wall -I./$(includeP) -I./$(vendorP) -g -ggdb $(addprefix -l, $(libs))

flagsI = -Wall -I./$(includeP) -I./$(vendorP) -I./$(includeP)/modules -g -ggdb $(addprefix -l, $(libs))

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
	$(shell mkdir -p $(dir $(subst $(srcP),$(includeP), $(sourceCPP
))))

init:
	$(shell mkdir -p $(srcP) $(includeP) $(buildP))
