# CROSS integration

This is a recipe for adding the CROSS signature algorithm to post-quantum libraries ([PQClean][repo_PQClean], [liboqs][repo_liboqs], [oqs-provider][repo_oqs-provider], [oqs-demos][repo_oqs-demos]) starting from the [NIST submission package][CROSS_package].

[CROSS_package]: https://www.cross-crypto.com/nist-submission.html
[repo_PQClean]: https://github.com/PQClean/PQClean/
[repo_liboqs]: https://github.com/open-quantum-safe/liboqs
[repo_oqs-provider]: https://github.com/open-quantum-safe/oqs-provider
[repo_oqs-demos]: https://github.com/open-quantum-safe/oqs-demos

<!-- generate table of contents -->
<!-- https://derlin.github.io/bitdowntoc/ -->

- [Get CROSS ready for PQClean](#get-cross-ready-for-pqclean)
   * [Directory structure](#directory-structure)
   * [SHAKE](#shake)
   * [Detached signatures](#detached-signatures)
   * [Fixed size integers](#fixed-size-integers)
   * [No variable-length arrays](#no-variable-length-arrays)
   * [Remove assertions](#remove-assertions)
   * [Unused parameters](#unused-parameters)
   * [Missing prototypes](#missing-prototypes)
   * [Parameter sets](#parameter-sets)
   * [Placeholders](#placeholders)
   * [Namespacing](#namespacing)
   * [Makefiles](#makefiles)
   * [META.yml](#metayml)
   * [KATs and test vectors](#kats-and-test-vectors)
   * [Parameter file](#parameter-file)
   * [No external includes in `api.h`](#no-external-includes-in-apih)
   * [Astyle](#astyle)
- [PQClean](#pqclean)
   * [Generate parameter sets](#generate-parameter-sets)
   * [Test PQClean](#test-pqclean)
- [liboqs](#liboqs)
   * [Copy from upstream](#copy-from-upstream)
   * [Build liboqs](#build-liboqs)
   * [Test liboqs](#test-liboqs)
- [oqs-provider](#oqs-provider)
   * [Code generation](#code-generation)
   * [Test oqs-provider](#test-oqs-provider)
- [oqs-demos](#oqs-demos)
   * [curl](#curl)
   * [httpd](#httpd)

## Get CROSS ready for PQClean

### Directory structure

* Create the `clean` directory and populate it with all the `.c` and `.h` files from the Reference Implementation, add also the `LICENSE` file.
* Create the `avx2` directory and populate it with all the `.c` and `.h` files from the Optimized Implementation.

### SHAKE

Migrate the internal implementation of SHAKE used by CROSS to the one in `PQClean/common`.
* Delete files:
    * `fips202.c`
    * `fips202.h`
    * `keccakf1600.c`
    * `keccakf1600.h`
* Delete the definition of `randombytes` from `csprng_hash.h`. This function is called during key generation and signing to obtain salts and seeds. To make results like KATs reproducible it is substituted by the one defined in PQClean.
* Delete the declaration of `platform_csprng_state` from `csprng_hash.c` as it was only used by the `randombytes` function.
* Free the memory allocated by SHAKE (the implementation in PQClean uses dynamic memory allocation while the internal one in CROSS uses fixed sizes).
    * In `sha3.h` define function `csprng_release` simply calling `xof_shake_release` which in turn wraps SHAKE's `inc_ctx_release`.
    * For every call to `initialize_csprng` wait for `csprng_randombytes` to be called (possibly more than once) and then add `csprng_release` to free up the memory.

### Detached signatures

PQClean requires two additional functions `crypto_sign_signature` and `crypto_sign_verify` which compute signing and verification using detached signatures (the signature is returned separately from the message instead of appended to it). Declare them in `api.h` and define them in `sign.c` by simply wrapping the existing CROSS functions.

### Fixed size integers

PQClean requires fixed sized integer types.

| Replace | With |
| - | - |
| `unsigned int` | `uint32_t` |
| `unsigned long` | `uint32_t` |
| `unsigned long long` | `uint64_t` |

### No variable-length arrays

PQClean requires the absence of variable length arrays: use a `#define` placed before the function (instead of a normal variable definition) for the size of the array. This happens in function `CROSS_verify` of `CROSS.c` and function `compute_round_seeds` of `seedtree.c`.

### Remove assertions

In `CROSS.c` during signing and verification `assert` is used multiple times to abort and display an error when something went wrong. This is useful for debugging but some tests in PQClean and liboqs require program termination, for example `test_wrong_pk` tries to verify a signature using the wrong public key and checks that `crypto_sign_open` returns `-1`. The assertions must therefore be deleted.

### Unused parameters

PQClean is compiled with flags `-Werror=unused-parameter` and `Werror=unused-value`. The occurrences of unused parameters in CROSS can be eliminated by either changing the function definition and declaration or simply adding a line that uses that parameter, this happens with:
* Parameter `leaves` of function `merkle_tree_proof_compute` in `merkle_tree.h`.
* Parameter `inlen` of function `generic_unpack_fz` in `pack_unpack.c`
* Parameter `inlen` of function `generic_unpack_fq` in file `pack_unpack.c`
* Parameter `val` of function `xof_shake_init` in `sha3.h`

### Missing prototypes

PQClean is compiled with flag `-Werror=missing-prototypes`, add the missing declarations in `merkle_tree.h`.

### Parameter sets
A parameter set is an instance of the algorithm defined by all its possible parameters. CROSS has two variants (RSDP and RSPDG), three security levels (128, 192, 256 bits) and three "targets" (signature size, balanced, speed). In total there are 18 possible parameter sets. Each parameter set contains two implementations: the reference (clean) one and the avx2 optimized one. The script `make_csv.py` uses Python's `itertools` to generate all the possible combinations of parameters, which will be used as placeholders in a parameter set.

### Placeholders
PQClean requires that every parameter set is placed in a separate directory. The script `generate.py` will use CROSS's source files as templates, copying them in an appropriately named directory for each parameter set. After copying it will use text substitution to replace all the necessary parameters in the source files. For example, every time a source file needs to access the length of the public key the placeholder `__length-public-key__` is used instead. The script will then substitute the placeholder with an actual value like 77 while generating the parameter sets.

### Namespacing
PQClean requires "all exported symbols" to be prefixed with a string corresponding to the parameter set and implementation, for example function `crypto_sign` becomes `PQCLEAN_CROSSRSDP128BALANCED_CLEAN_crypto_sign`. Use the placeholder `__namespace__` which will then be substituted when generating the parameter sets. PQClean has a handy test (`test_symbol_namespace.py`) to check that you didn't forget to namespace something.

### Makefiles

For PQClean's compilation two makefiles (make and NMAKE) are necessary in each parameter set's directory. Add CROSS's source files and compilation flags to `Makefile` and `Makefile.Microsoft_nmake`.

### META.yml
Every parameter set in PQClean also needs a file listing its parameters, the hashes for KATs and test vectors, and the implementations (clean and avx2). Placeholders are used again since `META.yml` is also copied as a template by `generate.py`. For liboqs an extra flag is added in `required_flags` for the avx2 implementation.

### KATs and test vectors

`TODO`

### Parameter file
Create a new file `set.h` with placeholders for the parameters in a set, the definitions in here were previously done externally like in `Benchmarking/CMakeLists.txt`. Include `set.h` in `parameters.h` and in the makefiles.

### No external includes in `api.h`
PQClean requires that the api file does not include any external file. Define parameters such as the length of the public key as placeholders, which will be substituted by actual values by `generate.py`.

### Astyle

`TODO`

## PQClean

The goal of [PQClean][repo_PQClean] is to collect implementations of post-quantum key encapsulation mechanisms and signature schemes. The rules for contributing a new scheme are clearly stated in its `README.md` and `CONTRIBUTING.md` files. These rules are also enforced by an extensive test framework built using Python's `pytest` and documented in the Wiki. After applying to CROSS all the changes just discussed the parameter sets can be generated using `generate.py` and then tested in PQClean.

### Generate parameter sets
Run `generate.py` to create a directory for each parameter set. The clean and avx2 implementations will be copied in it along with the META file. Then the placeholders will be substituted by the actual parameters taken from the csv file. The 18 directories can then be copied directly into PQClean's `crypto-sign`.

### Test PQClean

Install all required Python packages from `requirements.txt` then simply move inside the test folder and run the Python scripts. Here are some of the most essential tests:
* `test_nistkat.py` checks the consistency between the KATs produced by PQClean and the ones declared in the `META.yml` files.
* `test_functest.py` checks that the signatures work as expected, e.g. verifying a signed message with the wrong public key returns `-1` meaning failure.
* `test_symbol_namespace.py` checks that the all exported symbols are properly namespaced.

## liboqs

The aim of [liboqs][repo_liboqs] is to collect quantum-safe cryptographic algorithms (same as PQClean) and make them available and easy to use through a common API and benchmarking framework. The coding conventions are similar to PQClean, although less strict.

There are two methods for including a new scheme as documented by the Wiki: either add it directly or use the handy "copy-from-upstream" functionality (chosen here). After inclusion liboqs can be compiled and CROSS benchmarked against other signature schemes.

### Copy from upstream

The idea is to edit a configuration file in liboqs, adding a git repository as an "upstream" from which the source code for the new scheme will be automatically pulled.

* Edit `copy_from_upstream.yml`:
   * Add CROSS's repo under `upstreams` with the last commit SHA.
   * Add a new entry for CROSS under `sigs` specifying every parameter set. The signature should be appended to the message using `signed_msg_order: msg_then_sig`.
* Run `copy_from_upstream.py`, which will use the YAML file to download CROSS into `src/sig`.
* Update liboqs' documentation:
   * Create `docs/algs/sig/cross.yml` to add information about the scheme in general (e.g. the website), and about each specific parameter set (e.g. `length-secret-key`).
   * Run `update_docs_from_yaml.py` and `update_cbom.py` to update the documentation files

### Build liboqs

First install all dependencies listed in liboqs' `README.md`. Then export the variable indicating the source folder and build with:

```
export LIBOQS_DIR=/path/to/liboqs
rm -rf build; mkdir build && cd build; cmake -GNinja ..; ninja
```

### Test liboqs

Move into `liboqs/build/tests` and run the C executables. For example `speed_sig -f` will measure the time and CPU cycles it takes to generate the keypair, sign, and verify. 

Other tests are available in the `liboqs/tests` directory, for example `test_kat.py` will check the consistency of KATs.

## oqs-provider

openssl and liboqs `TODO`

replace liboqs cloning with fork `TODO`

### Code generation

* Edit `generate.yml`
* Run `generate.py`
* Run `generate_oid_nid_table.py`
* Build

### Test oqs-provider

`TODO`

## oqs-demos

`TODO`

### curl

`TODO`

### httpd

`TODO`

