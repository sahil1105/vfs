# vfs

Studien zum Vierfarbensatz

# Fall 2018 - Spring 2019

This is a project for NCSA SPIN 2018. It was continued through May 2019.

This file intends to give an overview of how this repository works, and some of
the machinery used to make it work.

(TODO: Explain what this project is and why such a project is relevant.
The Four-Color Theorem: Historic, controversial. Computational reproducibility:
important. Can we reproduce the results of Robertson et al. and the results of
John P. Steinberger?)

## Running Experiments

This repository has a number of "experiment branches", which are used to test
certain things in a (mostly) reproducible manner. (Accordingly, the `master`
branch is no longer in an reproducible state, because I have made attempts to
clean up and reorganize the files)

There currently exist the following experiment branches:

* experiment/jps-reduce-ring16
* experiment/jps-reduce-ring16_patched
* experiment/jps-reduce-all
* experiment/jps-reduce-instrumented
* experiment/jps-reduce-instrumented_patched
* experiment/jps-discharge
* experiment/rsst-reduce
* experiment/rsst-discharge

In order to run and record an experiment, perform the following steps.

1. Check out the correct branch.
   ```bash
   $ git checkout <name of the branch>
   ```

2. Run the experiment.
   ```bash
   $ ./build.sh <name of the run>
   ```

   Each run must be given a name. For example, for the first run of this
   experiment on RN's MacBook Pro, one might use the run name `RN-macbook-1`.
   
   This will compile and run the program, placing all the results under a
   subdirectory with the same name as the run. The subdirectory will be
   automatically committed.

3. Push the experiment results.
   
   The run was automatically committed, but it needs to be pushed to the main
   repository.

   ```bash
   $ git push origin <name of the branch>
   ```

## Creating Branches

In the event that you wish to create an entirely new set of experiment runs (not
building off of the existing ones), it is necessary to set up each branch before
the experiment. There is current a work-in-progress script called
`create_branches.sh` that attempts to serve this purpose.

Currently, 3 of the 8 branches have been implemented in `create_branches.sh`.
The following branches still need to be added to `create_branches.sh`.

* experiment/jps-reduce-instrumented
* experiment/jps-reduce-ring16_patched
* experiment/jps-reduce-instrumented_patched
* experiment/rsst-reduce
* experiment/rsst-discharge

### Creating a New Branch

To create a new branch, you should understand the `git worktree`, `diff`, and
`patch` commands.  `git worktree` allows you to check out a copy of a branch
into a subdirectory.  From there, we use the `patch` command to apply changes to
the subdirectory and finally commit the changes and close the worktree.

As of my current version of git, `git version 2.15.1 (Apple Git-101)`, there is
not a `git worktree remove` subcommand. You must instead manually delete the
subdirectory (using `rm -rf <the-subdirectory>`) and then use `git worktree
prune` to clean up git's internal state.

### Developing Patches for a New Branch

Currently, the `patches/` subdirectory contains a fair selection of the patches
you will need to create a new branch. E.G., performing only
discharging/reduction, not both, or reducing only a particular configuration
instead of all the configurations. However, for each branch, there are still
tweaks that must be made, necessitating new patches.

The `dev_patches` function in `create_branches.sh` is intended as as a place to
experiment with patches and to test them before they go into "production".

In general, here is workflow I found useful while developing my branches.

* Create a worktree off of the `experiment-base` tag. This tag contains basic
  infrastructure for an experiment, but it needs to be customized before it can
  be used.
* Create another worktree for the branch that you're interested in. This allows
  you to see what changes you need to make, compared to the `experiment-base`
  branch.
* `cp <experiment-base subdirectory>/build.sh temp_build.sh`. This gives you a
  copy of the experiment build script that you can modify at will, and then
  compare to the original.
* Copy over a set of changes to `temp_build.sh`. Try to keep the changes
  coherent and related.  (This is why there are `reduce_only`, `get_ring16` and
  `reduce_ring16` patches instead of a single, non-reusable
  `setup_jps-reduce-ring16` patch)
* Turn the changes you made into a patch. This is done by redirecting the output
  of a `diff` or `git diff` command into a file with the `.patch` suffix. For
  example, `diff <experiment-base subdirectory>/build.sh temp_build.sh
  >patches/add_instrumentation.patch` might be an example command invocation for a patch
  that instruments the reduction of a set of configurations.
* Add a `apply_patch <file targeted by a patch> patches/<the patch you want to
  apply>` command to `create_branches.sh`, under the relevant experiment.
* Once you're satisfied with a patch, clean up after yourself.
  * delete `temp_build.sh`
  * delete the worktree for the branch in question
  * run `git worktree prune`

And of course, remember to commit the changes you made to `create_branches.sh`.
(You do not -- and should not -- commit changes made to `temp_build.sh`, the
branch you're interested in, or `experiment-base`. `experiment-base` should no
longer need permament modifications, and the purpose of this script is to
create/apply the experiment branches, not make manual changes.)

### Miscellaneous Notes and Issues

#### `.orig` Files

One minor issue I have not yet resolved is that of multiple patches. Basically,
when applying two or more patches in succession, later patches may not align
exactly (because of insertions/deletions made by earlier patches). In this
scenario, the `patch` tool is cautious and creates a backup file like
`reduce.c.orig`. There is no harm to creating or committing such a file, but it
adds clutter and feels inelegant.

#### `tenative_*.patch` Files

In `patches/`, I have placed two "tenative" patch files. I used them earlier in
my own developments, but I suspect that they will need minor tweaks before using
them in `create_branches.sh`. However, I still wanted to keep them around,
because they do a pretty clear job of explaining how to patch `reduce.c` so it
doesn't crash anymore on Steinberger's configuration 783, and how to
"instrument" `reduce.c` to record the maximum "augment depth" attained while
reducing a configuration.

<!-- vim: set tw=80: -->
