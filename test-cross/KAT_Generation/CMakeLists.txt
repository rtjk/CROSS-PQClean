cmake_minimum_required(VERSION 3.7)

project(CROSS C)
set(CMAKE_C_STANDARD 11)

set(CC gcc)
# set(CC clang)
set(CMAKE_C_FLAGS  "${CMAKE_C_FLAGS} -Wall -pedantic -Wuninitialized -march=native -O3 -g3")
#set(CMAKE_C_FLAGS  "${CMAKE_C_FLAGS} -Wall -pedantic -Wuninitialized -march=native -Wextra -Wmissing-prototypes -Wredundant-decls")

set(CMAKE_C_FLAGS  "${CMAKE_C_FLAGS} ${SANITIZE}")
message("Compilation flags:" ${CMAKE_C_FLAGS})

# default compilation picks reference codebase
set(REFERENCE 1)

#generate by default KATs with standalone keccak library
set(STANDALONE_KECCAK 1)

# selection of specialized compilation units differing between ref and opt
# implementations.
set(REFERENCE_CODE_DIR ../../clean)
set(OPTIMIZED_CODE_DIR ../../Optimized_Implementation)

if(REFERENCE EQUAL 1)
message("Compiling portable reference code")
set(BASE_DIR ${REFERENCE_CODE_DIR})
set(SPEC_HEADERS
        ${BASE_DIR}/restr_arith.h
        ${BASE_DIR}/fq_arith.h )
set(SPEC_SOURCES
        ${BASE_DIR}/CROSS.c
)
else()
message("Compiling optimized AVX2 code")
set(BASE_DIR ${OPTIMIZED_CODE_DIR})
set(SPEC_HEADERS
        ${BASE_DIR}/architecture_detect.h
        ${BASE_DIR}/restr_arith.h
        ${BASE_DIR}/fq_arith.h )
set(SPEC_SOURCES
        ${BASE_DIR}/CROSS.c
)
endif()

set(COMMON_DIR ${REFERENCE_CODE_DIR})
set(HEADERS
    ${SPEC_HEADERS}
    ${COMMON_DIR}/api.h
    ${COMMON_DIR}/CROSS.h
    ${COMMON_DIR}/csprng_hash.h
    ${COMMON_DIR}/pack_unpack.h
    ${COMMON_DIR}/fips202_CROSS.h
    ${COMMON_DIR}/keccakf1600.h
    ${COMMON_DIR}/parameters.h
    ${COMMON_DIR}/seedtree.h
    ${COMMON_DIR}/sha3.h
    ${COMMON_DIR}/merkle_tree.h
    ${COMMON_DIR}/merkle.h
)

if(STANDALONE_KECCAK)
  message("Employing standalone SHA-3")
  set(KECCAK_EXTERNAL_LIB "")
  set(KECCAK_EXTERNAL_ENABLE "")
  list(APPEND FALLBACK_SOURCES ${COMMON_DIR}/keccakf1600.c)
  list(APPEND FALLBACK_SOURCES ${COMMON_DIR}/fips202_CROSS.c)
else()
  message("Employing libkeccak")
  set(KECCAK_EXTERNAL_LIB keccak)
  set(KECCAK_EXTERNAL_ENABLE "-DSHA_3_LIBKECCAK")
endif()


set(SOURCES
    ${SPEC_SOURCES}
    ${FALLBACK_SOURCES}
    ${COMMON_DIR}/csprng_hash.c
    ${COMMON_DIR}/pack_unpack.c
    ${COMMON_DIR}/keccakf1600.c
    ${COMMON_DIR}/fips202_CROSS.c
    ${COMMON_DIR}/seedtree.c
    ${COMMON_DIR}/merkle.c
    ${COMMON_DIR}/sign.c
)

foreach(category RANGE 1 5 2)
    set(RSDP_VARIANTS RSDP RSDPG)
    foreach(RSDP_VARIANT ${RSDP_VARIANTS})
        set(PARAM_TARGETS SIG_SIZE BALANCED SPEED)
        foreach(optimiz_target ${PARAM_TARGETS})
             if(optimiz_target STREQUAL SPEED)
                set(OMIT_SEED_TREE "-DNO_TREES=1")
             else()
                set(OMIT_SEED_TREE "")
             endif()
             # settings for benchmarking binary
             set(TARGET_BINARY_NAME CROSS_KATgen_cat_${category}_${RSDP_VARIANT}_${optimiz_target})
             add_executable(${TARGET_BINARY_NAME} ${HEADERS} ${SOURCES}
                                ./lib/KAT_NIST_rng.c ./lib/PQCgenKAT_sign.c)
             target_include_directories(${TARGET_BINARY_NAME} PRIVATE
                                        ${BASE_DIR}
                                        ${COMMON_DIR}
                                        ./include)
             target_link_libraries(${TARGET_BINARY_NAME} m ssl crypto ${SANITIZE} ${KECCAK_EXTERNAL_LIB})
             set_target_properties(${TARGET_BINARY_NAME} PROPERTIES RUNTIME_OUTPUT_DIRECTORY ./bin)
             set_property(TARGET ${TARGET_BINARY_NAME} APPEND PROPERTY
                 COMPILE_FLAGS "${OMIT_SEED_TREE} -DCATEGORY_${category}=1 -D${optimiz_target}=1 -D${RSDP_VARIANT}=1 ${KECCAK_EXTERNAL_ENABLE} ")        
        endforeach(optimiz_target)
    endforeach(RSDP_VARIANT)
endforeach(category)
