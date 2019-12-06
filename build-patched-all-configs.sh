main() {
  local outdir=$1;
  source compile-patched.sh $outdir
  source run-exp.sh $outdir config-files/U_2822.conf
}

main $1