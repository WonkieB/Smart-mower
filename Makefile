EXEC_NAME = smart_mower

.PHONY: all
all: comp run

.PHONY: comp
comp: smart_mower 

smart_mower:
	g++ src/main.cpp -o $(EXEC_NAME)

.PHONY: run
run:
	@ ./$(EXEC_NAME)

.PHONY: clean
clean:
	@ rm $(EXEC_NAME)
