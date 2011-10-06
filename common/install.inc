# install the debug server first, so that we don't overwrite the standard
# /usr/sbin/mysqld
mkdir -p %buildroot%_libdir/mysql
make -C _build-debug/sql install DESTDIR=%buildroot
mv %buildroot/usr/sbin/mysqld %buildroot/usr/sbin/mysqld-debug
install -m 644 _build-debug/sql/mysqld.sym %buildroot%_libdir/mysql/mysqld-debug.sym
# install the client, extra tools, libraries and the standard server
make -C _build install DESTDIR=%buildroot benchdir_root=/usr/share/
install -m 644 _build/sql/mysqld.sym %buildroot%_libdir/mysql/mysqld.sym
# remove most static libs (FIXME: don't build them at all...)
rm -f %buildroot%_libdir/libmysqlclient*a
rm -f %buildroot%_libdir/mysql/*.a %buildroot%_libdir/mysql/*.la
rm -f %buildroot%_libdir/mysql/plugin/*a
# XXX: package these? The ndb tools are linked statically atm
rm -f %buildroot%_libdir/mysql/libndbclient.*
rm -f %{buildroot}/usr/share/mysql-test/mysql-test-run  
rm -f %{buildroot}/usr/share/mysql-test/mtr  
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
filelist msql2mysql mysql_convert_table_format mysql_find_rows mysql_setpermission mysql_tzinfo_to_sql mysql_zap mysqlaccess mysqlhotcopy perror replace >mysql-tools.files
filelist ndbd ndbmtd ndbd_redo_log_reader >mysql-ndb-storage.files
filelist ndb_mgmd >mysql-ndb-management.files
filelist ndb_{config,desc,error_reporter,mgm,print_backup_file,print_schema_file,print_sys_file,restore,select_all,select_count,show_tables,size.pl,test_platform,waiter} >mysql-ndb-tools.files
filelist ndb_{cpcd,delete_all,drop_index,drop_table} >mysql-ndb-extra.files
### files not installed by make install
# Create directory structure
DOCS=(COPYING README EXCEPTIONS-CLIENT %_sourcedir/README.debug %_sourcedir/README.SuSE)
DOCDIR=%buildroot%_defaultdocdir/%name
install -d -m 755 ${DOCDIR}
install -d -m 755 %buildroot/etc/logrotate.d
install -d -m 755 %buildroot/etc/init.d
install -m 644 _build/support-files/mysql-log-rotate %buildroot/etc/logrotate.d/mysql
%if 0%{?prefered} > 0
install -m 644 _buildlibmysqld/*.so* %buildroot%_libdir
ln -s libmysqld.so.0.0.1 %buildroot%_libdir/libmysqld.so.0
ln -s libmysqld.so.0.0.1 %buildroot%_libdir/libmysqld.so
%endif
install -m 660 _build/support-files/my-medium.cnf %buildroot/etc/my.cnf
install -m 640 scripts/mysqlaccess.conf %buildroot/etc/mysqlaccess.conf
install -m 644 "${DOCS[@]}" ${DOCDIR} || true
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
# XXX this should go under /var/..., but some tests get confused by a symlink
# var -> ../../../var/...
#ln -sf ../../../var/lib/mysql/mysql-test %buildroot/usr/share/mysql-test/var
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
cat libmysqlclient-devel.files | while read tmp; do
	cd '%buildroot'
	rm -f .$tmp
done
rm -rf '%buildroot'/%_libdir/libmysqld.*
rm -rf '%buildroot'/%_libdir/*.la
rm -rf '%buildroot'/%_libdir/*.so
rm -rf '%buildroot'/%_libdir/*.a
rm -rf '%buildroot'/%_includedir  
rm -rf '%buildroot'/%_datadir/man/*/mysql_config*  
rm -rf '%buildroot'/%_datadir/aclocal
%endif
sed -i 's|doc/packages/mysql|doc/packages/%{name}|g' '%buildroot'/etc/init.d/mysql
ln -s mysqlcheck '%buildroot'%_bindir/mysqlrepair
ln -s mysqlcheck '%buildroot'%_bindir/mysqlanalyze
ln -s mysqlcheck '%buildroot'%_bindir/mysqloptimize
if [ -f '%buildroot'/usr/bin/mysqlaccess.conf ]; then
	mv '%buildroot'/usr/bin/mysqlaccess.conf '%buildroot'/etc/mysqlaccess.conf
fi
mkdir -p '%buildroot'/etc/mysql
[ -z "`ls -1 %_sourcedir/*.cnf`" ] || cp %_sourcedir/*.cnf '%buildroot'/etc/mysql
