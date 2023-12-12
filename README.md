# Temperature manager Graphical User Interface
This program is used to manage the room temperature using a Qt GUI and additional drivers and services executed through systemd.

## Installing the dependancies
A prebuilt script to install all the required software can be found below
```bash
apt update
apt install python-serial python-numpy libqt5core5a qt5-qmake cmake qtbase5-dev-tools qtbase5-dev libqt5widgets5 cmake php libapache2-mod-php apache2 dirmngr -y
apt install libqt5charts5 libqt5charts5-dev -y

wget https://github.com/rogersce/cnpy/archive/master.zip
unzip cnpy-master.zip
cd cnpy-master
cmake
make install

apt install certbot python-certbot-apache -y --force-yes
```
