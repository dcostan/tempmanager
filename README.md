# Temperature manager Graphical User Interface
This program is used to manage the room temperature using a Qt GUI and additional drivers and services executed through systemd.

## Installing the dependancies
A prebuilt script to install all the required software can be found below
```bash
apt update
apt install python3-serial python3-numpy libqt6core6 qmake6 cmake qt6-base-dev-tools qt6-base-dev libqt6widgets6 libqt6charts6 qt6-charts-dev cmake php libapache2-mod-php apache2 dirmngr -y

wget https://github.com/rogersce/cnpy/archive/master.zip
unzip master.zip
cd cnpy-master
cmake .
make install
```
