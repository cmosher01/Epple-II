FROM ubuntu

MAINTAINER Christopher A. Mosher <cmosher01@gmail.com>

RUN apt-get update && \
    apt-get install --no-install-recommends -y \
        autoconf \
        automake \
        build-essential \
        libsdl2-dev \
        supervisor \
        xvfb \
        x11vnc \
        && \
    apt-get clean

WORKDIR /root/

COPY bootstrap ./

COPY NEWS ./
COPY README ./
COPY AUTHORS ./
COPY ChangeLog ./

COPY configure.ac ./

COPY Makefile.am ./

COPY src/ ./src/
COPY conf/ ./conf/
COPY installer/ ./installer/
COPY doc/ ./doc/

RUN ./bootstrap
RUN ./configure
RUN make
RUN make check
RUN make install

# supervisor
CMD ["supervisord"]
COPY supervisord.conf /etc/supervisor/conf.d/

ENV DISPLAY :0
ENV SDL_VIDEODRIVER x11

EXPOSE 5900
