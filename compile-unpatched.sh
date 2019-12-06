add_version_notes() {
	local outdir=$1;
	cat >$outdir/version_notes.txt <<EOF
unpatched-version, WEIGHT_SIZE=8, instrumented
EOF
}

main() {
  local outdir=$1;
  source compile.sh $outdir RSST/anc/reduce-unpatched.c
  add_version_notes $outdir
}

main $1