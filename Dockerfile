FROM debian:testing

MAINTAINER Christopher A. Mosher <cmosher01@gmail.com>

ENV DEBIAN_FRONTEND noninteractive
RUN apt-get update && \
    apt-get upgrade -y && \
    apt-get install --no-install-recommends -y \
        asciidoc \
        automake \
        build-essential \
        libsdl2-dev \
#        nginx \
        source-highlight \
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

COPY src/ ./src/
COPY conf/ ./conf/
COPY rom/ ./rom/
COPY installer/ ./installer/
COPY doc/ ./doc/



ENV BUILD_LOG /var/log/build.log

RUN ./bootstrap 2>&1 | tee -a $BUILD_LOG
RUN ./configure 2>&1 | tee -a $BUILD_LOG
RUN make clean 2>&1 | tee -a $BUILD_LOG
RUN make 2>&1 | tee -a $BUILD_LOG
#RUN make check 2>&1 | tee -a $BUILD_LOG
#RUN make dist 2>&1 | tee -a $BUILD_LOG
#RUN make distcheck 2>&1 | tee -a $BUILD_LOG
RUN make install 2>&1 | tee -a $BUILD_LOG
#RUN make html 2>&1 | tee -a $BUILD_LOG
#RUN make install-html 2>&1 | tee -a $BUILD_LOG
#RUN make installcheck 2>&1 | tee -a $BUILD_LOG

#RUN mkdir /usr/local/share/doc/epple2/download && \
#    cd /usr/local/share/doc/epple2/download && \
#    ln -s ~/epple2-*.tar.gz



#COPY nginx.conf /etc/nginx/
#COPY default.nginx /etc/nginx/sites-available/default
#RUN ln -s /usr/local/share/doc/epple2 /usr/share/nginx/html/epple2

# supervisor
RUN rm -R /etc/supervisor/*
ENTRYPOINT ["supervisord"]
CMD ["-c", "both"]
COPY supervisor/includes /etc/supervisor/
COPY supervisor/commands ./

ENV DISPLAY :0
ENV SDL_VIDEODRIVER x11

EXPOSE 80 5900
