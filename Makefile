# image_processing_tools
# See LICENSE file for copyright and license details.

include config.mk

all: options install

# Compile each source file
%.o: %.cpp
	@echo CC -c ${CFLAGS} $<
	@${CC} -c ${CFLAGS} $<


lib${PROJ}.so:${OBJ}
	@echo CC -o $@ ${OBJ} ${LDFLAGS}
	@${CC} -o $@ ${OBJ} ${LDFLAGS}

clean:
	@echo cleaning
	@echo rm -f ${PROJ} ${OBJ} ${PROJ}-${VERSION}.tar.gz
	@rm -f ${PROJ} ${OBJ} ${PROJ}-${VERSION}.tar.gz

dist: clean
	@echo creating dist tarball
	@mkdir -p ${PROJ}-${VERSION}
	@cp -R Makefile config.mk README ${PROJ}-${VERSION}
	@cp src -rf ${PROJ}-${VERSION}
	@tar -cf ${PROJ}-${VERSION}.tar ${PROJ}-${VERSION}
	@gzip ${PROJ}-${VERSION}.tar
	@rm -rf ${PROJ}-${VERSION}

install: lib${PROJ}.so
	@echo installing executable files to ${DESTDIR}${PREFIX}/${PROJ}
	@mkdir -p ${DESTDIR}${PREFIX}/${PROJ}
	@mv -f lib${PROJ}.so ${DESTDIR}${PREFIX}/${PROJ}
	@chmod 755 ${DESTDIR}${PREFIX}/${PROJ}/lib${PROJ}.so
	@echo installing header files to ${DESTDIR}${H_PREFIX}/${PROJ}
	@mkdir -p ${DESTDIR}${H_PREFIX}/${PROJ}/
	@cd src; cp  *.hpp ${DESTDIR}${H_PREFIX}/${PROJ}/

uninstall:
	@echo removing executable file from ${DESTDIR}${PREFIX}/bin
	@rm -f ${DESTDIR}${PREFIX}/bin/${PROJ}

.PHONY: all options clean dist install uninstall
