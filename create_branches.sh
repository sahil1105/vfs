

# This script will create the experiment branches necessary for replicating our results.



# TODO: REMEMBER TO FIX THE THING IN jps-reduce-all, WITH THE EXTRANEOUS
# get_ring16


### Infrastructure stuff
# These functions implement the commands used below. It may be helpful to read
# through them to see how they work, but it is not crucial.

experiment_name=""
directory_name=""
experiment_branch=""

function create_experiment() {
    experiment_name=$1
    directory_name=branch-$experiment_name
    experiment_branch=experiment/$1

    echo "Creating experiment branch $experiment_branch"
    git branch $experiment_branch experiment-base
    git worktree add $directory_name $experiment_branch
}

function apply_patch() {
    local file=$1
    local patchfile=$2

    # Note: Patches and backup files
    # When applying multiple patches to a file in succession, the later patches
    # may not match exactly, because other patches inserted/deleted lines.
    # Consequently, (my version of) the `patch` program creates a `$file.orig`
    # file containing the original contents of the file. This is mostly
    # harmless, and is arguably even beneficial, because you can see all the
    # changes made to set up this branch.

    patch $directory_name/$file $patchfile
}

function finish_experiment() {
    git -C $directory_name add .
    git -C $directory_name commit -m "Setup for testing"
    rm -rf $directory_name
    echo "Created experiment branch $experiment_branch"
}


### Branch definitions
# This section uses the commands defined above to create a mini-language for
# creating new experiment branches.

# This function is temporary; it is used for developing new patches
function dev_patches() {
    create_experiment test-test-test
    finish_experiment

    git worktree prune
}


function create_branches() {
    create_experiment jps-reduce-ring16
    apply_patch build.sh patches/reduce_only.patch
    apply_patch build.sh patches/get_ring16.patch
    apply_patch build.sh patches/reduce_ring16.patch
    finish_experiment

    create_experiment jps-reduce-all
    apply_patch build.sh patches/reduce_only.patch
    apply_patch build.sh patches/reduce_all.patch
    finish_experiment

    create_experiment jps-discharge
    apply_patch build.sh patches/discharge_only.patch
    apply_patch build.sh patches/discharge_jps.patch
    finish_experiment

    # Basically, `git` does not provide a `git worktree remove $branch` command,
    # instead you must delete the directory manually. However, the deleted
    # worktrees still show up in `git worktree list`.
    # Thus, after dealing with all the branches, we run `git worktree prune` to
    # remove the worktrees that no longer exist.
    git worktree prune
}


# Once all branches are ready, delete this command and uncomment
# `create_branches`.
dev_patches
# create_branches

# vim: set tw=80 et sts=4 sw=4:
