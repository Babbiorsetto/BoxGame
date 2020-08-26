# tool macros
CC := gcc
CCFLAG :=
DBGFLAG := -g
CCOBJFLAG := $(CCFLAG) -c
TESTFLAG := -Ilibs/munit -Isrc

# path macros
BIN_PATH := bin
OBJ_PATH := obj
SRC_PATH := src
DBG_PATH := debug
TEST_PATH := test

# compile macros
CLIENT := $(BIN_PATH)/client
SERVER := $(BIN_PATH)/server
TEST := $(BIN_PATH)/test

# src files & obj files
SRC := $(foreach x, $(SRC_PATH), $(wildcard $(addprefix $(x)/*,.c*)))
OBJ := $(addprefix $(OBJ_PATH)/, $(addsuffix .o, $(notdir $(basename $(SRC)))))
TESTSRC := $(foreach x, $(TEST_PATH), $(wildcard $(addprefix $(x)/*,.c*)))

# clean files list
DISTCLEAN_LIST := $(OBJ)
CLEAN_LIST := $(CLIENT) \
			  $(SERVER) \
			  $(TEST) \
			  $(DISTCLEAN_LIST)

# default rule
default: none

# non-phony targets
$(SERVER): $(OBJ)
	$(CC) $(CCFLAG) -o $@ $^

$(CLIENT):
	$(CC) $(CCFLAG) -o $@ $^

$(TEST): $(OBJ_PATH)/player_alias.o $(OBJ_PATH)/player_list.o $(OBJ_PATH)/game_map.o $(OBJ_PATH)/personal_map.o libs/munit/munit.c $(TESTSRC)
	$(CC) $(CCFLAG) $(TESTFLAG) -o $@ $^

$(OBJ_PATH)/%.o: $(SRC_PATH)/%.c*
	$(CC) $(CCOBJFLAG) -o $@ $<

$(DBG_PATH)/%.o: $(SRC_PATH)/%.c*
	$(CC) $(CCOBJFLAG) $(DBGFLAG) -o $@ $<

$(TARGET_DEBUG): $(OBJ_DEBUG)
	$(CC) $(CCFLAG) $(DBGFLAG) $? -o $@

# phony rules
.PHONY: none
none:
	@echo 'specify a target: bin/client, bin/server, bin/test, clean, distclean'

.PHONY: clean
clean:
	@echo CLEAN $(CLEAN_LIST)
	@rm -f $(CLEAN_LIST)

.PHONY: distclean
distclean:
	@echo CLEAN $(DISTCLEAN_LIST)
	@rm -f $(DISTCLEAN_LIST)
