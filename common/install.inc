# install the debug server first, so that we don't overwrite the standard
# /usr/sbin/mysqld
mkdir -p %buildroot%_libdir/mysql
make -C _build-debug/sql install DESTDIR=%buildroot
mv %buildroot/usr/sbin/mysqld %buildroot/usr/sbin/mysqld-debug
install -m 644 _build-debug/sql/mysqld.sym %buildroot%_libdir/mysql/mysqld-debug.sym
# install the client, extra tools, libraries and the standard server
make -C _build install DESTDIR=%buildroot benchdir_root=/usr/share/
install -m 644 _build/sql/mysqld.sym %buildroot%_libdir/mysql/mysqld.sym
# remove handler socket client
rm -f %buildroot%_libdir/libhsclient*
rm -f %buildroot%_bindir/hsclient*
if [ -f "%{buildroot}%{_bindir}/mytop" ]; then
	mv %{buildroot}%{_bindir}/mytop %{buildroot}%{_bindir}/%{name}_mytop
fi
rm -f %buildroot%_libdir/mysql/plugin/handlersocket.so
# remove most static libs (FIXME: don't build them at all...)
rm -f %buildroot%_libdir/libmysqlclient*a
rm -f %buildroot%_libdir/libmysqlservices*a
rm -f %buildroot%_libdir/mysql/*.a %buildroot%_libdir/mysql/*.la
rm -f %buildroot%_libdir/mysql/plugin/*a
# XXX: package these? The ndb tools are linked statically atm
rm -f %buildroot%_libdir/mysql/libndbclient.*
rm -f %{buildroot}/usr/share/mysql-test/mysql-test-run  
rm -f %{buildroot}/usr/share/mysql-test/mtr
rm -f %{buildroot}/usr/share/mysql/binary-configure
rm -f %{buildroot}/usr/share/mysql/errmsg-utf8.txt
rm -f %{buildroot}/usr/share/mysql/magic
rm -f %{buildroot}/usr/share/mysql/mysql-log-rotate
rm -f %{buildroot}/usr/share/mysql/mysql*.server
rm -f %{buildroot}%{_libdir}/mysql/plugin/daemon_example.ini
ln -sf mysql-test-run.pl %{buildroot}/usr/share/mysql-test/mtr  
ln -sf mysql-test-run.pl %{buildroot}/usr/share/mysql-test/mysql-test-run 
mv %{buildroot}/usr/share/mysql-test/lib/My/SafeProcess/my_safe_process %{buildroot}%{_bindir}
# generate filelist for binaries and their manpages (easier than enumerating
# all the binaries and manapages manually, esp. when there are new manpages in
# each release)
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
		if ls usr/share/man/*/"$i".[1-9]* >/dev/null 2>&1; then
			echo "%%doc %_mandir/*/$i.[1-9]*"
		fi
	done
	popd >/dev/null
}
filelist innochecksum my_print_defaults myisam_ftdump myisamchk myisamlog myisampack mysql_fix_extensions mysql_fix_privilege_tables mysql_install_db mysql_secure_installation mysql_upgrade mysqlbug mysqld mysqld_multi mysqld_safe mysqlbinlog mysqldumpslow mysqlmanager resolve_stack_dump resolveip {m,}aria_chk {m,}aria_dump_log {m,}aria_ftdump {m,}aria_pack {m,}aria_read_log xtstat >mysql.files
filelist mysql mysqladmin mysqlcheck mysqldump mysqlimport mysqlshow >mysql-client.files
filelist mysql_config >libmysqlclient-devel.files
filelist mysqlslap >mysql-bench.files
filelist mysql_client_test mysql_client_test_embedded mysql_waitpid mysqltest mysqltest_embedded >mysql-test.files
# all binaries that don't need to be in a "default" install (mysql.rpm +
# mysql-client.rpm)
filelist msql2mysql mysql_config_editor mysql_plugin mysql_convert_table_format mysql_find_rows mysql_setpermission mysql_tzinfo_to_sql mysql_zap mysqlaccess mysqlhotcopy perror replace mysql_embedded %{name}_mytop hsclient >mysql-tools.files
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
DOCS=(COPYING README EXCEPTIONS-CLIENT %_sourcedir/README.debug %_sourcedir/README.SuSE plugin/daemon_example/daemon_example.ini)
DOCDIR=%buildroot%_defaultdocdir/%name
install -d -m 755 ${DOCDIR}
install -d -m 755 %buildroot/etc/logrotate.d
install -d -m 755 %buildroot/etc/init.d
install -m 644 _build/support-files/mysql-log-rotate %buildroot/etc/logrotate.d/mysql
%if 0%{?prefered} > 0%{?use_cmake}
install -m 644 _buildlibmysqld/*.so* %buildroot%_libdir
ln -s libmysqld.so.0.0.1 %buildroot%_libdir/libmysqld.so.0
ln -s libmysqld.so.0.0.1 %buildroot%_libdir/libmysqld.so
%endif
install -m 660 %{S:14} %buildroot/etc/my.cnf
install -m 640 scripts/mysqlaccess.conf %buildroot/etc/mysqlaccess.conf
for i in "${DOCS[@]}"; do
	install -m 644 "${i}" "${DOCDIR}" || true
done
install -m 755 %_sourcedir/rc.mysql-multi %buildroot/etc/init.d/mysql
ln -sf ../../etc/init.d/mysql %buildroot/usr/sbin/rcmysql
# this is used by the init script
install -m 755 -d %buildroot/var/run/mysql
# SuSEfirewall service description
install -D -m 644 %_sourcedir/mysql.SuSEfirewall2 \
	%buildroot/etc/sysconfig/SuSEfirewall2.d/services/mysql
# testsuite
mkdir -p '%buildroot'/usr/share/mysql-test/
install -m 755 suse-test-run '%buildroot'/usr/share/mysql-test/
mkdir '%buildroot'/usr/share/mysql-test/var
# Final fixes
find '%buildroot'/usr/share/mysql-test -name '*.orig' -delete
%if 0%{?suse_version} > 1030
%fdupes -s '%buildroot'/usr/share/mysql-test
%endif
%if 0%{?fedora_version} > 8
fdupes -q -n -r '%buildroot'/usr/share/mysql-test
%endif
dos2unixConversion() {
	%{__sed} -i 's/\r//g' "$1"
}
for i in `grep -Rl '\r' '%buildroot'/usr/share/sql-bench`; do
	dos2unixConversion "$i"
done
%if ! 0%{cluster} > 0
cat mysql-ndb-storage.files mysql-ndb-management.files  \
	mysql-ndb-tools.files mysql-ndb-extra.files |   \
	sed 's|^%%doc\ ||' | while read tmp; do
	cd '%buildroot'
	rm -f .$tmp
done
%endif
%if ! 0%{prefered} > 0
cat libmysqlclient-devel.files | sed 's|%%doc\ ||' | while read tmp; do
	cd '%buildroot'
	rm -f ."$tmp"
done
rm -rf '%buildroot'/%_libdir/libmysqld.*
rm -rf '%buildroot'/%_libdir/*.la
rm -rf '%buildroot'/%_libdir/*.so
rm -rf '%buildroot'/%_datadir/man/*/mysql_config.1*
rm -rf '%buildroot'/%_includedir  
rm -rf '%buildroot'/%_datadir/aclocal
rm -rf '%buildroot'/%_libdir/*.a
%endif
sed -i 's|doc/packages/mysql|doc/packages/%{name}|g' '%buildroot'/etc/init.d/mysql
ln -s mysqlcheck '%buildroot'%_bindir/mysqlrepair
ln -s mysqlcheck '%buildroot'%_bindir/mysqlanalyze
ln -s mysqlcheck '%buildroot'%_bindir/mysqloptimize
%if 0%{?cluster} > 1
ln -s libndbclient.so.6.0.0 '%buildroot'%_libdir/libndbclient.so.6.0
ln -s libndbclient.so.6.0.0 '%buildroot'%_libdir/libndbclient.so.6
%endif
if [ -f '%buildroot'/usr/bin/mysqlaccess.conf ]; then
	mv '%buildroot'/usr/bin/mysqlaccess.conf '%buildroot'/etc/mysqlaccess.conf
fi
mkdir -p '%buildroot'/etc/my.cnf.d
rm -rf '%buildroot'/etc/my.cnf.d/*
bzip2 -cd '%_sourcedir'/configuration-tweaks.tar.bz2 | tar -C '%buildroot'/etc/my.cnf.d -xvf -
if [ -z "`ls '%buildroot'/usr/share/info/mysql.info*`" ]; then
	install -D -m 0644 Docs/mysql.info "%buildroot/usr/share/info/mysql.info"
	gzip -9 "%buildroot/usr/share/info/mysql.info"
fi
if [ "`ls '%buildroot'%_libdir/mysql/plugin/dialog*.so`" ]; then
	echo '%%dir %%_libdir/mysql'               >> mysql-client.files
	echo '%%dir %%_libdir/mysql/plugin'        >> mysql-client.files
	echo '%%_libdir/mysql/plugin/dialog*.so'   >> mysql-client.files
fi
rm -rf '%buildroot'/usr/share/doc/{mysql,mariadb}*
rm -rf '%buildroot'/usr/share/mysql/solaris
rm -rf '%buildroot'/usr/share/mysql/SELinux
