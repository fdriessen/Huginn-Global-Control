#!/bin/bash -x

echo "run ..."

#streamcam/./StreamCam
#stream_kinect05/./kinect

#camrecord/./camrecord test.avi & stream_kinect05/./kinect && fg

#prog1 & prog2 && fg
#This will:

#Start prog1.
#Send it to background, but keep printing its output.
#Start prog2, and keep it in foreground, so you can close it with ctrl-c.
#When you close prog2, you'll return to prog1's foreground, so you can also close it with ctrl-c.

# http://stackoverflow.com/questions/9023164/in-bash-how-can-i-run-multiple-infinitely-running-commands-and-cancel-them-all
#run_development_webserver.sh &
#PIDS[0]=$!
#watch_sass_files_and_compile_them.sh &
#PIDS[1]=$!
#watch_coffeescript_files_and_compile_them.sh & 
#PIDS[2]=$!
#trap "kill ${PIDS[*]}" SIGINT
#wait

camrecord/./camrecord test.avi & 
PIDS[0]=$!
stream_kinect05/./kinect &
PIDS[1]=$!

#last program runs on foreground

trap "kill ${PIDS[*]}" SIGINT
wait