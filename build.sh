main() {
  local outdir=$1;
  local reduce_src=$2;
  local configfile=$3;
  source compile.sh $outdir $reduce_src
  source run-exp.sh $outdir $configfile
}

main $1 $2 $3