EXTRA_FLAGS="-Wno-unused-but-set-variable -fno-strict-aliasing -Wno-unused-parameter"
# Mariadb devs seems to fall in love with -Werror option
EXTRA_FLAGS="${EXTRA_FLAGS} -Wno-error"
%ifarch ppc64
EXTRA_FLAGS="${EXTRA_FLAGS} -mminimal-toc"
%endif
export CFLAGS="%{optflags} -DOPENSSL_LOAD_CONF -DPIC -fPIC -DFORCE_INIT_OF_VARS $EXTRA_FLAGS"
export CXXFLAGS="$CFLAGS -felide-constructors"
%cmake -DWITH_SSL=system                                             \
        -DWITH_ZLIB=system                                           \
        -DWITH_LIBEVENT=system                                       \
        -DWITH_JEMALLOC=auto                                         \
        -DWITH_READLINE=0                                            \
        -DWITH_LIBEDIT=0                                             \
        -DINSTALL_LAYOUT=RPM                                         \
        -DMYSQL_UNIX_ADDR="%{_localstatedir}/run/mysql/mysql.sock"      \
        -DINSTALL_UNIX_ADDRDIR="%{_localstatedir}/run/mysql/mysql.sock" \
        -DINSTALL_MYSQLSHAREDIR=share/%{name}                        \
        -DWITH_COMMENT="openSUSE MySQL rpm"                          \
        -DWITH_EXTRA_CHARSET=all                                     \
        -DDEFAULT_CHARSET=utf8  -DDEFAULT_COLLATION=utf8_general_ci  \
        -DWITH_INNOBASE_STORAGE_ENGINE=1                             \
        -DWITH_PERFSCHEMA_STORAGE_ENGINE=1                           \
%if %{with oqgraph}
        -DWITH_OQGRAPH_STORAGE_ENGINE=1                              \
%endif
%if %{with cassandra}
        -DWITH_CASSANDRA_STORAGE_ENGINE=1                            \
%endif
        -DWITH_XTRADB_STORAGE_ENGINE=1                               \
        -DWITH_CSV_STORAGE_ENGINE=1                                  \
        -DWITH_HANDLERSOCKET_STORAGE_ENGINE=1                        \
        -DWITH_EMBEDDED_SERVER=true                                  \
        -DCOMPILATION_COMMENT="openSUSE package"                     \
        -DDENABLE_DOWNLOADS=false                                    \
        -DINSTALL_PLUGINDIR_RPM="%{_lib}/mysql/plugin"               \
        -DINSTALL_LIBDIR_RPM="%{_lib}"                               \
        -DINSTALL_SYSCONF2DIR="%{_sysconfdir}/my.cnf.d"              \
        -DCMAKE_C_FLAGS_RELWITHDEBINFO="$CFLAGS"                     \
        -DCMAKE_CXX_FLAGS_RELWITHDEBINFO="$CXXFLAGS"                 \
        -DCMAKE_BUILD_TYPE=RelWithDebInfo -DINSTALL_SQLBENCHDIR=share \
        -DCMAKE_C_FLAGS="$CFLAGS"                                    \
        -DCMAKE_CXX_FLAGS="$CXXFLAGS"                                \
        -DCMAKE_EXE_LINKER_FLAGS="-Wl,--as-needed -pie -Wl,-z,relro,-z,now" \
        -DCMAKE_MODULE_LINKER_FLAGS="-Wl,--as-needed -pie -Wl,-z,relro,-z,now" \
        -DCMAKE_SHARED_LINKER_FLAGS="-Wl,--as-needed -pie -Wl,-z,relro,-z,now" \
        -Wno-dev "$@" ..
make %{?_smp_mflags}
nm --numeric-sort sql/mysqld > sql/mysqld.sym
