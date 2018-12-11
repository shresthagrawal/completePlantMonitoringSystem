Steps to start the Project.

* upload the Esp Client Code to Node MCU

* upload the GPIOPins Code to Arduino Due/ Mega

* Connect all the sensors as given in the comment in the TheGPIOPins code. Connect the nodeMcu with the due/mega 

* Configure System for running Elastic Stack inside docker.

Increasing the map cnt
$ sudo sysctl -w vm.max_map_count=262144

running the docker deamon
$ dockerd

set file descripto to 262144, do port forwarding, set ram for Stack, and pull the image, mount the elk-data folder to save the log files and start the container in interactive mode.(run this from the root dir of the repo!)

$ sudo docker run -p 5601:5601 -p 9200:9200 -p 5044:5044 -it --rm --ulimit nofile=262144:262144 --memory 8000m -v $PWD/elk-data:/var/lib/elasticsearch --name elk-test sebp/elk

* Wait for sometime to start the stack and then open 
http://localhost:5601 in chrome 

Asciinema: test.cast
  
