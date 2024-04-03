tar -xzf parsec-inputs.tar.gz
pushd parsec-inputs

rm -rf ../pkgs/apps/blackscholes/inputs
cp -r blackscholes-inputs ../pkgs/apps/blackscholes/inputs

rm -rf ../pkgs/apps/fluidanimate/inputs
cp -r fluidanimate-inputs ../pkgs/apps/fluidanimate/inputs

rm -rf ../pkgs/kernels/canneal/inputs
cp -r canneal-inputs ../pkgs/kernels/canneal/inputs

rm -rf ../pkgs/kernels/dedup/inputs
cp -r dedup-inputs ../pkgs/kernels/dedup/inputs

popd