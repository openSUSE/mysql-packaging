#!/bin/sh
mv rc.mysql.sysvinit rc.mysql-multi
sed -i 's|/etc/my.cnf.d|/etc/mysql|' *.spec
