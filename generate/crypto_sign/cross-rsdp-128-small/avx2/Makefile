# This Makefile can be used with GNU Make or BSD Make

LIB = libcross-rsdp-128-small_avx2.a

HEADERS = api.h CROSS.h csprng_hash.h fq_arith.h keccakf1600.h merkle.h merkle_tree.h pack_unpack.h parameters.h restr_arith.h seedtree.h sha3.h fips202_CROSS.h set.h architecture_detect.h
OBJECTS = CROSS.o csprng_hash.o keccakf1600.o merkle.o pack_unpack.o seedtree.o sign.o fips202_CROSS.o

# TODO: avx2 flags
CFLAGS=-std=c99 -I../../../common $(EXTRAFLAGS) -march=native -O3 -g3 -std=c99 -Wpedantic -Werror -Wredundant-decls -Wmissing-prototypes -Wuninitialized -Wall -Wextra


all: $(LIB)

%.o: %.c $(HEADERS)
	$(CC) $(CFLAGS) -c -o $@ $<

$(LIB): $(OBJECTS)
	$(AR) -r $@ $(OBJECTS)

clean:
	$(RM) $(OBJECTS)
	$(RM) $(LIB)