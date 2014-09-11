# file: dist.mak
# description: Build VLFeat DLL
# author: Andrea Vedaldi

# Copyright (C) 2007-12 Andrea Vedaldi and Brian Fulkerson.
# All rights reserved.
#
# This file is part of the VLFeat library and is made available under
# the terms of the BSD license (see the COPYING file).

clean: dist-bin-clean
archclean: dist-bin-archclean
distclean: dist-bin-distclean dist-src-distclean
info: dist-bin-info dist-src-info

# --------------------------------------------------------------------
#                                                        Configuration
# --------------------------------------------------------------------

GIT ?= git
RSYNC ?= rsync
VER ?= $(shell cat vl/generic.h | sed -n \
    's/.*VL_VERSION_STRING.*\"\([0-9.]*\)\".*/\1/p')
TMPDIR ?= /tmp

NAME := vlfeat
DIST := $(NAME)-$(VER)
BINDIST := $(DIST)-bin
HOST := vlfeat-admin:vlfeat.org/sandbox


# --------------------------------------------------------------------
#                                                 Build source package
# --------------------------------------------------------------------

.PHONY: dist-src, dist-src-clean, dist-src-info
no_dep_targets += dist-src, dist-src-clean, dist-src-info

dist-src:
	COPYFILE_DISABLE=1 \
	COPY_EXTENDED_ATTRIBUTES_DISABLE=1 \
	$(GIT) archive --prefix=$(NAME)-$(VER)/ v$(VER) | gzip > $(DIST).tar.gz

dist-src-distclean:
	rm -f $(DIST).tar.gz

dist-src-info:
	$(call echo-title,Source distribution)
	$(call echo-var,DIST)
	@echo

# --------------------------------------------------------------------
#                                                 Build binary package
# --------------------------------------------------------------------

# dist-bin-release: Rebuild binaries with optimizations and no debug
#   symbols.
#
# dist-bin-commit: Creates a new vXX.XX.XX-ARCH branch with the
#   binaries and pushes it to the remote called bin.
#
# dist-bin-commit-common: Creates a new vXX.XX.XX-common branch with
#   the architecture-independent parts of VLFeat.
#
# dist-bin-merge: Creates a new vXX.XX.XX-bin branch by merging the
#   architecture specific binary branches and adding additional
#   files. It then pushes the result to the remote called bin.
#
# dist-bin: Packs the commit remotes/bin/vXX.XX.XX-bin.
#
# dist-src: Packs the commit vXX.XX.XX.

.PHONY: dist-bin, dist-bin-release, dist-bin-commit, dist-bin-info
.PHONY: dist-bin-clean, dist-bin-archclean, dist-bin-distclean
no_dep_targets += dist-bin dist-bin-release dist-bin-commit dist-bin-info
no_dep_targets += dist-bin-clean dist-bin-archclean dist-bin-distclean

dist-bin-release: tmp-dir=$(TMPDIR)/tmp-$(NAME)-$(VER)-$(ARCH)
dist-bin-release:
	@echo Cloning VLFeat ;
	test -e "$(tmp-dir)" || $(GIT) clone --no-checkout . "$(tmp-dir)" ; \
	$(GIT) --git-dir="$(tmp-dir)/.git" config remote.bin.url $$($(GIT) config --get remote.bin.url) ; \
	$(GIT) --git-dir="$(tmp-dir)/.git" config remote.origin.url $$($(GIT) config --get remote.origin.url) ;
	@echo Checking out v$(VER) ;
	cd "$(tmp-dir)" ; $(GIT) fetch origin --tags v$(VER) ;
	cd "$(tmp-dir)" ; $(GIT) fetch origin v$(VER) ;
	cd "$(tmp-dir)" ; $(GIT) checkout v$(VER) ;
	echo Rebuilding binaries for release ;
	make -C "$(tmp-dir)" ARCH=$(ARCH) all

dist-bin-commit: tmp-dir=$(TMPDIR)/tmp-$(NAME)-$(VER)-$(ARCH)
dist-bin-commit: branch=v$(VER)-$(ARCH)
dist-bin-commit: dist-bin-release
	@echo Setting $(branch) to v$(VER) ;
	cd "$(tmp-dir)" ; $(GIT) branch -f $(branch) v$(VER) ;
	cd "$(tmp-dir)" ; $(GIT) checkout $(branch) ;
	@echo Adding binaries to $(branch) ;
	cd "$(tmp-dir)" ; $(GIT) add -f $(arch_bins) ;
	cd "$(tmp-dir)" ; \
	if test -z "$$($(GIT) diff --cached)" ; \
	then \
	  echo No changes to commit ; \
	  exit 1 ; \
	else  \
	  echo Commiting changes ; \
	  $(GIT) commit -m "$(ARCH) binaries for version $(VER)" ; \
	  echo Pushing $(branch) to the server ; \
	  $(GIT) push -v --force bin $(branch):refs/heads/$(branch) ; \
	fi

