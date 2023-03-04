# make working directory current.
cd "$(dirname $0)"

# shared-library directory.
export LD_LIBRARY_PATH="$LD_LIBRARY_PATH:$(pwd)/bin/darwin-x64/"

# run
./bin/darwin-x64/Ethertia