FROM ubuntu:latest as base

RUN apt-get update && apt-get install -y sudo build-essential pkg-config ninja-build git cmake curl zip unzip tar autoconf  iputils-ping && rm -rf /var/lib/apt/lists/*
WORKDIR /home
RUN git clone https://github.com/Microsoft/vcpkg.git && cd vcpkg && ./bootstrap-vcpkg.sh

FROM base as install

WORKDIR /home

COPY vcpkg.json vcpkg.json
RUN ./vcpkg/vcpkg install 

FROM install as build

COPY . /home/

WORKDIR /home
RUN ln -s /usr/bin/ninja /usr/bin/ninja-build
RUN cmake --preset linux-gcc && \
    cmake --build --preset gcc-debug

FROM ubuntu:latest as runtime

WORKDIR /app
COPY --from=build --chmod=777 /home/build/gcc/ /app
RUN groupadd -r sample && useradd -r -g sample sample
COPY ./front/ /home/front/
USER sample
    
ENTRYPOINT ["./chat-exe"]