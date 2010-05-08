# file:        bin.mak
# description: Build VLFeat command line utilities
# author:      Andrea Vedaldi

all: bin-all
clean: bin-clean
archclean: bin-archclean
distclean: bin-distclean
info: bin-info

# --------------------------------------------------------------------
#                                                        Configuration
# --------------------------------------------------------------------

BIN_CFLAGS = $(CFLAGS)
BIN_LDFLAGS = $(LDFLAGS) -L$(BINDIR) -lvl

# --------------------------------------------------------------------
#                                                                Build
# --------------------------------------------------------------------

# On Mac OS X the library install_name is prefixed with @loader_path/.
# At run time this causes the loader to search for a local copy of the
# library for any binary which is linked against it. The install_name
# can be modified later by install_name_tool.

bin_src := $(wildcard $(VLDIR)/src/*.c)
bin_tgt := $(addprefix $(BINDIR)/, $(patsubst %.c,%,$(notdir $(bin_src))))
bin_dep := $(addsuffix .d, $(bin_tgt))

deps += $(bin_dep)
arch_bins += $(bin_tgt)
comm_bins +=

.PHONY: bin-all, bin-info
.PHONY: bin-clean, bin-archclean, bin-distclean
no_dep_targets += bin-dir bin-clean bin-archclean bin-distclean
no_dep_targets += bin-info

bin-all: $(bin_tgt)

$(BINDIR)/% : $(VLDIR)/src/%.c $(dll-dir)
	@make -s $(dll_tgt)
	$(call C,CC) $(BIN_CFLAGS) $(BIN_LDFLAGS) "$<" -o "$@"

$(BINDIR)/%.d : $(VLDIR)/src/%.c $(dll-dir)
	$(call C,CC) $(BIN_CFLAGS) -M -MT  \
	       '$(BINDIR)/$* $(BINDIR)/$*.d' \
	       "$<" -MF "$@"

bin-clean:
	rm -f $(bin_dep)

bin-archclean: bin-clean

bin-distclean:

bin-info:
	$(call echo-title,Command line utilities)
	$(call dump-var,bin_src)
	$(call dump-var,bin_tgt)
	$(call dump-var,bin_dep)
	$(call echo-var,BIN_CFLAGS)
	$(call echo-var,BIN_LDFLAGS)
	@echo

# Local variables:
# mode: Makefile
# End:
