#------------------------------------------------------------------------------
# Copyright (c) 2007-2009, Stanford University
# All rights reserved.
#
# Redistribution and use in source and binary forms, with or without
# modification, are permitted provided that the following conditions are met:
#     * Redistributions of source code must retain the above copyright
#       notice, this list of conditions and the following disclaimer.
#     * Redistributions in binary form must reproduce the above copyright
#       notice, this list of conditions and the following disclaimer in the
#       documentation and/or other materials provided with the distribution.
#     * Neither the name of Stanford University nor the names of its 
#       contributors may be used to endorse or promote products derived from 
#       this software without specific prior written permission.
#
# THIS SOFTWARE IS PROVIDED BY STANFORD UNIVERSITY ``AS IS'' AND ANY
# EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
# WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
# DISCLAIMED. IN NO EVENT SHALL STANFORD UNIVERSITY BE LIABLE FOR ANY
# DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
# (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
# LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND
# ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
# (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
# SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
#------------------------------------------------------------------------------ 

# This Makefile requires GNU make.
CONCORD_HOME := $(shell pwd)/../../../../../../../..
CXXFLAGS := -Wl,-rpath=$(CONCORD_HOME)/lib
CXXFLAGS += -I$(CONCORD_HOME)/src -I$(CI_HOME)
LDFLAGS := -lm -L$(CONCORD_HOME)/lib

MODIFIED_SUBLOOP_COUNT ?= 200
DISABLE_BOUNDED_LOOP ?= 1

# ==== CONCORD Start ====
ifeq ($(ORIG), 1)
CONCORD_LIB = ${CONCORD_LIB_PATH}/concord_base.o -Wl,--wrap=pthread_create
else ifeq ($(ACCURACY_TEST), 0) 
CONCORD_LIB = ${CONCORD_LIB_PATH}/concord_pthread.o -Wl,--wrap=pthread_create
else
CONCORD_LIB = ${CONCORD_LIB_PATH}/concord_accuracy.a -Wl,--wrap=pthread_create
endif

ifeq ($(CONCORD_PASS_TYPE),rdtsc)
$(info Building with rdtsc pass)
CONCORD_PASS = ${CONCORD_SRC_PATH}/rdtsc-pass/build/src/libConcordPass.so
else
$(info Building with cache line pass)
CONCORD_PASS = ${CONCORD_SRC_PATH}/cache-line-pass/build/src/libConcordPass.so
endif

CONCORD_PASS_OPT = -S -load $(CONCORD_PASS) -yield -modified_subloops $(MODIFIED_SUBLOOP_COUNT) -disable_bounded_loops $(DISABLE_BOUNDED_LOOP) 
# ==== CONCORD End ====



# Query the shell to compile the code correctly for different architectures.
OSTYPE = $(shell uname)
LLVM_BUILD = /usr

ifeq ($(OSTYPE),CYGWIN_NT-5.1)
OS = -D_CYGWIN_
endif

ifeq ($(OSTYPE),Linux)
OS = -D_LINUX_
CC = clang-11
#DEBUG = -g
CFLAGS = -Wall $(OS) $(DEBUG) -O3 -D_GNU_SOURCE
LIBS = -pthread -lpapi
endif

ifeq ($(OSTYPE),SunOS)
OS =  -D_SOLARIS_
#CC = cc
CC = clang-11 
#DEBUG = -g
CFLAGS = -Wall $(OS) $(DEBUG) -O3 -D_FILE_OFFSET_BITS=64
LIBS = -lm -lrt -lthread -lmtmalloc -llgrp
endif

ifeq ($(OSTYPE),Darwin)
OS = -D_DARWIN_
endif

ARCHTYPE = $(shell uname -p)

ifeq ($(ARCHTYPE),sparc)
ARCH = -DCPU_V9
endif

ifeq ($(shell uname -m),x86_64)
ARCH = -D__x86_64__
endif

CC := clang-11
OPT := opt-11
LLVM_LINK := llvm-link-11
#CC := /mnt/nilanjana/bin/clang
#OPT := /mnt/nilanjana/bin/opt
#LLVM_LINK := /mnt/nilanjana/bin/llvm-link

CFLAGS += -Wall $(ARCH) -O3 -D_GNU_SOURCE 

# The $(OS) flag is included here to define the OS-specific constant so that
# only the appropriate portions of the application get compiled. See the README
# file for more information.
AR = ar
RANLIB = ranlib
LDFLAGS =

PHOENIX = phoenix
LIB_PHOENIX = lib$(PHOENIX)

LINKAGE = static
ifeq ($(LINKAGE),static)
TARGET = $(LIB_PHOENIX).a
LIB_DEP = $(HOME)/$(LIB_DIR)/$(TARGET)
endif

ifeq ($(LINKAGE),dynamic)
TARGET = $(LIB_PHOENIX).so
LIB_DEP =
endif

SRC_DIR = src
LIB_DIR = lib
INC_DIR = include
TESTS_DIR = tests
CI_LIB_HOME = $(shell pwd)/../../../../../#w.r.t src directory
CI_HOME = $(shell pwd)/../../../#w.r.t src directory

# these variables may be exported from outside

EXTRA_FLAGS ?=
CFLAGS += $(EXTRA_FLAGS)
#CFLAGS += -g $(EXTRA_FLAGS) -I$(CI_HOME) -I$(CI_LIB_HOME)
CFLAGS += -I$(CI_LIB_HOME)/src -I$(CI_HOME) -I$(CI_LIB_HOME)/../src -I$(CI_HOME)/../

LIBS +=  -Wl,-rpath=./$(LIB_DIR)/ -L$(CI_LIB_HOME)/../lib/ -Wl,-rpath=$(CI_LIB_HOME)/../lib/ $(EXTRA_LD_FLAGS) $(CONCORD_LIB)
#LDFLAGS := -lm -static -lpapi
#disable-verify
LC_FLAGS = $(CONCORD_PASS_OPT) 

SRC_LC_FLAGS = $(CONCORD_PASS_OPT) 


UNROLL_COUNT ?= 0

ifeq ($(ORIG), 1)
OPT_FLAGS = -postdomtree -mem2reg -indvars -loop-simplify -branch-prob -scalar-evolution
else ifeq ($(UINTR), 1)
OPT_FLAGS = -postdomtree -mem2reg -indvars -loop-simplify -branch-prob -scalar-evolution
else ifeq ($(UNROLL_COUNT), 0)
OPT_FLAGS = -postdomtree -mem2reg -indvars -loop-simplify -branch-prob -scalar-evolution
else
OPT_FLAGS = -postdomtree -mem2reg -indvars -loop-simplify -branch-prob -scalar-evolution -loop-unroll -unroll-allow-partial -unroll-count=$(UNROLL_COUNT)
endif