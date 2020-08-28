# tool macros
CC := gcc
CCFLAG := -pthread
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
SERVER_DEBUG := $(DBG_PATH)/server
CLIENT_DEBUG := $(DBG_PATH)/client

# src files & obj files
SRC := $(foreach x, $(SRC_PATH), $(wildcard $(addprefix $(x)/*,.c*)))
OBJ := $(addprefix $(OBJ_PATH)/, $(addsuffix .o, $(notdir $(basename $(SRC)))))
TESTSRC := $(foreach x, $(TEST_PATH), $(wildcard $(addprefix $(x)/*,.c*)))
OBJ_DEBUG := $(addprefix $(DBG_PATH)/, $(addsuffix .o, $(notdir $(basename $(SRC)))))

# clean files list
DISTCLEAN_LIST := $(OBJ) \
				  $(OBJ_DEBUG)
CLEAN_LIST := $(CLIENT) \
			  $(SERVER) \
			  $(TEST) \
			  $(DISTCLEAN_LIST)

# default rule
default: none

# non-phony targets
$(SERVER): $(OBJ_PATH)/player_alias.o $(OBJ_PATH)/player_list.o $(OBJ_PATH)/game_map.o $(OBJ_PATH)/personal_map.o $(OBJ_PATH)/server.o
	$(CC) $(CCFLAG) -o $@ $^

$(CLIENT): $(OBJ_PATH)/client.o
	$(CC) $(CCFLAG) -o $@ $^

$(TEST): $(OBJ_PATH)/player_alias.o $(OBJ_PATH)/player_list.o $(OBJ_PATH)/game_map.o $(OBJ_PATH)/personal_map.o libs/munit/munit.c $(TESTSRC)
	$(CC) $(CCFLAG) $(TESTFLAG) -o $@ $^

$(OBJ_PATH)/%.o: $(SRC_PATH)/%.c*
	$(CC) $(CCOBJFLAG) -o $@ $<

$(DBG_PATH)/%.o: $(SRC_PATH)/%.c*
	$(CC) $(CCOBJFLAG) $(DBGFLAG) -o $@ $<

$(SERVER_DEBUG): $(DBG_PATH)/player_alias.o $(DBG_PATH)/player_list.o $(DBG_PATH)/game_map.o $(DBG_PATH)/personal_map.o $(DBG_PATH)/server.o
	$(CC) $(CCFLAG) $(DBGFLAG) $^ -o $@

$(CLIENT_DEBUG): $(DBG_PATH)/client.o
	$(CC) $(CCFLAG) $(DBGFLAG) $^ -o $@	

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
