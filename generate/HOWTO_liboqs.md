# Keep CROSS updated in liboqs

When the codebase of CROSS changes we need to port the modifications to liboqs, this is a step-by-step guide on how to do it.

- Apply the changes to the files in the `clean` and `avx2` directories

- Generate code for the 18 variants:
    ```
    cd generate
    python3 generate.py
    ```

- Commit everything and push to GitHub

- Copy the commit SHA

- All the changes are now part of the "upstream" repository, it's time to deal with liboqs. Move to a fresh directory to follow the next steps

- Clone liboqs and create a new branch, change the branch name to something significant (e.g. `CROSS-fix-endianness-issue`):
    ```
    git clone https://github.com/open-quantum-safe/liboqs
    cd liboqs
    git checkout -b CROSS-fix
    ```

- Create a virtual environment for Python and activate it:
    ```
    python3 -m venv .venv
    source .venv/bin/activate
    pip install -r ./scripts/copy_from_upstream/requirements.txt
    ```

- Replace the commit SHA in these two configuration files with the one copied before:
    - `docs/algorithms/sig/cross.yml`
    - `scripts/copy_from_upstream/copy_from_upstream.yml`

- If necessary update also the version name (`spec-version`) in `cross.yml`

- Update the documentation and import the "upstream" code:
    ```
    export LIBOQS_DIR=/your/path/to/liboqs
    python3 ./scripts/update_docs_from_yaml.py
    cd ./scripts/copy_from_upstream
    rm -rf repos
    python3 copy_from_upstream.py "copy"
    ```

- Build and test liboqs:
    ```
    rm -rf build; mkdir build && cd build; cmake -GNinja ..; ninja; cd ..
    ./build/tests/speed_sig -f
    ```

- Go to your branch on `github.com` and you will be pompted to open a Pull Request to liboqs