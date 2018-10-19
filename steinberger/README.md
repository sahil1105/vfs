


This subdirectory contains things related to Steinbergers 4CT program.

files:
	* `U_2822.conf`: The unavoidable set of 2822 D-reducible configurations, in
	  machine-readable format.
	* `U_2822.pdf`: The unavoidable set of 2822 D-reducible configurations, in
	  graphical format.
	* `L_42`: The discharging rules used by discharge.c
	* `p{5,6,7,8,9,10,11}_2822`: "proof scripts" for the discharging rules.

Usage:

This assumes you have already compiled `src/reduce.c` and `src/discharge.c`,
and the resulting executables are `reduce` and `discharge`.

To check reducibility of the unavoidable set:

```bash
$ reduce U_2822.conf
```

To do ????? (what does discharge.c actually *do*?)

```bash
$ discharge p7_2822 U_2822.conf L_42 0 1
```
