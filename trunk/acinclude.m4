

# 
# Synopsis
# 
# AX_CHECK_GL
# 
# Description
# 
# Check for an OpenGL implementation. If GL is found, the required
# compiler and linker flags are included in the output variables
# "GL_CFLAGS" and "GL_LIBS", respectively. This macro adds the configure
# option "--with-apple-opengl-framework", which users can use to
# indicate that Apple's OpenGL framework should be used on Mac OS X. If
# Apple's OpenGL framework is used, the symbol
# "HAVE_APPLE_OPENGL_FRAMEWORK" is defined. If no GL implementation is
# found, "no_gl" is set to "yes".
# 
# Version
# 
# 1.6
# 
# Author
# 
# Braden McDaniel <braden@endoframe.com>
# 
# M4 Source Code
#
AC_DEFUN([AX_CHECK_GL],
[AC_REQUIRE([AC_PATH_X])dnl
#AC_REQUIRE([ACX_PTHREAD])dnl

#
# There isn't a reliable way to know we should use the Apple OpenGL framework
# without a configure option.  A Mac OS X user may have installed an
# alternative GL implementation (e.g., Mesa), which may or may not depend on X.
#
AC_ARG_WITH([apple-opengl-framework],
            [AC_HELP_STRING([--with-apple-opengl-framework],
                            [use Apple OpenGL framework (Mac OS X only)])])
if test "X$with_apple_opengl_framework" = "Xyes"; then
  AC_DEFINE([HAVE_APPLE_OPENGL_FRAMEWORK], [1],
            [Use the Apple OpenGL framework.])
  GL_LIBS="-framework OpenGL"
else
  GL_CFLAGS="${PTHREAD_CFLAGS}"
  GL_LIBS="${PTHREAD_LIBS} -lm"

  #
  # Use x_includes and x_libraries if they have been set (presumably by
  # AC_PATH_X).
  #
  if test "X$no_x" != "Xyes"; then
    if test -n "$x_includes"; then
      GL_CFLAGS="-I${x_includes} ${GL_CFLAGS}"
    fi
    if test -n "$x_libraries"; then
      GL_LIBS="-L${x_libraries} -lX11 ${GL_LIBS}"
    fi
  fi

  AC_LANG_PUSH(C)

  AC_CHECK_HEADERS([windows.h])

  AC_CACHE_CHECK([for OpenGL library], [ax_cv_check_gl_libgl],
  [ax_cv_check_gl_libgl="no"
  ax_save_CPPFLAGS="${CPPFLAGS}"
  CPPFLAGS="${GL_CFLAGS} ${CPPFLAGS}"
  ax_save_LIBS="${LIBS}"
  LIBS=""
  ax_check_libs="-lopengl32 -lGL"
    for ax_lib in ${ax_check_libs}; do
    if test "X$CC" = "Xcl"; then
      ax_try_lib=`echo $ax_lib | sed -e 's/^-l//' -e 's/$/.lib/'`
    else
      ax_try_lib="${ax_lib}"
    fi
    LIBS="${ax_try_lib} ${GL_LIBS} ${ax_save_LIBS}"
    AC_TRY_LINK([
# if HAVE_WINDOWS_H && defined(_WIN32)
#   include <windows.h>
# endif
# include <GL/gl.h>
],
    [glBegin(0)],
    [ax_cv_check_gl_libgl="${ax_try_lib}"; break])
  done
  LIBS=${ax_save_LIBS}
  CPPFLAGS=${ax_save_CPPFLAGS}])

  if test "X${ax_cv_check_gl_libgl}" = "Xno"; then
    no_gl="yes"
    GL_CFLAGS=""
    GL_LIBS=""
  else
    GL_LIBS="${ax_cv_check_gl_libgl} ${GL_LIBS}"
  fi
  AC_LANG_POP(C)
fi

AC_SUBST([GL_CFLAGS])
AC_SUBST([GL_LIBS])
])dnl



