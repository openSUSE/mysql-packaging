%cmake_install benchdir_root=/usr/share/
install -m 644 build/sql/mysqld.sym %buildroot%_libdir/mysql/mysqld.sym
# remove handler socket client
rm -f %buildroot%_libdir/libhsclient*
rm -f %buildroot%_bindir/hsclient*
[ \! -f "%{buildroot}%{_bindir}/mytop" ] || mv %{buildroot}%{_bindir}/mytop %{buildroot}%{_bindir}/%{name}_mytop
rm -f %buildroot%_libdir/mysql/plugin/handlersocket.so
# remove most static libs (FIXME: don't build them at all...)
[ \! -f "%buildroot%_libdir/"libmysqld.a ] || mv "%buildroot%_libdir/"libmysqld.a "%buildroot%_libdir/"libmysqld.static
rm -f %buildroot%_libdir/*.a
[ \! -f "%buildroot%_libdir/"libmysqld.static ] || mv "%buildroot%_libdir/"libmysqld.static "%buildroot%_libdir/"libmysqld.a
rm -f %buildroot%_libdir/mysql/{,plugin}/*.{a,la}
rm -f %buildroot%_libdir/mysql/libndbclient.*
rm -f %buildroot%_libdir/mysql/plugin/debug_key_management.so
rm -f %{buildroot}/usr/share/mysql-test/{mysql-test-run,mtr}
rm -f %{buildroot}/usr/share/mysql/{binary-configure,errmsg-utf8.txt,magic,mysql-log-rotate,mysql*.server}
rm -f %{buildroot}%{_libdir}/mysql/plugin/daemon_example.ini
ln -sf mysql-test-run.pl %{buildroot}/usr/share/mysql-test/mtr  
ln -sf mysql-test-run.pl %{buildroot}/usr/share/mysql-test/mysql-test-run 
mv %{buildroot}/usr/share/mysql-test/lib/My/SafeProcess/my_safe_process %{buildroot}%{_bindir}
# generate filelist for binaries and their manpages
filelist()
{
	echo '%%defattr(-, root, root)'
	pushd %buildroot >/dev/null
	for i; do
		if test -e usr/sbin/"$i"; then
			echo /usr/sbin/"$i"
		fi
		if test -e usr/bin/"$i"; then
			echo /usr/bin/"$i"
		fi
		if test -d usr/share/*/"$i"; then
			echo "/`echo usr/share/*/"$i"`"
		fi
		if test -n "`ls -1 %buildroot$i 2> /dev/null`"; then
			echo "$i"
		fi
		if ls usr/share/man/*/"$i".[1-9]* >/dev/null 2>&1; then
			echo "%%doc %_mandir/*/$i.[1-9]*"
		fi
	done
	popd >/dev/null
}
filelist innochecksum my_print_defaults myisam_ftdump myisamchk myisamlog myisampack mysql_fix_extensions mysql_fix_privilege_tables mysql_install_db mysql_secure_installation mysql_upgrade mysqlbug mysqld mysqld_multi mysqld_safe mysqlbinlog mysqldumpslow mysqlmanager mroonga resolve_stack_dump resolveip {m,}aria_chk {m,}aria_dump_log {m,}aria_ftdump {m,}aria_pack {m,}aria_read_log xtstat tokuftdump >mysql.files
filelist mysql mysqladmin mysqlcheck mysqldump mysqlimport mysqlshow mysql_config_editor >mysql-client.files
filelist mysql_config /usr/share/pkgconfig/mariadb.pc >libmysqlclient-devel.files
filelist mysqlslap >mysql-bench.files
filelist mysql_client_test mysql_client_test_embedded mysql_waitpid mysqltest mysqltest_embedded >mysql-test.files
# all binaries that don't need to be in a "default" install (mysql.rpm +
# mysql-client.rpm)
filelist msql2mysql mysql_plugin mysql_convert_table_format mysql_find_rows mysql_setpermission mysql_tzinfo_to_sql mysql_zap mysqlaccess mysqlhotcopy perror replace mysql_embedded %{name}_mytop hsclient %{_bindir}/wsrep* /usr/share/mysql/wsrep_notify >mysql-tools.files
filelist ndbd ndbmtd ndbd_redo_log_reader >mysql-ndb-storage.files
filelist ndb_mgmd >mysql-ndb-management.files
filelist ndb_{config,desc,error_reporter,mgm,print_backup_file,print_schema_file,print_sys_file,restore,select_all,select_count,show_tables,size.pl,test_platform,waiter} >mysql-ndb-tools.files
filelist ndb_{cpcd,delete_all,drop_index,index_stat,print_file,redo_log_reader,drop_table} ndbinfo_select_all memcached >mysql-ndb-extra.files
[ -z "`ls -1 "%buildroot/usr/share/mysql/"*.ini`" ]   || echo '/usr/share/mysql/*.ini' >> mysql.files 
[ -z "`ls -1 "%buildroot/usr/share/mysql/"*.cnf`" ]   || echo '/usr/share/mysql/*.cnf' >> mysql.files 
[ \! -f "%buildroot/usr/share/%name"/dictionary.txt ] || echo "/usr/share/%name/dictionary.txt" >> mysql.files 
errmsg_list()
{
	echo '%%defattr(-, root, root)'
	pushd %buildroot >/dev/null
	for f in usr/share/%{name}/*; do
		if test -e $f/errmsg.sys; then
			echo "%%dir /$f"
		fi
	done
	echo /usr/share/%{name}/errmsg-utf8.txt
	popd >/dev/null
}
errmsg_list > errormessages.files
### files not installed by make install
# Create directory structure
DOCS=(COPYING README EXCEPTIONS-CLIENT %_sourcedir/README.debug %_sourcedir/README.SUSE plugin/daemon_example/daemon_example.ini)
DOCDIR=%buildroot%_defaultdocdir/%name
install -d -m 755 ${DOCDIR}
install -d -m 755 %buildroot/etc/logrotate.d
install -m 644 build/support-files/mysql-log-rotate %buildroot/etc/logrotate.d/mysql
install -m 660 %{S:14} %buildroot/etc/my.cnf
[ \! -f scripts/mysqlaccess.conf ] || install -m 640 scripts/mysqlaccess.conf %buildroot/etc/mysqlaccess.conf
for i in "${DOCS[@]}"; do
	install -m 644 "${i}" "${DOCDIR}" || true
done
install -D -m 755 %_sourcedir/rc.mysql.systemd '%buildroot'/usr/lib/mysql/rcmysql
sed -i 's|@MYSQLVER@|%{version}|' '%buildroot'/usr/lib/mysql/rcmysql
ln -sf service '%buildroot'/usr/sbin/rcmysql
install -D -m 644 %_sourcedir/mysql.service '%buildroot'/usr/lib/systemd/system/mysql.service
sed -i 's|doc/packages/mysql|doc/packages/%{name}|g' '%buildroot'/usr/lib/mysql/rcmysql
rm -rf '%buildroot'/etc/init.d
# SuSEfirewall service description
install -D -m 644 %_sourcedir/mysql.SuSEfirewall2 \
	%buildroot/etc/sysconfig/SuSEfirewall2.d/services/mysql
# testsuite
mkdir -p '%buildroot'/usr/share/mysql-test/
install -m 755 suse-test-run '%buildroot'/usr/share/mysql-test/
mkdir '%buildroot'/usr/share/mysql-test/var
# Final fixes
find '%buildroot'/usr/share/mysql-test -name '*.orig' -delete
%fdupes -s '%buildroot'/usr/share/mysql-test
fdupes -q -n -r '%buildroot'/usr/share/mysql-test
dos2unixConversion() {
	%{__sed} -i 's/\r//g' "$1"
}
for i in `grep -Rl '\r' '%buildroot'/usr/share/sql-bench`; do
	dos2unixConversion "$i"
done
%if ! 0%{preferred} > 0
cat libmysqlclient-devel.files | sed 's|%%doc\ ||' | while read tmp; do
	cd '%buildroot'
	rm -f ."$tmp"
done
rm -rf '%buildroot'/%_libdir/libmysqld.*
rm -rf '%buildroot'/%_libdir/{*.la,*.so,*.a}
rm -rf '%buildroot'/%_datadir/man/*/mysql_config.1*
rm -rf '%buildroot'/%_includedir  
rm -rf '%buildroot'/%_datadir/aclocal
%endif
ln -s mysqlcheck '%buildroot'%_bindir/mysqlrepair
ln -s mysqlcheck '%buildroot'%_bindir/mysqlanalyze
ln -s mysqlcheck '%buildroot'%_bindir/mysqloptimize
if [ -f '%buildroot'/usr/bin/mysqlaccess.conf ]; then
	mv '%buildroot'/usr/bin/mysqlaccess.conf '%buildroot'/etc/mysqlaccess.conf
fi
[ \! -f '%buildroot'/etc/mysqlaccess.conf ] || echo '%config(noreplace) %attr(0640, root, mysql) /etc/mysqlaccess.conf' >> mysql-client.files
rm -rf '%buildroot'/etc/my.cnf.d
mkdir -p '%buildroot'/etc/my.cnf.d
bzip2 -cd '%_sourcedir'/configuration-tweaks.tar.bz2 | tar -C '%buildroot'/etc/my.cnf.d -xvf -
[ -z "`ls '%buildroot'/usr/share/info/mysql.info* 2> /dev/null`" ] || rm -f '%buildroot'/usr/share/info/mysql.info*
[ -z "`ls '%buildroot'/usr/share/mysql-test/suite/innodb/t/*.zip`" ] || chmod a-x '%buildroot'/usr/share/mysql-test/suite/innodb/t/*.zip
if [ "`ls '%buildroot'%_libdir/mysql/plugin/dialog*.so 2> /dev/null`" ]; then
	echo '%%dir %%_libdir/mysql'               >> mysql-client.files
	echo '%%dir %%_libdir/mysql/plugin'        >> mysql-client.files
	echo '%%_libdir/mysql/plugin/dialog*.so'   >> mysql-client.files
fi
if [ %{_defaultdocdir} \!= /usr/share/doc ]; then
rm -rf '%buildroot'/usr/share/doc/{mysql,mariadb}*
fi
if [ %{name} \!= mysql ]; then
rm -rf '%buildroot'/usr/share/doc/mysql
fi
if [ %{name} \!= mariadb ]; then
rm -rf '%buildroot'/usr/share/doc/mariadb
fi
rm -f  '%buildroot'/usr/share/doc/* 2> /dev/null || true
rm -rf '%buildroot'/usr/share/mysql/{solaris,SELinux}
