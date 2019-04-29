# vfs
Studien zum Vierfarbensatz

# Fall 2018

This is a project for NCSA SPIN 2018

## Running Experiments

This repository has a number of "experiment branches", which are used to test
certain things in a (mostly) reproducible manner.

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


<!-- vim: set tw=80: -->
