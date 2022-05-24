 #!/bin/sh
 service  mysql start
  #mkdir -p /var/mysql/wordpress
  #chown -R mysql:mysql /var/mysql/wordpress
  mysql -u root  -e "CREATE USER 'wordpress'@'localhost' IDENTIFIED BY '12345678';"
  mysql -u root  -e "CREATE DATABASE wordpress; GRANT ALL PRIVILEGES ON wordpress.* TO 'wordpress'@'localhost' IDENTIFIED BY '12345678'; FLUSH PRIVILEGES;"
  mysql -u root  -e "source /app/wordpress.sql;"
 mysql -u root -e "alter user 'root'@'localhost' identified by 'rootPass'; FluSH PRIVILEGES;"