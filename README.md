config-files : Some standard configuration files that can be run with the 'reduce' C program.

reduce-src-files : Versions of the 'reduce' C program. Currently there are 2 versions, 'reduce-patched.c' and 'reduce-unpatched.c'. They are both instrumented (have additional print statements such as configuration counter, maximum augmentation depth for each configuration, etc.). They both have a macro defined in the file called 'WEIGHT_SIZE'. In 'reduce-unpatched.c' it is set to 8, which is the value it originally was set to by the original authors. In 'reduce-patched.c', this macro is set to 16, which should be more than sufficient for it to work on systems the 'unpatched' version might have failed on.

The experiment takes place in two steps, a 'compile' and a 'run' step. In the 'compile' step, we specify the version of the code we want to use (patched, unpatched, etc.) and this creates an experiment folder with a copy of the code, the compiled executable and some additional meta data about the system. In the 'run' step we specify the experiment-folder (hence the compiled executable) we want to use, and the configuration file we want to run it on. This runs and stores the output of the running the configuration file through this executable, along with some additional system information.

To help with running experiments, we have provided the following scripts:
- compile.sh
- compile-patched.sh
- compile-unpatched.sh
- run-exp.sh
- build.sh
- build-patched.sh
- build-unpatched.sh
- build-patched-all-configs.sh
- build-patched-ring16-config.sh
- build-unpatched-all-configs.sh
- build-unpatched-ring16-config.sh

To be able to database these runs, we have provided a script '4ct_exp_databaser.py' which takes as arguments the experiment-directory and the name of a sqlite database (creates a new one if it doesn't already exist), in that order.
The files it gets the information from in the exp-dir are:
- cc.txt : Information about the compiler
- compile_os.txt : Information about system used for source code compilation
- runtime_os.txt : Information about system used to run the compiled executable
- version_notes.txt : Some notes about the experiment (can be added manually, some added automatically is using one of the specialized build/compile scripts)
- reduce-stdout.txt : The actual output of running the reduce C program on the provided configuration.
