FROM ubuntu

RUN apt -y update

RUN apt install -y --no-install-recommends apt-utils git build-essential python3 ninja-build cmake

ADD installref.sh /tmp/installref.sh
RUN chmod a+rx /tmp/installref.sh
RUN /tmp/installref.sh
