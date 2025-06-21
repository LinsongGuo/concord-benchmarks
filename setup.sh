#!/bin/bash
source ../../env.sh

sudo apt-get install \
	python3-pip \
 	libtiff-dev \
	build-essential \
	m4 \
	x11proto-xext-dev \
	libglu1-mesa-dev \
	libxi-dev \
	libxmu-dev \
	libtbb-dev \
	libpng-dev \
	libpapi-dev \
	libpfm4-dev \
	libpfm4 \

pip3 install -r requirements.txt