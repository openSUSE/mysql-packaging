%if 0%{?suse_version} < 1120 && 0%{?suse_version} > 0
%define socketpath /var/lib/mysql
%else
%define socketpath /var/run/mysql
%endif
%if 0%{?suse_version} > 1140
export EXTRA_FLAGS=" -Wno-unused-but-set-variable -fno-strict-aliasing -Wno-unused-parameter "
%endif
%ifarch ppc64
export EXTRA_FLAGS=" -mminimal-toc "
%endif
export CFLAGS="$RPM_OPT_FLAGS -DPIC -fPIC -DFORCE_INIT_OF_VARS $EXTRA_FLAGS "
export CXXFLAGS="$CFLAGS -fno-exceptions -fno-rtti"

%if 0%{use_cmake} < 1
autoreconf -f -i
# Make the compile a function, since we do multiple builds
BuildMySQL() {
	# The --enable-assembler simply does nothing on systems that do not
	# support assembler speedups.
	suffix="$1"; shift
	mkdir _build$suffix
	pushd _build$suffix
	../configure \
		--enable-assembler \
		--with-readline \
		--infodir=%{_infodir} \
		--libdir=%{_libdir} \
		--libexecdir=/usr/sbin \
		--localstatedir=/var/lib/mysql \
		--mandir=%{_mandir} \
		--prefix=%{_prefix} \
		--sysconfdir=/etc \
		--with-mysqld-user=mysql \
		--with-mysqld-group=mysql \
		--without-debug \
		--datadir=/usr/share \
		--includedir=/usr/include \
		--with-charset=utf8 \
		--with-collation=utf8_unicode_ci \
		--with-extra-charsets=all \
		--with-unix-socket-path=%{socketpath}/mysql.sock \
		--enable-thread-safe-client \
		--with-comment="SUSE MySQL RPM" \
		--with-libwrap \
		--with-server-suffix=$suffix \
%ifarch s390 s390x
		--with-atomic-ops=rwlocks \
%else
		--with-atomic-ops=smp \
%endif
		--with-ssl=/usr \
		--with-plugins=%{builtin_plugins} \
		--without-plugin-handlersocket \
		HOSTNAME="/bin/hostname -f" \
		"$@"
	# Add this for MyISAM RAID support:
	# --with-raid
	# benchdir does not fit in above model. Fix when we make a separate package
	make %{?jobs:-j%jobs}
	nm --numeric-sort sql/mysqld > sql/mysqld.sym
	# needed for make check, which is not VPATH aware
	# copy the files unconditionally so that manual 'make check' after
	# the build works, too
	cp -a ../mysql-test/* mysql-test/
	for i in ../sql/share/{english,charsets}; do
		[ -d $i ] && cp -a $i sql/share/
	done
	cp -a ../scripts/*.sql scripts/
%if %{?_with_testsuite:1}0
	pushd mysql-test; perl ./mysql-test-run.pl --force; popd
%endif
	popd
}
# Build the client, extra tools, libraries
%if 0%{?prefered} > 0
BuildMySQL "" --with-embedded-server
%else
BuildMySQL "" --without-embedded-server
%endif
# build mysqld-debug
BuildMySQL "-debug" --with-debug --without-embedded-server

%if 0%{?prefered} > 0
mkdir -p _buildlibmysqld/tmp
cd _buildlibmysqld/tmp
ar -x ../../_build/libmysqld/libmysqld.a
gcc $CFLAGS $LDFLAGS -shared -Wl,-soname,libmysqld.so.0 -o ../libmysqld.so.0.0.1 \
*.o \
-lpthread -lcrypt -lnsl -lssl -lcrypto -lz -lrt -lstdc++ -lm -lc
cd ..
rm -rf tmp
cd ..
%endif

%else # cmake build

BuildMySQL() {
	mkdir _build$1
	pushd _build$1
	shift
	cmake -DWITH_SSL=system                                            \
			-DWITH_ZLIB=system                                           \
			-DWITH_READLINE=0                                            \
			-DWITH_LIBEDIT=0                                             \
			-DINSTALL_LAYOUT=RPM                                         \
			-DMYSQL_UNIX_ADDR=%{socketpath}/mysql.sock                   \
			-DINSTALL_MYSQLSHAREDIR=share/%{name}                        \
			-DWITH_COMMENT="openSUSE MySQL rpm"                          \
			-DWITH_EXTRA_CHARSET=all                                     \
			-DDEFAULT_CHARSET=utf8  -DDEFAULT_COLLATION=utf8_general_ci  \
			-DWITH_INNOBASE_STORAGE_ENGINE=1                             \
			-DWITH_PERFSCHEMA_STORAGE_ENGINE=1                           \
			-DWITH_EMBEDDED_SERVER=true                                  \
			-DCOMPILATION_COMMENT="openSUSE package"                     \
			-DDENABLE_DOWNLOADS=false                                    \
			-DINSTALL_PLUGINDIR_RPM="%{_libdir}/mysql/plugin"            \
			-DINSTALL_LIBDIR_RPM="%{_libdir}"                            \
			"$@" ..
	make %{?jobs:-j%jobs}
	nm --numeric-sort sql/mysqld > sql/mysqld.sym
	popd
}

BuildMySQL "" -DCMAKE_BUILD_TYPE=Release -DINSTALL_SQLBENCHDIR=share
BuildMySQL "-debug" -DCMAKE_BUILD_TYPE=Debug
%endif
