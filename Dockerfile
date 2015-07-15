FROM debian:testing

MAINTAINER Christopher A. Mosher <cmosher01@gmail.com>

ENV DEBIAN_FRONTEND noninteractive
RUN apt-get update && \
    apt-get install --no-install-recommends -y \
        asciidoc \
        automake \
        build-essential \
        libsdl2-dev \
        nginx \
        source-highlight \
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



ENV BUILD_LOG /var/log/build.log

RUN ./bootstrap 2>&1 | tee -a $BUILD_LOG
RUN ./configure 2>&1 | tee -a $BUILD_LOG
RUN make 2>&1 | tee -a $BUILD_LOG
RUN make check 2>&1 | tee -a $BUILD_LOG
RUN make install 2>&1 | tee -a $BUILD_LOG
RUN make html 2>&1 | tee -a $BUILD_LOG
RUN make install-html 2>&1 | tee -a $BUILD_LOG





COPY nginx.conf /etc/nginx/
COPY default.nginx /etc/nginx/sites-available/default
RUN ln -s /usr/local/share/doc/epple2 /usr/share/nginx/html/epple2

# supervisor
CMD ["supervisord"]
COPY supervisord.conf /etc/supervisor/conf.d/

ENV DISPLAY :0
ENV SDL_VIDEODRIVER x11

EXPOSE 80 5900