#
# Synopsis
# 
# AX_CHECK_GLU
# 
# Description
#
# Check for GLU. If GLU is found, the required preprocessor and linker
# flags are included in the output variables "GLU_CFLAGS" and
# "GLU_LIBS", respectively. This macro adds the configure option
# "--with-apple-opengl-framework", which users can use to indicate that
# Apple's OpenGL framework should be used on Mac OS X. If Apple's OpenGL
# framework is used, the symbol "HAVE_APPLE_OPENGL_FRAMEWORK" is
# defined. If no GLU implementation is found, "no_glu" is set to "yes".
# 
# Version
#
# 1.1
# 
# Author
# 
# Braden McDaniel <braden@endoframe.com>
#
AC_DEFUN([AX_CHECK_GLU],
[AC_REQUIRE([AX_CHECK_GL])dnl
GLU_CFLAGS="${GL_CFLAGS}"
if test "X${with_apple_opengl_framework}" != "Xyes"; then
  AC_CACHE_CHECK([for OpenGL Utility library], [ax_cv_check_glu_libglu],
  [ax_cv_check_glu_libglu="no"
  ax_save_CPPFLAGS="${CPPFLAGS}"
  CPPFLAGS="${GL_CFLAGS} ${CPPFLAGS}"
  ax_save_LIBS="${LIBS}"
  LIBS=""
  ax_check_libs="-lglu32 -lGLU"
  for ax_lib in ${ax_check_libs}; do
    if test "X$CC" = "Xcl"; then
      ax_try_lib=`echo $ax_lib | sed -e 's/^-l//' -e 's/$/.lib/'`
    else
      ax_try_lib="${ax_lib}"
    fi
    LIBS="${ax_try_lib} ${GL_LIBS} ${ax_save_LIBS}"
    #
    # libGLU typically links with libstdc++ on POSIX platforms. However,
    # setting the language to C++ means that test program source is named
    # "conftest.cc"; and Microsoft cl doesn't know what to do with such a
    # file.
    #
    if test "X$CXX" != "Xcl"; then
      AC_LANG_PUSH([C++])
    fi
    AC_TRY_LINK([
# if HAVE_WINDOWS_H && defined(_WIN32)
#   include <windows.h>
# endif
# include <GL/glu.h>
],
    [gluBeginCurve(0)],
    [ax_cv_check_glu_libglu="${ax_try_lib}"; break])
    if test "X$CXX" != "Xcl"; then
      AC_LANG_POP([C++])
    fi
  done
  LIBS=${ax_save_LIBS}
  CPPFLAGS=${ax_save_CPPFLAGS}])
  if test "X${ax_cv_check_glu_libglu}" = "Xno"; then
    no_glu="yes"
    GLU_CFLAGS=""
    GLU_LIBS=""
  else
    GLU_LIBS="${ax_cv_check_glu_libglu} ${GL_LIBS}"
  fi
fi
AC_SUBST([GLU_CFLAGS])
AC_SUBST([GLU_LIBS])
])



# Check target compiler
AC_DEFUN([SETUP_FOR_TARGET],
[case $target in
*-*-cygwin* | *-*-mingw* | *-*-pw32*)
	AC_SUBST(SHARED_FLAGS, "-shared -no-undefined -Xlinker --export-all-symbols")
	AC_SUBST(PLUGIN_FLAGS, "-shared -no-undefined -avoid-version")
	AC_SUBST(GL_LIBS, "-lopengl32 -lglu32")	
	AC_CHECK_TOOL(RC, windres)
        nt=true
;;
*-*-darwin*)
        AC_SUBST(SHARED_FLAGS, "-shared")
        AC_SUBST(PLUGIN_FLAGS, "-shared -avoid-version")
        AC_SUBST(GL_LIBS, "-lGL -lGLU")
        osx=true
;;
 *) dnl default to standard linux
	AC_SUBST(SHARED_FLAGS, "-shared")
	AC_SUBST(PLUGIN_FLAGS, "-shared -avoid-version")
	AC_SUBST(GL_LIBS, "-lGL -lGLU")
        linux=true
