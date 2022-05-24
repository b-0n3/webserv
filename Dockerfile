FROM debian:buster

RUN apt-get update -y
RUN apt-get install mariadb-server -y

ADD ./src/setup.sh /setup.sh

RUN apt-get install clang make php-cgi php-common php-fpm php-pear php-mbstring \
    php-zip php-net-socket php-gd php-xml-util php-gettext php-mysql php-bcmath unzip wget -y
#RUN /bin/sh /setup.sh


ADD  ./src /app

RUN mkdir -p /var/www/ ; mv /app/root /var/www/html

WORKDIR /app

ENTRYPOINT service mysql start&& /bin/sh /setup.sh &&  make re  && ./webserv config/default.yml