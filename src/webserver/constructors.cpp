/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   constructors.cpp                                   :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: stales <stales@student.42angouleme.fr>     +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/03/15 11:31:17 by stales            #+#    #+#             */
/*   Updated: 2023/05/26 22:44:36 by ldournoi         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "webserver.hpp"
#include "http_utils.hpp"

WebServer::WebServer(void)
{
	_srvBlk = false;
	_locBlk = false;
	_nserv = 0;
	_error[ERRPARSE_OK] = "No error";
	_error[ERRPARSE_UNKNOWN] = "Unknown config directive";
	_error[ERRPARSE_NEWSRVBLK] = "New Server block is illegal here";
	_error[ERRPARSE_NEWLOCBLK] = "New Location block is illegal here";
	_error[ERRPARSE_ENDBLK] = "End of block is illegal here";
	_error[ERRPARSE_SINGLEARG] = "Directive with only one argument";
	_error[ERRPARSE_NOARG] = "Directive with no argument";
	_error[ERRPARSE_PORT] = "Port is illegal value";
	_error[ERRPARSE_ERRPAGE] = "Error page is illegal value";
	_error[ERRPARSE_SPCORTAB] = "Arg is only whitespaces";
	_error[ERRPARSE_DUP] = "Duplicate directive";
	_error[ERRPARSE_POST] = "Post directive is over max value";
	_error[ERRPARSE_TIMEOUT] = "Timeout directive is over max value";
	_error[ERRPARSE_COOKIES] = "Illegal arg for cookies. Must be 'on' or 'off'";
	_error[ERRPARSE_UPLOADS] = "Illegal arg for uploads. Must be 'on' or 'off'";
	_error[ERRPARSE_DIRLIST] = "Illegal arg for dirlist. Must be 'on' or 'off'";
	_error[ERRPARSE_CGI] = "Error parsing CGI directive";
	_error[ERRPARSE_METHODS] = "Error parsing methods directive (only GET, POST, PUT, DELETE)";
	_error[ERRPARSE_UPLOADDIR] = "Error parsing upload dir directive";
	_error[ERRPARSE_NBARGS] = "Wrong number of arguments";
	_error[ERRPARSE_NOSRVNAME] = "No server name";
	_error[ERRPARSE_NOPORT] = "No port";
}

WebServer::WebServer(const std::string& path): _cfgfile(path.c_str(), O_RDONLY, S_IRUSR)
{
	if (path.empty() || !this->_cfgfile.size()) return ;
	_srvBlk = false;
	_locBlk = false;
	_nserv = 0;
	_error[ERRPARSE_OK] = "No error";
	_error[ERRPARSE_UNKNOWN] = "Unknown config directive";
	_error[ERRPARSE_NEWSRVBLK] = "New Server block is illegal here";
	_error[ERRPARSE_NEWLOCBLK] = "New Location block is illegal here";
	_error[ERRPARSE_ENDBLK] = "End of block is illegal here";
	_error[ERRPARSE_SINGLEARG] = "Directive with only one argument";
	_error[ERRPARSE_NOARG] = "Directive with no argument";
	_error[ERRPARSE_PORT] = "Port is illegal value";
	_error[ERRPARSE_ERRPAGE] = "Error page is illegal value";
	_error[ERRPARSE_SPCORTAB] = "Arg is only whitespaces";
	_error[ERRPARSE_DUP] = "Duplicate directive";
	_error[ERRPARSE_POST] = "Post directive is over max value";
	_error[ERRPARSE_TIMEOUT] = "Timeout directive is over max value";
	_error[ERRPARSE_COOKIES] = "Illegal arg for cookies. Must be 'on' or 'off'";
	_error[ERRPARSE_UPLOADS] = "Illegal arg for uploads. Must be 'on' or 'off'";
	_error[ERRPARSE_DIRLIST] = "Illegal arg for dirlist. Must be 'on' or 'off'";
	_error[ERRPARSE_CGI] = "Error parsing CGI directive";
	_error[ERRPARSE_METHODS] = "Error parsing methods directive (only GET, POST, PUT, DELETE)";
	_error[ERRPARSE_UPLOADDIR] = "Error parsing upload dir directive";
	_error[ERRPARSE_NBARGS] = "Wrong number of arguments";
	_error[ERRPARSE_NOSRVNAME] = "No server name";
	_error[ERRPARSE_NOPORT] = "No port";
	_error[ERRPARSE_HTTPCODE] = "Wrong http code, please read the RFC";
}

WebServer::WebServer(const WebServer& ws): _cfgfile(ws._cfgfile)
{
	if (this == &ws) return ;
	_srvBlk = false;
	_locBlk = false;
	_nserv = 0;
	_error[ERRPARSE_OK] = "No error";
	_error[ERRPARSE_UNKNOWN] = "Unknown config directive";
	_error[ERRPARSE_NEWSRVBLK] = "New Server block is illegal here";
	_error[ERRPARSE_NEWLOCBLK] = "New Location block is illegal here";
	_error[ERRPARSE_ENDBLK] = "End of block is illegal here";
	_error[ERRPARSE_SINGLEARG] = "Directive with only one argument";
	_error[ERRPARSE_NOARG] = "Directive with no argument";
	_error[ERRPARSE_PORT] = "Port is illegal value";
	_error[ERRPARSE_ERRPAGE] = "Error page is illegal value";
	_error[ERRPARSE_SPCORTAB] = "Arg is only whitespaces";
	_error[ERRPARSE_DUP] = "Duplicate directive";
	_error[ERRPARSE_POST] = "Post directive is over max value";
	_error[ERRPARSE_TIMEOUT] = "Timeout directive is over max value";
	_error[ERRPARSE_COOKIES] = "Illegal arg for cookies. Must be 'on' or 'off'";
	_error[ERRPARSE_UPLOADS] = "Illegal arg for uploads. Must be 'on' or 'off'";
	_error[ERRPARSE_DIRLIST] = "Illegal arg for dirlist. Must be 'on' or 'off'";
	_error[ERRPARSE_CGI] = "Error parsing CGI directive";
	_error[ERRPARSE_METHODS] = "Error parsing methods directive (only GET, POST, PUT, DELETE)";
	_error[ERRPARSE_UPLOADDIR] = "Error parsing upload dir directive";
	_error[ERRPARSE_NBARGS] = "Wrong number of arguments";
	_error[ERRPARSE_NOSRVNAME] = "No server name";
	_error[ERRPARSE_NOPORT] = "No port";

}

