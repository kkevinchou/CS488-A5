export RUN_TYPE=COORDINATOR
make

ssh cm2chou@gl01 "nohup pkill rt -u cm2chou &"
ssh cm2chou@gl01 "nohup ./w > & ~/test1 &"
ssh cm2chou@gl04 "nohup pkill rt -u cm2chou &"
ssh cm2chou@gl04 "nohup ./w > & ~/test1 &"
ssh cm2chou@gl05 "nohup pkill rt -u cm2chou &"
ssh cm2chou@gl05 "nohup ./w > & ~/test1 &"
ssh cm2chou@gl06 "nohup pkill rt -u cm2chou &"
ssh cm2chou@gl06 "nohup ./w > & ~/test1 &"
ssh cm2chou@gl07 "nohup pkill rt -u cm2chou &"
ssh cm2chou@gl07 "nohup ./w > & ~/test1 &"
ssh cm2chou@gl09 "nohup pkill rt -u cm2chou &"
ssh cm2chou@gl09 "nohup ./w > & ~/test1 &"
ssh cm2chou@gl10 "nohup pkill rt -u cm2chou &"
ssh cm2chou@gl10 "nohup ./w > & ~/test1 &"
ssh cm2chou@gl11 "nohup pkill rt -u cm2chou &"
ssh cm2chou@gl11 "nohup ./w > & ~/test1 &"
ssh cm2chou@gl12 "nohup pkill rt -u cm2chou &"
ssh cm2chou@gl12 "nohup ./w > & ~/test1 &"
ssh cm2chou@gl13 "nohup pkill rt -u cm2chou &"
ssh cm2chou@gl13 "nohup ./w > & ~/test1 &"
ssh cm2chou@gl14 "nohup pkill rt -u cm2chou &"
ssh cm2chou@gl14 "nohup ./w > & ~/test1 &"
ssh cm2chou@gl19 "nohup pkill rt -u cm2chou &"
ssh cm2chou@gl19 "nohup ./w > & ~/test1 &"
ssh cm2chou@gl23 "nohup pkill rt -u cm2chou &"
ssh cm2chou@gl23 "nohup ./w > & ~/test1 &"
ssh cm2chou@gl24 "nohup pkill rt -u cm2chou &"
ssh cm2chou@gl24 "nohup ./w > & ~/test1 &"
ssh cm2chou@gl26 "nohup pkill rt -u cm2chou &"
ssh cm2chou@gl26 "nohup ./w > & ~/test1 &"
ssh cm2chou@gl27 "nohup pkill rt -u cm2chou &"
ssh cm2chou@gl27 "nohup ./w > & ~/test1 &"

./rt