dist-bin-commit-common: tmp-dir=$(TMPDIR)/tmp-$(NAME)-$(VER)-$(ARCH)
dist-bin-commit-common: branch=v$(VER)-common
dist-bin-commit-common: dist-bin-release
	@echo Building doc
	make -C "$(tmp-dir)" ARCH=$(ARCH) doc-deep
	@echo Setting up $(branch) to v$(VER) ;
	cd "$(tmp-dir)" ; $(GIT) branch -f $(branch) v$(VER)
	cd "$(tmp-dir)" ; $(GIT) checkout $(branch)
	@echo Adding products to $(branch)
	cd "$(tmp-dir)" ; $(GIT) add -f $(m_lnk)
	cd "$(tmp-dir)" ; $(GIT) add -f $$(find doc \
	-name '*.html' -or -name '*.jpg' -or -name '*.png' -or -name '*.css')
	cd "$(tmp-dir)" ; \
	if test -z "$$($(GIT) diff --cached)" ; \
	then \
	  echo No changes to commit ; \
	  exit 1 ; \
	else \
	  echo Commiting changes ; \
	  $(GIT) commit -m "common products for $(VER)" ; \
	  echo Pushing $(branch) to the server ; \
	  $(GIT) push -v --force bin $(branch):refs/heads/$(branch); \
	fi

dist-bin-merge: tmp-dir=$(TMPDIR)/tmp-$(NAME)-$(VER)-merge
dist-bin-merge: branch=v$(VER)-bin
dist-bin-merge:
	@echo Cleaning up merge directory
	rm -rf "$(tmp-dir)"
	@echo Cloning VLFeat
	$(GIT) clone --no-checkout . "$(tmp-dir)" ;
	$(GIT) --git-dir=$(tmp-dir)/.git config remote.bin.url $$($(GIT) config --get remote.bin.url) ;
	$(GIT) --git-dir=$(tmp-dir)/.git config remote.origin.url $$($(GIT) config --get remote.origin.url) ;
	echo Creating or resetting and checking out branch $(branch) to v$(VER);
	cd "$(tmp-dir)" ; $(GIT) fetch origin --tags v$(VER);
	cd "$(tmp-dir)" ; $(GIT) fetch origin v$(VER) ;
	cd "$(tmp-dir)" ; $(GIT) checkout v$(VER) ;
	cd "$(tmp-dir)" ; $(GIT) branch -f $(branch) v$(VER) ;
	cd "$(tmp-dir)" ; $(GIT) checkout $(branch) ;
	cd "$(tmp-dir)" ; \
	MERGE_BRANCHES=; \
	FETCH_BRANCHES=; \
	for ALT_ARCH in common maci maci64 glnx86 glnxa64 win32 win64 ; \
	do \
	  MERGE_BRANCH=v$(VER)-$$ALT_ARCH ; \
	  MERGE_BRANCHES="$$MERGE_BRANCHES bin/$$MERGE_BRANCH" ; \
	  FETCH_BRANCHES="$$FETCH_BRANCHES $$MERGE_BRANCH:remotes/bin/$$MERGE_BRANCH" ; \
	done ; \
	echo Fetching binaries ; \
	echo $(GIT) fetch -f bin $$FETCH_BRANCHES ; \
	$(GIT) fetch -f bin $$FETCH_BRANCHES ; \
	echo merging $$MERGE_BRANCHES ; \
	$(GIT) merge -m "Merged binaries $$MERGE_BRANCHES" $$MERGE_BRANCHES ;
	echo Pushing to server the merged binaries ; \
	cd "$(tmp-dir)" ; $(GIT) push -v --force bin $(branch):refs/heads/$(branch) ;

dist-bin: branch=v$(VER)-bin
dist-bin:
	echo Fetching binaries ; \
	$(GIT) fetch -f bin $(branch):remotes/bin/$(branch) ; \
	echo Creating archive ; \
	COPYFILE_DISABLE=1 \
	COPY_EXTENDED_ATTRIBUTES_DISABLE=1 \
	$(GIT) archive --prefix=$(NAME)-$(VER)/ bin/v$(VER)-bin | gzip > $(BINDIST).tar.gz

dist-bin-clean:

dist-bin-archclean:
	rm -rf $(TMPDIR)/tmp-$(NAME)-$(VER)-$(ARCH)

dist-bin-distclean:
	rm -rf $(TMPDIR)/tmp-$(NAME)-$(VER)-*
	rm -f $(BINDIST).tar.gz

dist-bin-info:
	$(call echo-title,Binary distribution)
	$(call dump-var,arch_bins)
	$(call dump-var,comm_bins)
	$(call echo-var,NAME)
	$(call echo-var,BINDIST)
	@echo

# --------------------------------------------------------------------
#                                             Post packages on the web
# --------------------------------------------------------------------

.PHONY: post, post-doc, post-doc-from-dist

post:
	$(RSYNC)                                                     \
	    -aP $(DIST).tar.gz $(BINDIST).tar.gz                     \
	    $(HOST)/download

rsync-doc = \
	$(RSYNC)                                                     \
	      --recursive                                            \
	      --perms                                                \
	      --group=lab                                            \
	      --chmod=Dg+s,g+w,o-w                                   \
	      --exclude=build                                        \
	      --exclude=download                                     \
	      --delete                                               \
	      --progress                                             \

post-doc:
	$(rsync-doc) doc/ $(HOST)

post-doc-from-dist: dist-bin
	tar xzvf $(BINDIST).tar.gz -C $(TMPDIR)/ $(NAME)-$(VER)/doc/
	$(rsync-doc) $(TMPDIR)/$(NAME)-$(VER)/doc/ $(HOST)

# Local variables:
# mode: Makefile
# End:
