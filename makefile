# Variables
PROJECT  := resource

CUR_DIR  := $(shell cd)
INC_DIR  := ${CUR_DIR}
SRC_DIR  := ${CUR_DIR}
OUT_DIR  ?= ${CUR_DIR}\_output

HEADERS  := $(wildcard ${INC_DIR}/*.h)
SOURCES  := $(wildcard ${SRC_DIR}/*.c)
OBJECTS  := $(patsubst ${SRC_DIR}/%.c,${OUT_DIR}/${PROJECT}_%.o,${SOURCES})
BINARY   := ${OUT_DIR}/${PROJECT}.dll
IMPLIB   := ${OUT_DIR}/${PROJECT}.a

CPPFLAGS += -I${INC_DIR}
CFLAGS   += -g -Wall
#CFLAGS  += -mno-ms-bitfields
LDFLAGS  += -g -shared
#LDFLAGS += -s -shared --dll --out-implib ${IMPLIB}

# Commands
CC    = gcc
LD    = ld
ECHO  = echo
MKDIR = mkdir
RMDIR = rmdir

# Rules
all : ${OUT_DIR} ${BINARY}

clean : 
	@${RMDIR} /s /q ${OUT_DIR}

${OUT_DIR} :
	@${ECHO} MD: $(notdir $@)
	@${MKDIR} $@

${BINARY} : ${OBJECTS}
	@${ECHO} LD: $(notdir $@)
	@${CC} ${LDFLAGS} -o $@ $^

${OUT_DIR}/${PROJECT}_%.o : ${SRC_DIR}/%.c
	@${ECHO} CC: $(notdir $<)
	@${CC} ${CFLAGS} ${CPPFLAGS} -c -o $@ $<
