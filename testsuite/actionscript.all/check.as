// 
//   Copyright (C) 2005, 2006 Free Software Foundation, Inc.
//
// This program is free software; you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation; either version 2 of the License, or
// (at your option) any later version.

// This program is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.

// You should have received a copy of the GNU General Public License

// along with this program; if not, write to the Free Software
// Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA  02110-1301  USA

//

#ifndef _CHECK_AS_
#define _CHECK_AS_

// ONLINE mode uses DEJAGNU module
#ifdef ONLINE
# define USE_DEJAGNU_MODULE
#endif

// Use facilities provided by dejagnu shared library module
//
// NOTE: if you define USE_DEJAGNU_MODULE you
//       will also need put dejagnu_so_init.as in first frame
//       and put dejagnu_so_fini.as in last frame.
#ifdef USE_DEJAGNU_MODULE

# define trace info

#else // ndef USE_DEJAGNU_MODULE

#define pass_check(text) trace("PASSED: "+text)
#define xpass_check(text) trace("XPASSED: "+text)
#define fail_check(text) trace("FAILED: "+text)
#define xfail_check(text) trace("XFAILED: "+text)
#define info(x) trace(x)


#endif

//
// Use check(<expression>)
//
#define check(expr)  \
	if ( expr ) pass_check(#expr + \
		" [" + __LINE__ + "]" ); \
	else fail_check(#expr + \
		" [" + __LINE__ + "]" ); \

#define xcheck(expr)  \
        if ( expr ) xpass_check(#expr + \
                " [" + __LINE__ + "]" ); \
        else xfail_check(#expr + \
                " [" + __LINE__ + "]" ); \

//
// Use check_equals(<obtained>, <expected>)
//
#define check_equals(obt, exp)  \
	if ( obt == exp ) pass_check( \
		#obt + " == " + #exp + \
		" [" + __LINE__ + "]" ); \
	else fail_check("expected: " + #exp + \
		" obtained: " + obt + \
		" [" + __LINE__ + "]" ); \

#define xcheck_equals(obt, exp)  \
        if ( obt == exp ) xpass_check( \
                #obt + " == " + #exp + \
                " [" + __LINE__ + "]" ); \
        else xfail_check("expected: " + #exp + \
                " obtained: " + obt + \
                " [" + __LINE__ + "]" ); \

info('['+rcsid+']');

// If using the DEJAGNU_MODULE this info will be printed
// by code in dejagnu_so_init.as
#ifndef USE_DEJAGNU_MODULE
info("SWF" + OUTPUT_VERSION + " - " + System.capabilities.version + "\n");
#endif

#endif // _CHECK_AS_
