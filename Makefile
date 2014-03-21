WORK_DIR = $(shell pwd)
BIN_DIR  = $(WORK_DIR)/bin

all:
	@if [ ! -d $(BIN_DIR) ]; \
		then echo "mkdir -p $(BIN_DIR)"; mkdir -p $(BIN_DIR); \
		fi
	cd $(WORK_DIR)/mesher; make; cp mesh $(BIN_DIR)
	cd $(WORK_DIR)/meshvis; make; cp meshvis $(BIN_DIR)

clean:
	cd $(WORK_DIR)/mesher; make clean;
	cd $(WORK_DIR)/meshvis; make clean;
	$(RM) $(BIN_DIR)/mesh $(BIN_DIR)/meshvis
