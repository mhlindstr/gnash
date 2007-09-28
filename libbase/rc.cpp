// rc.cpp:  "Run Command" configuration file, for Gnash.
// 
//   Copyright (C) 2005, 2006, 2007 Free Software Foundation, Inc.
// 
// This program is free software; you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation; either version 3 of the License, or
// (at your option) any later version.
// 
// This program is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.
//
// You should have received a copy of the GNU General Public License
// along with this program; if not, write to the Free Software
// Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA  02110-1301  USA
//

// This is generated by autoconf
#ifdef HAVE_CONFIG_H
# include "config.h"
#endif

#ifdef HAVE_PWD_H
# include <pwd.h>
#endif

#include <sys/types.h>
#include <unistd.h> // for getuid()
#include <sys/stat.h>

#include <cctype>  // for toupper
#include <string>
#include <vector>
#include <iostream>
#include <fstream>

#include "tu_types.h"
#include "StringPredicates.h"
#include "log.h"
#include "rc.h"

#ifndef DEFAULT_STREAMS_TIMEOUT
// TODO: add a ./configure switch to set this
# define DEFAULT_STREAMS_TIMEOUT 10
#endif

using namespace std;
namespace gnash {

RcInitFile&
RcInitFile::getDefaultInstance()
{
	// TODO: allocate on the heap and provide a destroyDefaultInstance,
	//       for finer control of destruction order
	static RcInitFile rcfile;
	return rcfile;
}


RcInitFile::RcInitFile() : _delay(0),
                           _debug(false),
                           _debugger(false),
                           _verbosity(-1),
                           _flashVersionString("GSH "\
				DEFAULT_FLASH_MAJOR_VERSION","\
				DEFAULT_FLASH_MINOR_VERSION","\
				DEFAULT_FLASH_REV_NUMBER ",0"),
                           _actiondump(false),
                           _parserdump(false),
			   _verboseASCodingErrors(false),
			   _verboseMalformedSWF(false),
                           _splash_screen(true),
                           _localdomain_only(false),
                           _localhost_only(false),
                           _log("gnash-dbg.log"),
			   _writelog(false),
                           _sound(true),
                           _plugin_sound(true),
			   _extensionsEnabled(false),
			   _startStopped(false),
			   _streamsTimeout(DEFAULT_STREAMS_TIMEOUT)

{
//    GNASH_REPORT_FUNCTION;
    loadFiles();
}

RcInitFile::~RcInitFile()
{
//    GNASH_REPORT_FUNCTION;    
}

// Look for a config file in the likely places.
bool
RcInitFile::loadFiles()
{
//    GNASH_REPORT_FUNCTION;
    
    // Check the default system location
    string loadfile = "/etc/gnashrc";
    parseFile(loadfile);
    
    // Check the default config location
    loadfile = "/usr/local/etc/gnashrc";
    parseFile(loadfile);
    
    // Check the users home directory
    char *home = getenv("HOME");
    if (home) {
        loadfile = home;
        loadfile += "/.gnashrc";
        parseFile(loadfile);
    }

    // Check the GNASHRC environment variable
    char *gnashrc = getenv("GNASHRC");
    if (gnashrc) {
        loadfile = gnashrc;
        return parseFile(loadfile);
    }
    
    return false;
}

bool
RcInitFile::extractSetting(bool *var, const char *pattern,
                           std::string &variable, std::string &value)
{
//    GNASH_REPORT_FUNCTION;
    //log_msg ("%s: %s", variable, value);
    
	StringNoCaseEqual noCaseCompare;
    if ( noCaseCompare(variable, pattern) ) {
        if ( noCaseCompare(value, "on") || noCaseCompare(value, "yes") ||
             noCaseCompare(value, "true")) {
            //log_msg ("%s: Enabled", variable);
            *var = true;
        }
        if (noCaseCompare(value, "off") || noCaseCompare(value, "no") ||
            noCaseCompare(value, "false")) {
            //log_msg ("%s: Disabled", variable);
            *var = false;
        }
    }
    return *var;
}

int
RcInitFile::extractNumber(int *num, const char *pattern, string &variable,
                           string &value)
{      
//    GNASH_REPORT_FUNCTION;

    StringNoCaseEqual noCaseCompare;

//        log_msg ("%s: %s", variable.c_str(), value.c_str());
    if ( noCaseCompare(variable, pattern) ) {
        *num = strtol(value.c_str(), NULL, 0);
    }
    return *num;
}

void
RcInitFile::extractDouble(double& out, const char *pattern, string &variable,
                           string &value)
{
//    GNASH_REPORT_FUNCTION;

    StringNoCaseEqual noCaseCompare;

    // printf("%s: %s\n", variable.c_str(), value.c_str());

    if ( noCaseCompare(variable, pattern) ) {
        out = strtod(value.c_str(), 0);
	//printf("strtod returned %g\n", out);
    }
}

string
RcInitFile::expandPath (std::string _path)

{

//Returns what's passed to it on systems without
//POSIX tilde expansion, but is still called to prepare
//for other operations on the path string

#ifdef HAVE_PWD_H
//Don't build tilde expansion on systems without pwd.h

              //Only if path starts with "~"
             if (_path.substr(0,1) == "~") {
             const char *home = getenv("HOME");
                     if (_path.substr(1,1) == "/") {
                          // Initial "~" followed by "/"
                          if (home) {
                               // if HOME set in env, replace ~ with HOME
                               _path = _path.replace(0,1,home);
                          }

# ifdef HAVE_GETPWNAM
//Don't try this on systems without getpwnam

                          //HOME not set in env: try using pwd

                          else { 
                               struct passwd *password = getpwuid(getuid());
                               const char *pwdhome = password->pw_dir;
                               if (home) {
                                   _path = _path.replace(0,1,pwdhome);
                               }
                               //If all that fails, leave path alone
                          }
                     }

                     //Initial "~" is not followed by "/"
                     else {
                          const char *userhome = NULL;
                          string::size_type first_slash =
                              _path.find_first_of("/");
                          string user;
                          if (first_slash != string::npos) {
                              // everything between initial ~ and / 
                              user = _path.substr(1, first_slash - 1 );
                          } else user = _path.substr(1);

                          //find user using pwd    
                          struct passwd *password = getpwnam(user.c_str());
                          if (password) {
                              //User found
                              userhome = password->pw_dir;
                          }
                          if (userhome) {
                               string foundhome(userhome);
                               _path = _path.replace(0,first_slash,foundhome);
                          }
# endif
                      }
                 }
#endif

     return _path;
}

// Parse the config file and set the variables.
bool
RcInitFile::parseFile(const std::string& filespec)
{
//    GNASH_REPORT_FUNCTION;
    struct stat stats;
    string action;
    string variable;
    string value;
    ifstream in;
    
//  log_msg ("Seeing if %s exists", filespec);
    if (filespec.size() == 0) {
        return false;
    }
    
    if (stat(filespec.c_str(), &stats) == 0) {
        in.open(filespec.c_str());
        
        if (!in) {
            log_error(_("Couldn't open file: %s"), filespec.c_str());
            return false;
        }
        
        // Read in each line and parse it
        while (!in.eof()) {

	    // Make sure action is empty, otherwise the last loop (with no new
	    // data) keeps action, variable and value from the previous loop. This
	    // causes problems if set blacklist or set whitelist are last, because
	    // value is erased while parsing and the lists are thus deleted.
	    action.clear();

            // Get the first token
            in >> action;

            // Ignore comment lines
            if (action[0] == '#') {
                // suck up the rest of the line
                char name[128];
                in.getline(name, 128);
                continue;
            } 
            
	    // Get second token
            in >> variable;

	    // Read in rest of line for parsing.
            getline(in, value);

	    // Erase leading spaces.
            string::size_type position = value.find_first_not_of(' ');
            if(position != string::npos) value.erase(0, position);

            if (action == "set" || action == "append") {

                if (variable == "flashVersionString") {
                    _flashVersionString = value;
		                    continue;
                }

                if (variable == "debuglog") {
                    _log = expandPath (value);
                    continue;
                }

                if (variable == "documentroot") {
                    _wwwroot = value;
                    continue;
                }
                
                if (variable == "blacklist") {
		    // 'set' should override all previous blacklists
		    // else 'append' should add to the end.
		    if (action == "set") _blacklist.clear();

                    string::size_type pos;
                    //This is an ugly way to avoid breaking lists
                    //Lists will work if they worked before, but
                    //combining the two separators will not.
                    //The colon way must be removed before protocols
                    //(http://, https:// can be included in lists).
                    char separator;
                    if (value.find(':') != string::npos) separator = ':';
                    else separator = ' ';
                    while (value.size()) {
                        pos = value.find(separator, 0);
                        _blacklist.push_back(value.substr(0, pos));
                        value.erase(0, pos);
                        if (value.size()) value.erase(0, value.find_first_not_of(separator)); 
                    }
                    continue;
                }

                if (variable == "whitelist") {
		    if (action == "set") _whitelist.clear();
		    
                    string::size_type pos;
                    //This is an ugly way to avoid breaking lists
                    //Lists will work if they worked before, but
                    //combining the two separators will not.
                    //The colon way must be removed before protocols
                    //(http://, https:// can be included in lists).
                    char separator;
                    if (value.find(':') != string::npos) separator = ':';
                    else separator = ' ';
                    while (value.size()) {
                        pos = value.find(separator, 0);
                        _whitelist.push_back(value.substr(0, pos));
                        value.erase(0, pos);
                        if (value.size()) value.erase(0, value.find_first_not_of(separator)); 
                    }
                    continue;
                }

		if (action == "set") {
                     extractSetting(&_splash_screen, "splash_screen", variable,
                               value);
                     extractSetting(&_localhost_only, "localhost", variable,
                               value);
                     extractSetting(&_localdomain_only, "localdomain", variable,
                               value);
                     extractSetting(&_debugger, "debugger", variable, value);
                     extractSetting(&_actiondump, "actionDump", variable, value);
                     extractSetting(&_parserdump, "parserDump", variable, value);
                     extractSetting(&_writelog, "writelog", variable, value);
                     extractSetting(&_sound, "sound", variable, value);
                     extractSetting(&_plugin_sound, "pluginsound", variable, value);
                     extractSetting(&_verboseASCodingErrors,
                               "ASCodingErrorsVerbosity", variable, value);
                     extractSetting(&_verboseMalformedSWF, "MalformedSWFVerbosity",
                               variable, value);
                     extractSetting(&_extensionsEnabled, "EnableExtensions",
                               variable, value);
                     extractSetting(&_startStopped, "StartStopped", variable, value);

                     extractDouble(_streamsTimeout, "StreamsTimeout", variable, value);
                
                     extractNumber(&_delay, "delay", variable, value);
                     extractNumber(&_verbosity, "verbosity", variable, value);
		}
            }
        }

    } else {
        if (in) {
            in.close();
        }
        return false;
    }  
    
    if (in) {
        in.close();
    }

    return true;
}

// Write the changed settings to the config file
bool
RcInitFile::updateFile(const std::string& /* filespec */)
{
    cerr << __PRETTY_FUNCTION__ << "ERROR: unimplemented!" << endl;
    return false;
}

void
RcInitFile::useSplashScreen(bool value)
{
    _splash_screen = value;
}

void
RcInitFile::useLocalDomain(bool value)
{
    _localdomain_only = value;
}

void
RcInitFile::useLocalHost(bool value)
{
    _localhost_only = value;
}

void
RcInitFile::useActionDump(bool value)
{
//    GNASH_REPORT_FUNCTION;
    _actiondump = value;
    if (value) {
        _verbosity++;
    }
}

void
RcInitFile::showASCodingErrors(bool value)
{
//    GNASH_REPORT_FUNCTION;
    
    _verboseASCodingErrors = value;
    if (value) {
        _verbosity++;
    }
}

void
RcInitFile::useParserDump(bool value)
{
//    GNASH_REPORT_FUNCTION;
    
    _parserdump = value;
    if (value) {
        _verbosity++;
    }
}

void
RcInitFile::useWriteLog(bool value)
{
//    GNASH_REPORT_FUNCTION;
    
    _writelog = value;
}

void
RcInitFile::dump()
{
    cerr << endl << "Dump RcInitFile:" << endl;
    cerr << "\tTimer interupt delay value: " << _delay << endl;
    cerr << "\tFlash debugger: "
         << ((_debugger)?"enabled":"disabled") << endl;
    cerr << "\tVerbosity Level: " << _verbosity << endl;
    cerr << "\tDump ActionScript processing: "
         << ((_actiondump)?"enabled":"disabled") << endl;
    cerr << "\tDump parser info: "
         << ((_parserdump)?"enabled":"disabled") << endl;
    cerr << "\tActionScript coding errors verbosity: "
         << ((_verboseASCodingErrors)?"enabled":"disabled") << endl;
    cerr << "\tMalformed SWF verbosity: "
         << ((_verboseASCodingErrors)?"enabled":"disabled") << endl;
    cerr << "\tUse Splash Screen: "
         << ((_splash_screen)?"enabled":"disabled") << endl;
    cerr << "\tUse Local Domain Only: "
         << ((_localdomain_only)?"enabled":"disabled") << endl;
    cerr << "\tUse Localhost Only: "
         << ((_localhost_only)?"enabled":"disabled") << endl;
    cerr << "\tWrite Debug Log To Disk: "
         << ((_writelog)?"enabled":"disabled") << endl;
    cerr << "\tEnable sound: "
         << ((_sound)?"enabled":"disabled") << endl;
    cerr << "\tEnable Plugin sound: "
         << ((_plugin_sound)?"enabled":"disabled") << endl;
    cerr << "\tEnable Extensions: "
         << ((_extensionsEnabled)?"enabled":"disabled") << endl;
    if (_log.size()) {
        cerr << "\tDebug Log name is: " << _log << endl;
    }
    if (_flashVersionString.size()) {
        cerr << "\tFlash Version String is: " << _flashVersionString << endl;
    }
    
//     std::vector<std::string> _whitelist;
//     std::vector<std::string> _blacklist;
}


} // end of namespace gnash
