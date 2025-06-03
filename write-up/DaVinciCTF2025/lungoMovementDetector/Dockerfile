FROM ubuntu:focal

RUN apt update && apt install -y wget gnupg net-tools python3 python3-pip vim
RUN apt-get install -y mosquitto
RUN apt-get install -y mosquitto-clients

RUN pip3 install paho-mqtt==1.6.0

COPY app /running
COPY app/mosquitto.conf /etc/mosquitto/
COPY app/mosquitto.acl /mosquitto.acl
COPY certs /certs
# COPY app/hosts /etc/hosts

RUN chmod +x /running/start.sh
RUN chmod 777 -R /var/run
ENTRYPOINT ["/running/start.sh"]
