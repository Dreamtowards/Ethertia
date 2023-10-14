# OpenXR-Hpp project

This repository contains build scripts and test files for the `openxr*.hpp`
headers, providing a C++-friendly projection of the OpenXR API.

The authoritative public repository is located at
<https://github.com/KhronosGroup/OpenXR-HPP/>. It hosts the public Issue
tracker, and accepts patches (Pull Requests) from the general public.

If you want to simply write an application using OpenXR (the headers and
loader), with minimum dependencies, see
<https://github.com/KhronosGroup/OpenXR-SDK/>. That project will likely contain
the `openxr*.hpp` artifact when it is ready for widespread production usage.

To build this project, you must have `OpenXR-SDK-Source` cloned in a peer
directory of this one.

## Directory Structure

- `README.md` - This file
- `COPYING.md` - Copyright and licensing information
- `CODE_OF_CONDUCT.md` - Code of Conduct
- `OPENXR-HPP.md` - Some basic introductory documentation
- `include/` - Build system to generate the `openxr*.hpp` files
- `scripts/` - Python source code and Jinja2 templates for generating the
  headers.
- `tests/` - some simple files that ensures the header is always compilable.

## Building

If you just want to generate the headers, run `./generate-openxr-hpp.sh` or
`./generate-openxr-hpp.ps1`. If your OpenXR-SDK-Source (or internal gitlab) repo
isn't in a directory named that parallel to this one, you can set `OPENXR_REPO`
environment variable before running. Requires clang-format, preferably 6.0.

If you'd like to build the tests (making sure the headers can compile),
use CMake to generate a build system, like:

```sh
mkdir build
cd build
cmake ..
make
```

## Development

To improve/maintain consistent code style and code quality, we strongly
recommend setting up the pre-commit hooks, which check/correct:

- large file additions
- byte-order marker
- case conflicts
- unresolved merge conflicts
- broken symlinks
- file endings
- line endings
- trailing whitespace
- autopep8
- [cmake-format][]

Using these hooks involves the following steps:

**Install** [pre-commit][] - available thru pip or your
preferred package manager.

```sh
python3 -m pip install --user pre-commit
```

**Setup** the git hook scripts by running this script. This will configure the
current git repo working directory to run the hooks, as well as cloning and
building (if required) the various tools used by the hooks.

```sh
pre-commit install
```

Optionally, you can **run** the hooks over all files manually, before a commit:

```sh
pre-commit run --all-files
```

[cmake-format]: https://cmake-format.readthedocs.io
[pre-commit]: https://pre-commit.com/
