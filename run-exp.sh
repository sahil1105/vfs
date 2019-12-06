record_system() {
  local expdir=$1
  uname -a >$expdir/runtime_os.txt
}

run_reduce() {
  local expdir=$1
  local configfile=$2
  cp $configfile $expdir/config.conf
  time (./$expdir/reduce $expdir/config.conf) >$expdir/reduce-stdout.txt 2>$expdir/reduce-stderr.txt
}

main() {
  local expdir=$1;
  local configfile=$2;
  echo "Recording system information"
  record_system $expdir
  echo "Running reduce"
  run_reduce $expdir $configfile
}

main $1 $2