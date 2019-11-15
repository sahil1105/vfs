

Created 2019-04-01.

# Reproducibility Test 1

This is an attempt to document the process of reproducing the "ring16 crash". I
am attempting to replicate the style of [This thing][repro1]

# Description

The "ring16 crash" occurred when running Steinberger's slightly modified
version of RSST's code on Steinberger's set of 2822 unavoidable D-reducible
configurations. In particular, it occurred on configuration 783, the first
configuration with a ring size of 16. I eventually tracked the crash to
`augment()` and its related functions (with Clang's sanitizers), and found a
fixed-size array that wasn't large enough. I increased the size of the array,
and observed that it did not crash. (In `vfs-exodus`, I found a place where
Steinberger had also increased the size of the array, and added a comment
regarding the size necessary to deal with configurations of ring size `<= 17`.

# Step 1: Obtaining the right files

Early on in the learning process, I started poking around in the code. (In
particular, "fixing" the formatting. Seriously, who uses 3-space indents mixed
with 8-character tabs? And why were RSST so fond of loops with a null statement
for the body?)

Nonetheless, I intend to reproduce this bug with the original code and data.
However, first, I need to check out those files, and put them somewhere.

I put at git tag, `steinberger-sources` on the commit that imported
Steinberger's source files (`0fb6cc2`)

```bash
$ git log -1 steinberger-sources
commit 0fb6cc279682f1f95e87ac330273516d233675a0 (tag: steinberger-sources)
Author: Bertram Ludaescher <ludaesch@illinois.edu>
Date:   Tue Apr 10 04:47:24 2018 -0500

    updated C files from [Steinberger2010]
```

The next challenge is the fact that I need those files from back then, but also
need a build script, and eventually will need to store files generated from
running that build script and the compiled executable.

What if I create a new branch off of `steinberger-sources`, and develop things
there? (Would this manuscript go on that branch? On `master`? On no branch at
all?) The issue still remains, however, regarding what to do with the run
artifacts. Maybe the run artifacts should just stay on that branch.


Create a new branch for this and switch to it:

```bash
$ git branch ring16-repro steinberger-sources
$ git checkout ring16-repro
switched to branch 'ring16-repro'
```

# Step 2: Writing a build script

I need a build script, `build.sh` to:

* Record some information about the enviroment (Compiler version, git revision,
  operating system info
* Extract the offending configuration from `RSST/anc/U_2822.conf`
* Compile the source files (`reduce.c`)
* Run the compiled executable


* (Commit 82c0710: Create build script)

# Step 3: Using the build script

```bash
$ ./build.sh main
```

This should finish rapidly, and create a number of new files:

* `cc.txt`
* `os.txt`
* `commit.txt`
* `ring16.conf`
* `stdout.txt`
* `stderr.txt`

and the executable, `reduce`.

In particular, observe that `stderr.txt` contains the line:

```
./build.sh: line 20: 96192 Abort trap: 6           ./reduce $1
```

signifying that the program has crashed.

Commit the files, with the exception of the executable.

```bash
$ git add cc.txt os.txt commit.txt ring16.conf stdout.txt stderr.txt
$ git commit -m "Record results"
[ring16-repro 175c0b2] Record results
 6 files changed, 58 insertions(+)
 create mode 100644 cc.txt
 create mode 100644 commit.txt
 create mode 100644 os.txt
 create mode 100644 ring16.conf
 create mode 100644 stderr.txt
 create mode 100644 stdout.txt
```


* * *

2019-04-05: I now realize that I will want to record multiple runs, all starting
from commit 467c313. Recording each run as a commit on the previous one is not a
good idea because previous runs alter the state for future runs, so doing a
simple linear sequence is a bad idea.

I propose that commit 467c313 is tagged as `ring16-ready`. Each run will branch
off from there, named `ring16-run1` `ring16-run2` `ring16-run3` (`git branch
ring16-run1 ring16-ready`

```bash
$ git tag 467c313 ring16-ready
$ git branch ring16-run1 ring16-ready
$ git checkout ring16-run1
$ ./build.sh main
$ git add cc.txt os.txt commit.txt ring16.conf stdout.txt stderr.txt
$ git commit -m "Record run"
```

# 2019-04-05 through 2019-04-06

Following discussion with Professor Ludäscher, there are a number of things that
I want to change regarding the build script, so this can be generally considered
a redo.

* Each "run" is now to be contained within its own subdirectory.
  The build script is now invoked as `./build.sh <directory_name>`.
  The directory will be created, filled with run artifacts (with the exception
  of the compiled executables), and committed.
* For example, I would run `./build.sh RN_macbook-1` for my first run, but for
  the third run on Professor Ludäscher's GEME computer, the command
  `./build.sh GEME-3` might be used instead.
* The build script now does both reduction and discharging.
* The build script now record the commands used to compile the programs, by
  generating a build script in the subdirectory and executing that.

So really, there's three different things I want to test:

* reduce ring16: Configuration 783 of U_2822.conf is the first one of ring size 16,
  which causes problems with the insufficiently-large arrays present in reduce.c
* reducing U_2822.conf: Test that all the configurations in U_2822.conf are
  reducible.
* discharging U_2822.conf: Run the discharging procedure on U_2822.conf.
  This involves running the discharging command 8 times: Once for each hub
  degree from 5 to 11.

It may also be desirable to do the same tests for RSST:

* reducing unavoidable.conf
* discharging unavoidable.conf: This uses the present{5..11} proof scripts

I think each "testing flavor" is deserving of its own branch off of a basic
"experimental base" tag. (Because each branch will have its own sequence of
test-record commits added to it) Each branch will involve some minor
configuration of the build script: namely, the `run_reduce` and `run_discharge`
functions.

# 2019-04-09 through 2019-04-10

Various fiddling around with build scripts and rebasing things so that the git
history is not a giant mess. I think its ready now.

The workflow is as follows:

* To create a new test, fork a new branch off of the tag `experiment-base`:
  
  ```bash
  $ git branch experiment/<descriptive stuff here> experiment-base
  $ git checkout experiment/<descriptive stuff here>
  ```

  Then, it is necessary to do minor customisation of the build script: Namely,
  changing the invocations of `reduce` and `discharge` or removing unnecessary
  ones. Commit this with a message like "Setup for testing"
* Now, to run a test, you need to give a name for the run. I propose using names
  like `RN-macbook-1` for the first run on Robert Nagel's Macbook. Professor
  Ludäscher, however may use a run like `BL-GEME-3` for the 3rd run on GEME.

  Then, to actually perform the run, execute

  ```bash
  $ ./build.sh <run name>
  ```

  This will automatically record OS/hardware, compiler version, compiler output,
  git revision, run output, etc. and store the results in a subdirectory with
  the same name as the run. (The outputs of the run `RN-macbook-1` will be
  found in `RN-macbook-1/`) The build script will then commit the output
  subdirectory with a bland-but-descriptive commit message.

  Note that the produced binaries are not committed because:

  * We have no use for them (Out project does not care about the bits and bytes
    of the compiled programs)
  * They take up useful space (GitHub has an 80MB repository size limit? or
    something... Point is, we should not stuff large numbers of binaries on it.)


[repro1]: https://github.com/tmcphillips/openrefine-provenance/blob/master/notes/2019-03-04%20install%20openrefine%20in%20reproducible%20computing%20environment.md


<!-- vim: set tw=80: -->
