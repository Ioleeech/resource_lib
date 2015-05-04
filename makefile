# Variables
PROJECT  := res_edit

INC_DIR  := .
SRC_DIR  := .
OUT_DIR  := .\_output

HEADERS  := $(wildcard ${INC_DIR}/*.h)
SOURCES  := $(wildcard ${SRC_DIR}/*.c)
OBJECTS  := $(patsubst ${SRC_DIR}/%.c,${OUT_DIR}/%.o,${SOURCES})
BINARY   := ${OUT_DIR}/${PROJECT}.exe

CPPFLAGS += -I${INC_DIR}
CFLAGS   += -g -Wall
#CFLAGS  += -mno-ms-bitfields
LDFLAGS  += -g
#LDFLAGS += -s

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
	@${ECHO} MD: $@
	@${MKDIR} $@

${BINARY} : ${OBJECTS}
	@${ECHO} LD: $@
	@${CC} ${LDFLAGS} -o $@ $^

${OUT_DIR}/%.o : ${SRC_DIR}/%.c
	@${ECHO} CC: $<
	@${CC} ${CFLAGS} ${CPPFLAGS} -c -o $@ $<
