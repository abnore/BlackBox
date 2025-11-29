# Manual Installation of BlackBox on macOS

Follow these steps to compile and install your BlackBox C library system-wide.

---

## Step 1 — Compile the source into an object file

```bash
clang -Wall -Wextra -O2 -fPIC -c blackbox.c -o blackbox.o
```
## Step 2 — Build the shared library (.dylib)

```bash
clang -dynamiclib -o libblackbox.dylib blackbox.o
```
## Step 3 — Install the header globally

```bash
sudo cp blackbox.h /usr/local/include/
```

## Step 4 — Install the library globally

```bash
sudo cp libblackbox.dylib /usr/local/lib/
```
You can now include BlackBox in any project using:

```c
#include <blackbox.h>
```

and link when compiling using:

```bash
-lblackbox
```

For example:

```bash
clang main.c -lblackbox -o main
```
