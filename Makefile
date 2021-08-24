CC		:= gcc
CFLAGS		:= -O3 -I./include
LDLIBS		:= 
OBJ		:= obj
BIN		:= bin
DEST		:= dest

TARGET		:= readbytes
TARGET2		:= extract
TARGET3		:= genBig
TARGET4		:= compress
TARGET5		:= genExample

MAIN_SRC	:= $(TARGET:%=%.c)
MAIN2_SRC	:= $(TARGET2:%=%.c)
MAIN3_SRC	:= $(TARGET3:%=%.c)
MAIN4_SRC	:= $(TARGET4:%=%.c)
MAIN5_SRC	:= $(TARGET5:%=%.c)
TARGET_DEPS	:= binutils  readHeader 
TARGET2_DEPS	:= binutils  readHeader doHeader
TARGET4_DEPS	:= binutils  readHeader doHeader
TARGET_OBJS 	:= $(foreach f, $(TARGET_DEPS), $(OBJ)/$(f).o) $(OBJ)/$(MAIN_SRC:%.c=%.o)
TARGET2_OBJS 	:= $(foreach f, $(TARGET2_DEPS), $(OBJ)/$(f).o) $(OBJ)/$(MAIN2_SRC:%.c=%.o)
TARGET3_OBJS 	:= $(foreach f, $(TARGET3_DEPS), $(OBJ)/$(f).o) $(OBJ)/$(MAIN3_SRC:%.c=%.o)
TARGET4_OBJS 	:= $(foreach f, $(TARGET4_DEPS), $(OBJ)/$(f).o) $(OBJ)/$(MAIN4_SRC:%.c=%.o)
TARGET5_OBJS 	:= $(foreach f, $(TARGET5_DEPS), $(OBJ)/$(f).o) $(OBJ)/$(MAIN5_SRC:%.c=%.o)

VPATH	:= src

all: $(BIN)/$(TARGET) $(BIN)/$(TARGET2) $(BIN)/$(TARGET3) $(BIN)/$(TARGET4) $(BIN)/$(TARGET5)
.PHONY: example archive

example:
	$(BIN)/$(TARGET5)

archive: $(DEST)/sizes.tarcmd.tar $(DEST)/sizes.compress.tar $(DEST)/exceptional.tarcmd.tar $(DEST)/exceptional.compress.tar

$(DEST)/sizes.tarcmd.tar: example
	@mkdir -p $(dir $@)
	time tar cpf $@ sizes

$(DEST)/sizes.compress.tar: example
	@mkdir -p $(dir $@)
	time ./bin/compress $@ sizes

$(DEST)/exceptional.tarcmd.tar: example
	@mkdir -p $(dir $@)
	time tar cpf $@ exceptional

$(DEST)/exceptional.compress.tar: example
	@mkdir -p $(dir $@)
	time ./bin/compress $@ exceptional



$(BIN)/$(TARGET): $(TARGET_OBJS)
	@mkdir -p $(dir $@)
	$(CC) -o $@ $(LDFLAGS) $^ $(LDLIBS)

$(BIN)/$(TARGET2): $(TARGET2_OBJS)
	@mkdir -p $(dir $@)
	$(CC) -o $@ $(LDFLAGS) $^ $(LDLIBS)

$(BIN)/$(TARGET3): $(TARGET3_OBJS)
	@mkdir -p $(dir $@)
	$(CC) -o $@ $(LDFLAGS) $^ $(LDLIBS)

$(BIN)/$(TARGET4): $(TARGET4_OBJS)
	@mkdir -p $(dir $@)
	$(CC) -o $@ $(LDFLAGS) $^ $(LDLIBS)

$(BIN)/$(TARGET5): $(TARGET5_OBJS)
	@mkdir -p $(dir $@)
	$(CC) -o $@ $(LDFLAGS) $^ $(LDLIBS)


$(OBJ)/%.o: %.c
	@mkdir -p $(dir $@)
	$(CC) -c -o $@ $(CFLAGS) $<

.PHONY: clean
clean:
	@-rm -rfv *~ $(BIN) $(OBJ) `find . -name *~`
