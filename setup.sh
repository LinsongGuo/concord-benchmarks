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

mkdir -p phoenix/input_datasets
pushd phoenix/input_datasets

wget http://csl.stanford.edu/~christos/data/histogram.tar.gz
wget http://csl.stanford.edu/~christos/data/linear_regression.tar.gz
wget http://csl.stanford.edu/~christos/data/word_count.tar.gz
wget http://csl.stanford.edu/~christos/data/reverse_index.tar.gz
wget http://csl.stanford.edu/~christos/data/string_match.tar.gz

# untar datasets into benchmark_name+datafiles
tar -xzf histogram.tar.gz
tar -xzf linear_regression.tar.gz
tar -xzf word_count.tar.gz
tar -xzf reverse_index.tar.gz
tar -xzf string_match.tar.gz
popd

# Commenting out since parsec benchmarks are no longer publicly available
# pushd parsec-benchmark
# 	./setup.sh
# popd

