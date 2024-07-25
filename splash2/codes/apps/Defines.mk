CC := clang-11
OPT := opt-11
LLVM_LINK := llvm-link-11

TARGET1 := $(TARGET_NAME)-lc
TARGET2 := $(TARGET_NAME)-orig

all: $(TARGET1)

$(TARGET1): createfiles lc_all.ll
	$(CC) -g $(CFLAGS) -fstandalone-debug $(word 2,$^) $(LOADLIBS) $(LDFLAGS) -o $@ 

$(TARGET2): createfiles opt_simplified.ll
	$(CC) -g $(CFLAGS) -fstandalone-debug $(word 2,$^) $(LOADLIBS) $(LDFLAGS) -o $@ 

ifeq ($(UINTR),1)
lc_all.ll: opt_simplified.ll
	cp $< $@
else
lc_all.ll: opt_simplified.ll
	$(OPT) $(LC_FLAGS) < $< > $@
endif

opt_simplified.ll: llvm_all.ll
	$(OPT) $(OPT_FLAGS) -S < $< > $@

#opt_simplified.ll: opt_all.ll
#	$(OPT) $(OPT_FLAGS) -S < $< > $@
#
#opt_all.ll: llvm_all.ll
#	$(OPT) $(OPT_LEVEL) -S < $< > $@

llvm_all.ll: $(INTERMEDIATE_FILES)
	$(LLVM_LINK) $^ -o $@

$(INTERMEDIATE_FILES): llvm_%.ll : %.c
	$(CC) $(CFLAGS) -S -emit-llvm -o $@ $< 

createfiles: $(HDR_FILES) $(SRC_FILES)

$(HDR_FILES): %.h: %.H
	$(M4) $(MACROS) $< > $@

$(SRC_FILES): %.c: %.C
	$(M4) $(MACROS) $< > $@

clean-$(TARGET1):
	rm -rf *.c *.h *.ll *.o $(TARGET1)

clean-$(TARGET2):
	rm -rf *.c *.h *.ll *.o $(TARGET2)

clean:
	rm -rf *.c *.h *.ll *.o $(TARGET1) $(TARGET2)
