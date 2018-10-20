


# TODO: Fix whitespace/indentation
# (three-space indent? mixed tabs/spaces? seems legit.)

# TODO: Use structures instead of arrays, so things have names.

# Running time of './reduce RSST/anc/unavoidable.conf':
# real	4m51.547s
# user  4m24.004s
# sys	0m2.470s

# TODO: running './reduce RSST/anc/U_2822.conf' fails after 3m58.366s with
# 'Abort trap: 6'. According to the internet, this is probably something to do
# with an out-of-bound array index. Joy.

# TODO: Separate the files from Robertson and Steinberger.
#	present{7,8,9,10,11} are Robertson, as is unavoidable.conf
#	p{5,6,7,8,9,10,11}_2822 are Steinberger, as is U_2822.conf
#	(Steinberger uses a set that only has D-reducible configurations)
#	where do L_42 and rules come from?
#		L_42 is steinberger, I think. (42 rules for discharging)
#		rules: roberson (32 rules for discharging)

CFLAGS="-Wall -Wextra -Wundef -Wold-style-definition -Wstrict-prototypes -Wpedantic --std=c99"

build() {
	clang $CFLAGS -o reduce RSST/anc/reduce.c
	clang $CFLAGS -o discharge RSST/anc/discharge.c
}

debug-build() {
	CFLAGS="-g ${CFLAGS}" build
}

clean() {
	rm -f reduce
	rm -rf reduce.dSYM
	rm -f discharge
	rm -rf discharge.dSYM
}

# TODO: Write a function that builds and runs everything, so that the entire project
# can be run with one command.

if [ $# = 0 ]; then
	build
else
	"$@"
fi
