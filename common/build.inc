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
export CFLAGS="$RPM_OPT_FLAGS -DOPENSSL_LOAD_CONF -DPIC -fPIC -DFORCE_INIT_OF_VARS $EXTRA_FLAGS "
export CXXFLAGS="$CFLAGS"

BuildMySQL() {
    mkdir _build$1
    pushd _build$1
    shift
    cmake -DWITH_SSL=system                                              \
            -DWITH_ZLIB=system                                           \
            -DWITH_LIBEVENT=system                                       \
            -DWITH_JEMALLOC=no                                           \
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
            -DINSTALL_PLUGINDIR_RPM="%{_lib}/mysql/plugin"               \
            -DINSTALL_LIBDIR_RPM="%{_lib}"                               \
            -Wno-dev "$@" ..
    make %{?jobs:-j%jobs}
    nm --numeric-sort sql/mysqld > sql/mysqld.sym
    popd
}

BuildMySQL "" -DCMAKE_BUILD_TYPE=Release -DINSTALL_SQLBENCHDIR=share
# TokuDB can't be compiled without optimisations turned on
BuildMySQL "-debug" -DCMAKE_BUILD_TYPE=Debug -DWITHOUT_TOKUDB=1
