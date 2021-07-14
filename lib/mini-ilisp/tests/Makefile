TEST_FLAG=flag.txt

FILES=$(shell find . -name "*.mlsp")
BINARY_FILES=$(shell find . -name "*.mlsp" | sed 's/.mlsp/.bin/g')

all: $(TEST_FLAG)

.PHONY: all

$(TEST_FLAG): $(BINARY_FILES)
	@for z in $(BINARY_FILES); do \
		echo ./$$z ; \
		./$$z ; \
		if [ $$? != 0 ] ; then exit 1 ; fi \
	done
	echo saweet > $(TEST_FLAG)

%.bin: %.mlsp
	../../../src/main ../compile.scm  $(basename $<)

clean:
	rm -f $(BINARY_FILES)
	rm -f `find . -name "*.bc"`
	rm -f `find . -name "*.ll"`
	rm -f $(TEST_FLAG)