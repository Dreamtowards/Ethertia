FROM debian:buster

RUN apt update && apt install -y build-essential crossbuild-essential-amd64 crossbuild-essential-armhf mingw-w64 \
    python3 python3-pip python3-setuptools python3-wheel ninja-build \
    wget cmake xxd valgrind gcovr lcov git cppcheck gdb \
    && pip3 install meson

COPY meson /usr/local/share/meson
COPY .bashrc /root/.bashrc
