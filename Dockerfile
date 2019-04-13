FROM debian:testing

MAINTAINER Christopher A. Mosher <cmosher01@gmail.com>

ENV DEBIAN_FRONTEND noninteractive
RUN apt-get update && \
    apt-get upgrade -y && \
    apt-get install --no-install-recommends -y \
        automake \
        build-essential \
        libsdl2-dev \
        supervisor \
        xa65 \
        xvfb \
        x11vnc \
        && \
    apt-get clean





WORKDIR /root/



COPY config.site  /usr/local/share/

COPY bootstrap configure.ac Makefile.am ./

COPY NEWS README AUTHORS ChangeLog COPYING* ./

# quick-start ROMs and floppy disk:
ADD https://mosher.mine.nu/monitor.a65 ./
ADD https://mosher.mine.nu/applesoft.a65 ./
ADD https://mosher.mine.nu/disk2.a65 ./
ADD https://mosher.mine.nu/choplifter.woz ./
COPY setup.conf ./

COPY src/ ./src/
COPY conf/ ./conf/
COPY rom/ ./rom/



ENV BUILD_LOG /var/log/build.log

RUN ./bootstrap 2>&1 | tee -a $BUILD_LOG
RUN ./configure 2>&1 | tee -a $BUILD_LOG
RUN make clean 2>&1 | tee -a $BUILD_LOG
RUN make 2>&1 | tee -a $BUILD_LOG
RUN make install 2>&1 | tee -a $BUILD_LOG



# supervisor
CMD ["supervisord"]
COPY supervisord.conf /etc/supervisor/conf.d/

ENV DISPLAY :0
ENV SDL_VIDEODRIVER x11

EXPOSE 5900
