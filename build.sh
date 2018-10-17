


# TODO: Fix whitespace/indentation
# (three-space indent? mixed tabs/spaces? seems legit.)

# Running time of reduce.c:
# real	4m51.547s
# user  4m24.004s
# sys	0m2.470s

CFLAGS="-Wall -Wextra -Wundef -Wold-style-definition -Wstrict-prototypes -Wpedantic --std=c99"

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
