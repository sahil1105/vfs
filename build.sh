


# TODO: Fix whitespace/indentation
# (three-space indent? mixed tabs/spaces? seems legit.)
#	* Done for reduce.c

# TODO: Use structures instead of arrays, so things have names.

# TODO: running './reduce steinberger/U_2822.conf' fails after 3m58.366s with
# 'Abort trap: 6'. According to the internet, this is probably something to do
# with an out-of-bound array index. Joy.

CFLAGS="-Wall -Wextra -Wundef -Wold-style-definition -Wstrict-prototypes -Wpedantic --std=c99"

build() {
	clang $CFLAGS -o reduce src/reduce.c
	clang $CFLAGS -o discharge src/discharge.c
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

check-robertson() {
	# Run everything for Robertson's four color program.

	# Check reducibility of the unavoidable set.
	./reduce robertson/unavoidable.conf

	# Do the stuff with discharging.
	./discharge robertson/present7 robertson/unavoidable.conf robertson/rules 0 1
	./discharge robertson/present8 robertson/unavoidable.conf robertson/rules 0 1
	./discharge robertson/present9 robertson/unavoidable.conf robertson/rules 0 1
	./discharge robertson/present10 robertson/unavoidable.conf robertson/rules 0 1
	./discharge robertson/present11 robertson/unavoidable.conf robertson/rules 0 1
}

check-steinberger() {
	# Run everything for Steinberger's four color program.

	# Check reducibility of the unavoidable set.
	./reduce steinberger/U_2822.conf

	# ./discharge steinberger/p5_2822 steinberger/U_2822.conf steinberger/L_42 0 1
	# ./discharge steinberger/p6_2822 steinberger/U_2822.conf steinberger/L_42 0 1
	# ./discharge steinberger/p7_2822 steinberger/U_2822.conf steinberger/L_42 0 1
	# ./discharge steinberger/p8_2822 steinberger/U_2822.conf steinberger/L_42 0 1
	# ./discharge steinberger/p9_2822 steinberger/U_2822.conf steinberger/L_42 0 1
	# ./discharge steinberger/p10_2822 steinberger/U_2822.conf steinberger/L_42 0 1
	# ./discharge steinberger/p11_2822 steinberger/U_2822.conf steinberger/L_42 0 1
}

check-both() {
	check-robertson
	check-steinberger
}

# TODO: Write a function that builds and runs everything, so that the entire project
# can be run with one command.

if [ $# = 0 ]; then
	build
else
	"$@"
fi
