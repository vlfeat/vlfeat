# file:        Makefile.dist
# description: Build VLFeat DLL
# author:      Andrea Vedaldi

clean: dist-bin-clean
archclean: dist-bin-archclean
distclean: dist-bin-distclean dist-src-distclean
info: dist-bin-info dist-src-info

# --------------------------------------------------------------------
#                                                        Configuration
# --------------------------------------------------------------------

GIT ?= git
RSYNC ?= rsync

NAME := vlfeat
VER := $(shell cat vl/generic.h | sed -n \
    's/.*VL_VERSION_STRING.*\(\([0-9][0-9]*\.\{0,1\}\)\{3\}\).*/\1/p')
DIST := $(NAME)-$(VER)
BINDIST := $(DIST)-bin
HOST := ganesh.cs.ucla.edu:/var/www/vlfeat.org

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

# dist-bin-release: Rebuild binaries with optimizations and no debug symbols
# dist-bin-commit:  Creates a new vXX.XX.XX-ARCH branch with the binaries
#              and pushes it to the remote called bin
# dist-bin-merge:   Creates a new vXX.XX.XX-bin branch by merging
#              the architecture specific binary branches and
#              adding additional files. It then pushes the result
#              to the remote called bin.
# dist-bin-dist:    Packs the commit remotes/bin/vXX.XX.XX-bin.
# dist-src-dist:    Packs the commit vXX.XX.XX.

.PHONY: dist-bin, dist-bin-release, dist-bin-commit, dist-bin-info
.PHONY: dist-bin-clean, dist-bin-archclean, dist-bin-distclean
no_dep_targets += dist-bin dist-bin-release dist-bin-commit dist-bin-info
no_dep_targets += dist-bin-clean dist-bin-archclean dist-bin-distclean

dist-bin-release:
	set -e ; \
	TMP=tmp-$(NAME)-$(VER)-$(ARCH) ; \
	\
	echo Fetching remote tags ; \
	$(GIT) fetch --tags ; \
	echo Cloning VLFeat ; \
	test -e $$TMP || $(GIT) clone --no-checkout . $$TMP ; \
	$(GIT) --git-dir=$$TMP/.git config remote.bin.url $$($(GIT) config --get remote.bin.url) ; \
	$(GIT) --git-dir=$$TMP/.git config remote.origin.url $$($(GIT) config --get remote.origin.url) ; \
	\
	echo Checking out v$(VER) ; \
	cd $$TMP ; \
	$(GIT) fetch origin ; \
	$(GIT) checkout v$(VER) ; \
	echo Rebuilding binaries for release ; \
	make NDEBUG=yes ARCH=$(ARCH) all

dist-bin-commit: dist-bin-release
	@set -e ; \
	TMP=tmp-$(NAME)-$(VER)-$(ARCH) ; \
	BRANCH=v$(VER)-$(ARCH)  ; \
	\
	cd $$TMP ; \
	echo Setting up $$BRANCH to v$(VER) ; \
	$(GIT) branch -f $$BRANCH v$(VER) ; \
	$(GIT) checkout $$BRANCH ; \
	echo Adding binaries to $$BRANCH ; \
	$(GIT) add -f $(arch_bins) ; \
	if test -z "$$($(GIT) diff --cached)" ; \
	then \
	  echo No changes to commit ; \
	  exit 1 ; \
	fi ; \
	echo Commiting changes ; \
	$(GIT) commit -m "$(ARCH) binaries for version $(VER)" ; \
	echo Pushing $$BRANCH to the server ; \
	$(GIT) push -v --force bin $$BRANCH:refs/heads/$$BRANCH ;

