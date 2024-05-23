FC = gcc
LINKER = -Wall -std=c99 -pedantic

NAME1 = semaphore_maker
NAME2 = supply_manager

EXEC1 = $(NAME1).x
OBJS1 = $(NAME1).o

EXEC2 = $(NAME2).x
OBJS2 = $(NAME2).o


%.o: %.c
	$(FC) -c $<


%.x: %.o
	$(FC) $(LINKER) -o $@ $^
	chmod +x $@

$(EXEC1): $(OBJS1) 
	$(FC) $(LINKER) -o $@ $^
	chmod +x $@

$(EXEC2): $(OBJS2) 
	$(FC) $(LINKER) -o $@ $^
	chmod +x $@

run-cw6: $(EXEC1) $(EXEC2)
	./$(EXEC1) ./$(EXEC2) 3 8

.PHONY: all
all: $(EXEC1) $(EXEC2) 

help:
	@echo "make run-cw6		Kompiluje i uruchamia programy powielacz.c i wykluczanie.c"
	@echo "make all			Kompiluje programy powielacz.c i wykluczanie.c"
	@echo "make clean  		Usuwa pliki .x, .o"

.PHONY: clean
clean:
	@-rm *.x *.o  