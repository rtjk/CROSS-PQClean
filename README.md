# test

This is a recipe for adding the CROSS signature algorithm to post-quantum libraries ([PQClean][repo_PQClean], [liboqs][repo_liboqs], [oqs-provider][repo_oqs-provider]) starting from the [NIST submission package][CROSS_package].

[CROSS_package]: https://www.cross-crypto.com/nist-submission.html
[repo_PQClean]: https://github.com/PQClean/PQClean/
[repo_liboqs]: https://github.com/open-quantum-safe/liboqs
[repo_oqs-provider]: https://github.com/open-quantum-safe/oqs-provider

<!-- generate table of contents -->
<!-- https://derlin.github.io/bitdowntoc/ -->

- [Get CROSS ready for PQClean](#get-cross-ready-for-pqclean)
   * [Directory structure](#directory-structure)
   * [SHAKE](#shake)
   * [Detached signatures](#detached-signatures)
   * [Fixed size integers](#fixed-size-integers)
   * [No variable-length arrays](#no-variable-length-arrays)
   * [Remove assertions](#remove-assertions)
   * [Missing prototypes](#missing-prototypes)
   * [Parameter sets](#parameter-sets)
   * [Placeholders](#placeholders)
   * [Namespacing](#namespacing)
   * [Makefiles](#makefiles)
   * [META.yml](#metayml)
   * [KATs and test vectors](#kats-and-test-vectors)
   * [Parameter file](#parameter-file)
   * [No external includes in `api.h`](#no-external-includes-in-apih)
- [PQClean](#pqclean)
   * [Generate parameter sets](#generate-parameter-sets)
   * [Test PQClean](#test-pqclean)
- [liboqs](#liboqs)
   * [Copy from upstream](#copy-from-upstream)
   * [Test liboqs](#test-liboqs)
- [oqs-provider](#oqs-provider)
   * [Code generation](#code-generation)
   * [Test oqs-provider](#test-oqs-provider)

## Get CROSS ready for PQClean

### Directory structure

* Create the `clean` directory and populate it with all the `.c` and `.h` files from the Reference Implementation, add also the `LICENSE` file.
* Create the `avx2` direcotry and populate it with all the `.c` and `.h` files from the Optimized Implementation.

### SHAKE

Migrate the internal implementation of SHAKE used by CROSS to the one in `PQClean/common`.
* Delete files:
    * `fips202.c`
    * `fips202.h`
    * `keccakf1600.c`
    * `keccakf1600.h`
* Delete the definition of `randombytes` from `csprng_hash.h`. This function is called during key generation and signing to obtain salts and seeds. To make results like KATs reproducible it is substituted by the one defined in PQClean.
* Delete the declaration of `platform_csprng_state` from `csprng_hash.c` as it was only used by the `randombytes` function.
* Free the memory allocated by SHAKE (the implementation in PQClean uses dynamic memory allocation while the internal one in CROSS used fixed sizes).
    * In `sha3.h` define function `csprng_release` simply calling `xof_shake_release` which in turn wraps SHAKE's `inc_ctx_release`.
    * For every call to `initialize_csprng` wait for `csprng_randombytes` to be called (possibly more than once) and then add `csprng_release` to free up the memory.

### Detached signatures

PQClean requires two additional functions `crypto_sign_signature` and `crypto_sign_verify` wich compute signing and verification usign detached signatures (the signature is reurned separately from the message insted of appended to it). Declare them in `api.h` and define them in `sign.c` by simply wrapping the existing CROSS funcitons.

### Fixed size integers

PQClean requires fixed sized integer types.

| Replace | With |
| - | - |
| `unsigned int` | `uint32_t` |
| `unsigned long` | `uint32_t` |
| `unsigned long long` | `uint64_t` |

### No variable-length arrays

`TODO`

### Remove assertions

In `CROSS.c` during signing and verification `assert` is used multiple times to abort and display an error when something went wrong. This is useful for debugging but some tests in PQClean an liboqs require program termination, for example `test_wrong_pk` tries to verify a signature using the wrong public key and checks that `crypto_sign_open` returns `-1`. The assertions must therefore be deleted.

### Missing prototypes

`TODO`

### Parameter sets
A parameter set is an instance of the algorithm defined by all its possible parameters. CROSS has two variants (RSDP and RSPDG), three security levels (128, 192, 256 bits) and three "targets" (signature size, balanced, speed). In total there are 18 possible parameter sets. Each parameter set contains two implementations: the reference (clean) one and the avx2 optimized one. The script `make_csv.py` uses Python's `itertools` to generate all the possible combinations of parameters, wich will be used as placeholders in a parameter set.

### Placeholders
PQClean requires that every parameter set is placed in a separate directory. The script `generate.py` will use CROSS's source files as templates, copying them in an apporpiately named directory for each parameter set. After copying it will use text substitution to replace all the necessary parameters in the source files. For example, every time a source file needs to access the length of the public key the placeholder `__length-public-key__` is used instead. The script will then substitute the placeholder with an actual value like 77 while generating the parameter sets.

### Namespacing
PQClean requires functions and constants to be prefixed with a string corresponding to the parameter set and implementation, for example function `crypto_sign` becomes `PQCLEAN_CROSSRSDP128BALANCED_CLEAN_crypto_sign`. Use the placeholder `__namespace__` wich will then be substituted when generating the parameter sets.

`TODO: namespacing all functions " All exported symbols"?`

### Makefiles

For PQClean's compilation two makefiles (make and NMAKE) are necessary in each parameter set's directory. Add CROSS's source files and compilation flags to `Makefile` and `Makefile.Microsoft_nmake`.

### META.yml
Every parameter set in PQClean also needs a file wich listing its parameters, the hashes for KATs and test vectors, and the implementations (clean and avx2). Placeholders are used again since `META.yml` is also copied as a template by `generate.py`. For liboqs an extra flag is added in `required_flags` for the avx2 implementation.

### KATs and test vectors

`TODO`

### Parameter file
Create a new file `set.h` with placeholders for the parameters in a set, the definitions in here were previously done externally like in `Benchmarking/CMakeLists.txt`. Include `set.h` in `parameters.h` and in the makefiles.

### No external includes in `api.h`
PQClean requires that the api file does not include any external file. Define parameters such as the length of the public key as placeholders, wich will be substituted by actual values.

## PQClean

`TODO`

### Generate parameter sets
Run `generate.py` to create a directory for each parameter set. The clean and avx2 implementations will be copied in it along with the META file. Then the placeholders will be substituted by the actual parameters taken from the csv file. The 18 directories can then be copied inside PQClean's `crypto-sign`.

### Test PQClean

`TODO`

## liboqs

`TODO`

### Copy from upstream

`TODO`

* Edit `copy_from_upstream.yml`
* Run `copy_from_upstream.py`
* Edit `cross.yml`
* Run `update_docs_from_yaml.py`
* Run `update_cbom.py`
* Build

### Test liboqs

`TODO`

## oqs-provider

openssl and liboqs `TODO`

### Code generation

* Edit `generate.yml`
* Run `generate.py`
* Run `generate_oid_nid_table.py`
* Build

### Test oqs-provider

`TODO`





