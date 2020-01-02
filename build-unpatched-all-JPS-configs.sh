main() {
  local outdir=$1;
  source compile-unpatched.sh $outdir
  source run-exp.sh $outdir config-files/JPS-2822.conf
}

main $1