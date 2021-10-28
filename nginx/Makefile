IMAGE_NAME = nginx\:webserv
CONTAINER_NAME = nginx_webserv
PORT = 8080

PWD = $(shell pwd)

CONFIG_CONTAINER=type=bind,source=$(PWD)/conf,target=/conf
CONTENT_CONTAINER=type=bind,source=$(PWD)/html,target=/html


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
		@echo "fclean:		Remove container, images and data directories"

		@echo ""
		@echo "'docker ps [-a]' to see running/all containers"
		@echo "'docker images' to see all images"


all: 	help

build:
		docker build . -t $(IMAGE_NAME)

run:
		mkdir -p conf html
		docker run --name $(CONTAINER_NAME) -d -p $(PORT):80 --mount $(CONFIG_CONTAINER) --mount $(CONTENT_CONTAINER) $(IMAGE_NAME)

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

fclean: clean iclean
		-rm -rf conf html
