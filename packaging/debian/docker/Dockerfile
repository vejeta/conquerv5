FROM debian:trixie

# Install build dependencies
RUN apt-get update && apt-get install -y \
    build-essential \
    devscripts \
    debhelper \
    dh-make \
    fakeroot \
    lintian \
    git \
    libncurses-dev \
    pkg-config \
    sed \
    coreutils \
    gcc \
    libc6-dev \
    make \
    binutils \
    gawk \
    groff \
    bash \
    && rm -rf /var/lib/apt/lists/*

# Set up build environment
RUN useradd -m -s /bin/bash builder

# Copy and set permissions for build script (as root)
COPY build.sh /home/builder/build.sh
RUN chmod +x /home/builder/build.sh && \
    chown builder:builder /home/builder/build.sh

WORKDIR /home/builder
