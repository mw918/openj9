# Copyright (c) 2017, 2020 IBM Corp. and others
#
# This program and the accompanying materials are made available under
# the terms of the Eclipse Public License 2.0 which accompanies this
# distribution and is available at https://www.eclipse.org/legal/epl-2.0/
# or the Apache License, Version 2.0 which accompanies this distribution and
# is available at https://www.apache.org/licenses/LICENSE-2.0.
#
# This Source Code may also be made available under the following
# Secondary Licenses when the conditions for such availability set
# forth in the Eclipse Public License, v. 2.0 are satisfied: GNU
# General Public License, version 2 with the GNU Classpath
# Exception [1] and GNU General Public License, version 2 with the
# OpenJDK Assembly Exception [2].
#
# [1] https://www.gnu.org/software/classpath/license.html
# [2] http://openjdk.java.net/legal/assembly-exception.html
#
# SPDX-License-Identifier: EPL-2.0 OR Apache-2.0 OR GPL-2.0 WITH Classpath-exception-2.0 OR LicenseRef-GPL-2.0 WITH Assembly-exception

# To use this docker file:
#   docker build -t=openj9 .
#   docker run -it openj9

FROM ubuntu:16.04

# This section installs the required OS tools.
RUN apt-get update \
  && apt-get install -qq -y --no-install-recommends \
    autoconf \
    ca-certificates \
    ccache \
    cmake \
    cpio \
    file \
    git \
    git-core \
    libasound2-dev \
    libcups2-dev \
    libdwarf-dev \
    libelf-dev \
    libfontconfig1-dev \
    libfreetype6-dev \
    libmpc3 \
    libx11-dev \
    libxext-dev \
    libxrender-dev \
    libxt-dev \
    libxtst-dev \
    make \
    pkg-config \
    realpath \
    ssh \
    unzip \
    wget \
    zip \
  && rm -rf /var/lib/apt/lists/*

# Install optional tools
RUN apt-get update \
  && apt-get install -qq -y --no-install-recommends \
    vim \
  && rm -rf /var/lib/apt/lists/*

# Install GCC-7.4
RUN cd /usr/local \
  && wget -O gcc-7.tar.xz "https://ci.adoptopenjdk.net/userContent/gcc/gcc740+ccache.s390x.tar.xz" \
  && tar -xJf gcc-7.tar.xz --strip-components=1 \
  && rm -rf gcc-7.tar.xz

# Create links for GCC to access the C library and gcc,g++
RUN ln -s /usr/lib/s390x-linux-gnu /usr/lib64 \
  && ln -s /usr/include/s390x-linux-gnu/* /usr/local/include \
  && ln -s /usr/local/bin/g++-7.4 /usr/bin/g++ \
  && ln -s /usr/local/bin/gcc-7.4 /usr/bin/gcc

# Download and setup freemarker.jar to /root/freemarker.jar
RUN cd /root \
  && wget https://sourceforge.net/projects/freemarker/files/freemarker/2.3.8/freemarker-2.3.8.tar.gz/download -O freemarker.tgz \
  && tar -xzf freemarker.tgz freemarker-2.3.8/lib/freemarker.jar --strip=2 \
  && rm -f freemarker.tgz

# Download and install boot JDK from AdoptOpenJDK
RUN cd /root \
  && wget -O bootjdk8.tar.gz https://api.adoptopenjdk.net/openjdk8-openj9/releases/s390x_linux/latest/binary \
  && tar -xzf bootjdk8.tar.gz \
  && rm -f bootjdk8.tar.gz \
  && ls | grep -i jdk | xargs -I % sh -c 'mv % bootjdk8'

# Set environment variable JAVA_HOME, and prepend ${JAVA_HOME}/bin to PATH
ENV JAVA_HOME="/root/bootjdk8"
ENV PATH="${JAVA_HOME}/bin:${PATH}"

WORKDIR /root
