# Docker image for refverifier

We have built a docker image containing latest version of `refverifier`. You can use it as below:

## Using existing docker image
```
docker run -it machiry/refverifier:latest
```

`refverifier` is present in the system path. You can use it by just running `refverifier`.

## Building new docker image

The following command will create a new image and push it to the repo: `machiry/refverifier:latest`

```
./build_and_push.sh
```
