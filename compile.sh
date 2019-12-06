
record_system() {
  local outdir=$1
  uname -a >$outdir/compile_os.txt
  cc --version >$outdir/cc.txt
}

build() {
  local outdir=$1
  local reduce_src=$2
  # The compiled executables are placed at the repository root so that we do not
  # commit many executables to GitHub.

  cp reduce_src $outdir/reduce.c
  #
  # We generate a shell script instead of directly issuing commands so that
  # the compile commands are recorded.
  cat >$outdir/compile.sh <<EOF
cc $outdir/reduce.c -o $outdir/reduce 2>$outdir/compile-reduce.txt
EOF

  # Note that the compilation script is invoked from the top-level directory,
  # so relative paths in $outdir/compile.sh (RSST/anc/reduce.c, reduce) are
  # interpreted as being relative to the top-level-directory.

  source $outdir/compile.sh
}

main() {
  local outdir=$1;
  local reduce_src=$2;
  echo "Creating output directory"
  mkdir -p $outdir
  echo "Recording system information"
  record_system $outdir
  echo "Compiling program"
  build $outdir $reduce_src
}

main $1 $2