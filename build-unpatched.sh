main() {
  local outdir=$1;
  local configfile=$2;
  source compile-unpatched.sh $outdir
  source run-exp.sh $outdir $configfile
}

main $1 $2