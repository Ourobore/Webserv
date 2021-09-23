### A little nginx config to make some tests

Merge this branch into yours if necessary, there should be no conflics if you don't already have the nginx folder
You can modify the Dockerfile or Makefile to your needs, for now it just use the offical Nginx image.

I added Vim to be able to easily modify files from within

> !! Don't commit and push modification on this branch. Make an alternative branch, and if after discussion your modifications are good to everybody, we will merge them on this branch !!

If you don't know how to use the Makefile, `make` or `make help` will show you the commands and variables in it.
But I will detail here how to use it

You can use multiple commands at once: e.g. `make build run`

Usage:

1. Do a 'make build' to build the image from the Dockerfile
2. Then a 'make run' to run the container. It will run in detach mode, so you will still have your shell
   You can see the running containers with a `docker ps [-a]`. `[-a]` option is used to also see stoped containers
3. It is possible to jump into the container with `make bash` and modify it from within
   It is equivalent to `docker exec -it <container_name> bash`. `exit` to quit it
4. The default nginx config seems to be in `/etc/nginx/nginx.conf` and the content folder for the website in `/usr/share/nginx/html`
   You can either modify them by hand when jumping in the container with a docker exec, or add files to COPY in the Dockerfile
5. `make stop` to just stop the container, not delete it. When you restart it, you will still have your modifications
6. `make start` to start a stoped container
7. `make clean` to remove the container (will stop it if it is running)
8. `make iclean` calls `make clean` and then delete the official nginx image and our image