dist-bin-commit-common: dist-bin-commit
	@set -e ; \
	TMP=tmp-$(NAME)-$(VER)-$(ARCH) ; \
	BRANCH=v$(VER)-common  ; \
	\
	cd $$TMP ; \
	echo Setting up $$BRANCH to v$(VER) ; \
	$(GIT) branch -f $$BRANCH v$(VER) ; \
	$(GIT) checkout $$BRANCH ; \
	echo Adding binaries to $$BRANCH ; \
	$(GIT) add -f $(m_lnk) ; \
	if test -z "$$($(GIT) diff --cached)" ; \
	then \
	  echo No changes to commit ; \
	  exit 1 ; \
	fi ; \
	echo Commiting changes ; \
	$(GIT) commit -m "common products for $(VER)" ; \
	echo Pushing $$BRANCH to the server ; \
	$(GIT) push -v --force bin $$BRANCH:refs/heads/$$BRANCH ;

dist-bin-merge:
	set -e ; \
	BRANCH=v$(VER)-bin ; \
	TMP=tmp-$(NAME)-$(VER)-bin ; \
	\
	echo Fetching remote tags ; \
	$(GIT) fetch --tags ; \
	echo Cloning VLFeat ; \
	test -e $$TMP || $(GIT) clone --no-checkout . $$TMP ; \
	$(GIT) --git-dir=$$TMP/.git config remote.bin.url $$($(GIT) config --get remote.bin.url) ; \
	$(GIT) --git-dir=$$TMP/.git config remote.origin.url $$($(GIT) config --get remote.origin.url) ; \
	\
	cd $$TMP ; \
	echo Creating or resetting and checking out branch $$BRANCH to v$(VER); \
	$(GIT) fetch origin ; \
	$(GIT) checkout v$(VER) ; \
	$(GIT) branch -f $$BRANCH v$(VER) ; \
	$(GIT) checkout $$BRANCH ; \
	MERGE_BRANCHES=; \
	FETCH_BRANCHES=; \
	for ALT_ARCH in common maci maci64 glx a64 w32 w64 ; \
	do \
	  MERGE_BRANCH=v$(VER)-$$ALT_ARCH ; \
	  MERGE_BRANCHES="$$MERGE_BRANCHES bin/$$MERGE_BRANCH" ; \
	  FETCH_BRANCHES="$$FETCH_BRANCHES $$MERGE_BRANCH:remotes/bin/$$MERGE_BRANCH" ; \
	done ; \
	echo Fetching binaries ; \
	echo $(GIT) fetch -f bin $$FETCH_BRANCHES ; \
	$(GIT) fetch -f bin $$FETCH_BRANCHES ; \
	echo merging $$MERGE_BRANCHES ; \
	$(GIT) merge -m "Merged binaries $$MERGE_BRANCHES" $$MERGE_BRANCHES ; \
	echo Pushing to server the merged binaries ; \
	$(GIT) push -v --force bin $$BRANCH:refs/heads/$$BRANCH ;


dist-bin:
	echo Fetching binaries ; \
	BRANCH=v$(VER)-bin ; \
	git fetch -f bin $$BRANCH:remotes/bin/$$BRANCH ; \
	echo Creating archive ; \
	COPYFILE_DISABLE=1 \
	COPY_EXTENDED_ATTRIBUTES_DISABLE=1 \
	$(GIT) archive --prefix=$(NAME)-$(VER)/ bin/v$(VER)-bin | gzip > $(BINDIST).tar.gz

dist-bin-clean:

dist-bin-archclean:
	rm -rf tmp-$(NAME)-$(VER)-$(ARCH)

dist-bin-distclean:
	rm -rf tmp-$(NAME)-$(VER)-*
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

.PHONY: post, post-doc

post:
	$(RSYNC)                                                     \
	    -aP $(DIST).tar.gz $(BINDIST).tar.gz                     \
	    $(HOST)/download

post-doc: doc
	$(RSYNC)                                                     \
	      --recursive                                            \
	      --perms                                                \
	      --group=lab                                            \
	      --chmod=Dg+s,g+w,o-w                                   \
	      --exclude=*.eps                                        \
	      --progress                                             \
	      --exclude=download                                     \
	      --exclude=cvpr10wiki                                   \
	      --exclude=man-src                                      \
	      --exclude=toolbox-src                                  \
	      --exclude=.htaccess                                    \
	      --exclude=favicon.ico                                  \
	      --delete                                               \
	      doc/ $(HOST)

# Local variables:
# mode: Makefile
# End:
