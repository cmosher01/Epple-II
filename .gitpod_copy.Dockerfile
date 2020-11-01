FROM gitpod/workspace-full

USER gitpod

# Install custom tools, runtime, etc. using apt-get
# For example, the command below would install "bastet" - a command line tetris clone:
#
# RUN sudo apt-get -q update && \
#     sudo apt-get install -yq bastet && \
#     sudo rm -rf /var/lib/apt/lists/*
#
# More information: https://www.gitpod.io/docs/config-docker/
FROM gitpod/workspace-full
USER gitpod
RUN sudo apt-get update -q && \
    sudo apt-get install -yq libsdl*   \
    sudo apt-get install qmc2-sdlmame qmc2-arcade qmc2-data qchdman

