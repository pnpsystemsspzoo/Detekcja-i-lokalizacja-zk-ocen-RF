set -e
cd /home/mock/sonda-fpga/sources
echo -e "Cleaning old build"
make clean
echo -e "Building"
make
PID=`ps -ef  | grep 'app' | grep -v grep | awk '{print $2}'`
if [ ! -z "$PID" ]
then
    echo -e "Killing old mock instance PID=$PID"
    kill -9 $PID
fi
./app > /dev/null &
disown $!
echo -e "Deployed new mock app with PID=$!" 
echo -e "Done"
