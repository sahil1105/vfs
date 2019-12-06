main() {
  local outdir=$1;
  source compile-patched.sh $outdir
  source run-exp.sh $outdir config-files/ring16.conf
}

main $1