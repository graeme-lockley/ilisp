TEST_FLAG=flag

all: $(TEST_FLAG)

$(TEST_FLAG): *.bc
	@for z in `ls -1 | grep  -E "^[a-z0-9-]+$$"`; do \
		echo ./$$z ; \
		./$$z ; \
	done
	echo saweet > $(TEST_FLAG)

.PHONY: all

%.bc: %.mlsp
	../../../src/main ../compile.scm  $(basename $<)

clean:
	rm -f `ls -1 | grep  -E "^[a-z0-9-]+$$"`
	rm -f *.bc
	rm -f *.ll