IMAGE_NAME = nginx\:webserv
CONTAINER_NAME = nginx_webserv
PORT = 8081

help:
		@echo "Little nginx docker config to make some tests"	
		@echo ""
		@echo "Image name: $(IMAGE_NAME)"
		@echo "Container name: $(CONTAINER_NAME)"
		@echo "Port: $(PORT)"	
		@echo ""
		@echo "Makefile commands:"
		@echo "build:		Build the container from $(IMAGE_NAME)"
		@echo "run:		Run $(CONTAINER_NAME) container in detach mode on port $(PORT)"
		@echo "bash:		Jump into $(CONTAINER_NAME) container with bash ('exit' to quit)"
		@echo "start:		Start $(CONTAINER_NAME) container"
		@echo "stop:		Stop $(CONTAINER_NAME) container"
		@echo ""
		@echo "clean:		Remove $(CONTAINER_NAME) container"
		@echo "iclean:		Remove $(IMAGE_NAME) and nginx images"
		@echo ""
		@echo "'docker ps [-a]' to see running/all containers"
		@echo "'docker images' to see all images"


all: 	help

build:
		docker build . -t $(IMAGE_NAME)

run:
		docker run --name $(CONTAINER_NAME) -d -p $(PORT):80 $(IMAGE_NAME)

bash:
		docker exec -it $(CONTAINER_NAME) bash

start:
		-docker start $(CONTAINER_NAME)

stop:
		-docker stop $(CONTAINER_NAME)

clean: stop
		-docker rm $(CONTAINER_NAME)

iclean: clean
		-docker rmi -f $(IMAGE_NAME) nginx
