#!/bin/bash
docker built -t machiry/refverifier:latest .
docker image push machiry/refverifier:latest