;;
esac
#dnl you must arrange for every AM_conditional to run every time configure runs
#AM_CONDITIONAL(NR_NT, test x$nt = xtrue)
#AM_CONDITIONAL(NR_LINUX, test x$linux = xtrue)
#AM_CONDITIONAL(NR_OSX,test x$osx = xtrue )
])




# Check for Endian support
AC_DEFUN([DETECT_ENDIAN],
[AC_TRY_RUN([
		int main()
		{
			short s = 1;
			short* ptr = &s;
			unsigned char c = *((char*)ptr);
			return c;
		}
	]
	,[AC_DEFINE(CONFIG_BIG_ENDIAN,,[Big endian machine])]
	,[AC_DEFINE(CONFIG_LITTLE_ENDIAN,,[Little endian machine])])
])
 

# Check for PIC support
AC_DEFUN([CHECK_PIC],
[
AC_MSG_CHECKING([whether -fPIC is needed])
    case $build in
        x86_64-*)
            CXXFLAGS="$CXXFLAGS -fPIC"
            echo "yes"
        ;;
        *)
            echo "no"
        ;;
    esac
])



AC_DEFUN([AX_BOOST_BASE],
[
AC_ARG_WITH([boost],
        AS_HELP_STRING([--with-boost@<:@=DIR@:>@], [use boost (default is No) - it is possible to specify the root directory for boost (optional)]),
        [
    if test "$withval" = "no"; then
                want_boost="no"
    elif test "$withval" = "yes"; then
        want_boost="yes"
        ac_boost_path=""
    else
            want_boost="yes"
        ac_boost_path="$withval"
        fi
    ],
    [want_boost="no"])

if test "x$want_boost" = "xyes"; then
        boost_lib_version_req=ifelse([$1], ,1.20.0,$1)
        boost_lib_version_req_shorten=`expr $boost_lib_version_req : '\([[0-9]]*\.[[0-9]]*\)'`
        boost_lib_version_req_major=`expr $boost_lib_version_req : '\([[0-9]]*\)'`
        boost_lib_version_req_minor=`expr $boost_lib_version_req : '[[0-9]]*\.\([[0-9]]*\)'`
        boost_lib_version_req_sub_minor=`expr $boost_lib_version_req : '[[0-9]]*\.[[0-9]]*\.\([[0-9]]*\)'`
        if test "x$boost_lib_version_req_sub_minor" = "x" ; then
                boost_lib_version_req_sub_minor="0"
        fi
        WANT_BOOST_VERSION=`expr $boost_lib_version_req_major \* 100000 \+  $boost_lib_version_req_minor \* 100 \+ $boost_lib_version_req_sub_minor`
        AC_MSG_CHECKING(for boostlib >= $boost_lib_version_req)
        succeeded=no

        dnl first we check the system location for boost libraries
        dnl this location ist chosen if boost libraries are installed with the --layout=system option
        dnl or if you install boost with RPM
        if test "$ac_boost_path" != ""; then
                BOOST_LDFLAGS="-L$ac_boost_path/lib"
                BOOST_CPPFLAGS="-I$ac_boost_path/include"
        else
                for ac_boost_path_tmp in /usr /usr/local /opt ; do
                        if test -d "$ac_boost_path_tmp/include/boost" && test -r "$ac_boost_path_tmp/include/boost"; then
                                BOOST_LDFLAGS="-L$ac_boost_path_tmp/lib"
                                BOOST_CPPFLAGS="-I$ac_boost_path_tmp/include"
                                break;
                        fi
                done
        fi

        CPPFLAGS_SAVED="$CPPFLAGS"
        CPPFLAGS="$CPPFLAGS $BOOST_CPPFLAGS"
        export CPPFLAGS

        LDFLAGS_SAVED="$LDFLAGS"
        LDFLAGS="$LDFLAGS $BOOST_LDFLAGS"
        export LDFLAGS

        AC_LANG_PUSH(C++)
        AC_COMPILE_IFELSE([AC_LANG_PROGRAM([[
        @%:@include <boost/version.hpp>
        ]], [[
#if BOOST_VERSION >= $WANT_BOOST_VERSION
        // Everything is okay
#else
#  error Boost version is too old
#endif
        ]])],[
        AC_MSG_RESULT(yes)
        succeeded=yes
        found_system=yes
        ],[
        ])
        AC_LANG_POP([C++])



        dnl if we found no boost with system layout we search for boost libraries
        dnl built and installed without the --layout=system option or for a staged(not installed) version
        if test "x$succeeded" != "xyes"; then
                _version=0
                if test "$ac_boost_path" != ""; then
                        BOOST_LDFLAGS="-L$ac_boost_path/lib"
                        if test -d "$ac_boost_path" && test -r "$ac_boost_path"; then
                                for i in `ls -d $ac_boost_path/include/boost-* 2>/dev/null`; do
                                        _version_tmp=`echo $i | sed "s#$ac_boost_path##" | sed 's/\/include\/boost-//' | sed 's/_/./'`
                                        V_CHECK=`expr $_version_tmp \> $_version`
                                        if test "$V_CHECK" = "1" ; then
                                                _version=$_version_tmp
                                        fi
                                        VERSION_UNDERSCORE=`echo $_version | sed 's/\./_/'`
                                        BOOST_CPPFLAGS="-I$ac_boost_path/include/boost-$VERSION_UNDERSCORE"
                                done
                        fi
                else
                        for ac_boost_path in /usr /usr/local /opt ; do
                                if test -d "$ac_boost_path" && test -r "$ac_boost_path"; then
                                        for i in `ls -d $ac_boost_path/include/boost-* 2>/dev/null`; do
                                                _version_tmp=`echo $i | sed "s#$ac_boost_path##" | sed 's/\/include\/boost-//' | sed 's/_/./'`
                                                V_CHECK=`expr $_version_tmp \> $_version`
                                                if test "$V_CHECK" = "1" ; then
                                                        _version=$_version_tmp
                                                        best_path=$ac_boost_path
                                                fi
                                        done
                                fi
                        done

                        VERSION_UNDERSCORE=`echo $_version | sed 's/\./_/'`
                        BOOST_CPPFLAGS="-I$best_path/include/boost-$VERSION_UNDERSCORE"
                        BOOST_LDFLAGS="-L$best_path/lib"

                        if test "x$BOOST_ROOT" != "x"; then
                                if test -d "$BOOST_ROOT" && test -r "$BOOST_ROOT" && test -d "$BOOST_ROOT/stage/lib" && test -r "$BOOST_ROOT/stage/lib"; then
                                        version_dir=`expr //$BOOST_ROOT : '.*/\(.*\)'`
                                        stage_version=`echo $version_dir | sed 's/boost_//' | sed 's/_/./g'`
                                        stage_version_shorten=`expr $stage_version : '\([[0-9]]*\.[[0-9]]*\)'`
                                        V_CHECK=`expr $stage_version_shorten \>\= $_version`
                                        if test "$V_CHECK" = "1" ; then
                                                AC_MSG_NOTICE(We will use a staged boost library from $BOOST_ROOT)
                                                BOOST_CPPFLAGS="-I$BOOST_ROOT"
                                                BOOST_LDFLAGS="-L$BOOST_ROOT/stage/lib"
                                        fi
                                fi
                        fi
                fi

                CPPFLAGS="$CPPFLAGS $BOOST_CPPFLAGS"
                export CPPFLAGS
                LDFLAGS="$LDFLAGS $BOOST_LDFLAGS"
                export LDFLAGS

                AC_LANG_PUSH(C++)
                AC_COMPILE_IFELSE([AC_LANG_PROGRAM([[
                @%:@include <boost/version.hpp>
                ]], [[
#if BOOST_VERSION >= $WANT_BOOST_VERSION
                // Everything is okay
#else
#  error Boost version is too old
#endif
                ]])],[
                AC_MSG_RESULT(yes)
                succeeded=yes
                found_system=yes
                ],[
                ])
                AC_LANG_POP([C++])
        fi

        if test "$succeeded" != "yes" ; then
                if test "$_version" = "0" ; then
                        AC_MSG_ERROR([[We could not detect the boost libraries (version $boost_lib_version_req_shorten or higher). If you have a staged boost library (still not installed) please specify \$BOOST_ROOT in your environment and do not give a PATH to --with-boost option.  If you are sure you have boost installed, then check your version number looking in <boost/version.hpp>. See http://randspringer.de/boost for more documentation.]])
                else
                        AC_MSG_NOTICE([Your boost libraries seems to old (version $_version).])
                fi
        else
                AC_SUBST(BOOST_CPPFLAGS)
                AC_SUBST(BOOST_LDFLAGS)
                AC_DEFINE(HAVE_BOOST,,[define if the Boost library is available])
        fi

        CPPFLAGS="$CPPFLAGS_SAVED"
        LDFLAGS="$LDFLAGS_SAVED"
fi

])

# Check for boost_signals library
AC_DEFUN([AX_BOOST_SIGNALS],
[
        AC_ARG_WITH([boost-signals],
        AS_HELP_STRING([--with-boost-signals@<:@=special-lib@:>@],
                   [use the Signals library from boost - it is possible to specify a certain library for the linker
                        e.g. --with-boost-signals=boost_signals-gcc-mt-d ]),
        [
        if test "$withval" = "no"; then
                        want_boost="no"
        elif test "$withval" = "yes"; then
            want_boost="yes"
            ax_boost_user_signals_lib=""
        else
                    want_boost="yes"
                ax_boost_user_signals_lib="$withval"
                fi
        ],
        [want_boost="no"]
        )

        if test "x$want_boost" = "xyes"; then
        AC_REQUIRE([AC_PROG_CC])
                CPPFLAGS_SAVED="$CPPFLAGS"
                CPPFLAGS="$CPPFLAGS $BOOST_CPPFLAGS"
                export CPPFLAGS

                LDFLAGS_SAVED="$LDFLAGS"
                LDFLAGS="$LDFLAGS $BOOST_LDFLAGS"
                export LDFLAGS

        AC_CACHE_CHECK(whether the Boost::Signals library is available,
                                           ax_cv_boost_signals,
        [AC_LANG_PUSH([C++])
                 AC_COMPILE_IFELSE(AC_LANG_PROGRAM([[@%:@include <boost/signal.hpp>
                                                                                        ]],
                                  [[boost::signal<void ()> sig;
                                    return 0;
                                  ]]),
                           ax_cv_boost_signals=yes, ax_cv_boost_signals=no)
         AC_LANG_POP([C++])
                ])
                if test "x$ax_cv_boost_signals" = "xyes"; then
                        AC_DEFINE(HAVE_BOOST_SIGNALS,,[define if the Boost::Signals library is available])
                        BN=boost_signals
            if test "x$ax_boost_user_signals_lib" = "x"; then
                                for ax_lib in $BN $BN-$CC $BN-$CC-mt $BN-$CC-mt-s $BN-$CC-s \
                              lib$BN lib$BN-$CC lib$BN-$CC-mt lib$BN-$CC-mt-s lib$BN-$CC-s \
                              $BN-mgw $BN-mgw $BN-mgw-mt $BN-mgw-mt-s $BN-mgw-s ; do
                                    AC_CHECK_LIB($ax_lib, main, [BOOST_SIGNALS_LIB="-l$ax_lib" AC_SUBST(BOOST_SIGNALS_LIB) link_signals="yes" break],
                                 [link_signals="no"])
                                done
            else
               for ax_lib in $ax_boost_user_signals_lib $BN-$ax_boost_user_signals_lib; do
                                      AC_CHECK_LIB($ax_lib, main,
                                   [BOOST_SIGNALS_LIB="-l$ax_lib" AC_SUBST(BOOST_SIGNALS_LIB) link_signals="yes" break],
                                   [link_signals="no"])
                  done

            fi
                        if test "x$link_signals" = "xno"; then
                                AC_MSG_ERROR(Could not link against $ax_lib !)
                        fi
                fi

                CPPFLAGS="$CPPFLAGS_SAVED"
        LDFLAGS="$LDFLAGS_SAVED"
        fi
])