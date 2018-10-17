


CFLAGS="-Wall -Wextra"

build() {
	clang $CFLAGS -o reduce RSST/anc/reduce.c
	clang $CFLAGS -o discharge RSST/anc/discharge.c
}

clean() {
	rm -f reduce
	rm -f discharge
}

if [ $# = 0 ]; then
	build
else
	"$@"
fi

# vim: set ft=bash:
