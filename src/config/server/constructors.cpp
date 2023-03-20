/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   constructors.cpp                                   :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: stales <stales@student.42angouleme.fr>     +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/03/13 18:48:26 by stales            #+#    #+#             */
/*   Updated: 2023/03/20 01:11:49 by ldournoi         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "file.hpp"
#include "http_config.hpp"

HttpServerConfig::HttpServerConfig(void)
{
	_parseMap["server_name"] = &HttpServerConfig::SetServerNames;
	_parseMap["server_names"] = &HttpServerConfig::SetServerNames;
	_parseMap["listen"] = &HttpServerConfig::SetServerPorts;
	_parseMap["index"] = &HttpServerConfig::SetIndexes;
	_parseMap["errpage"] = &HttpServerConfig::SetErrorPages;
	_parseMap["docroot"] = &HttpServerConfig::SetRoot;
	_parseMap["session_path"] = &HttpServerConfig::SetSessionPath;
	_parseMap["timeout"] = &HttpServerConfig::SetServerTimeout;
	_parseMap["max_post_size"] = &HttpServerConfig::SetMaxPostSize;
	_parseMap["cookies"] = &HttpServerConfig::SetCookies;
	_parseMap["uploads"] = &HttpServerConfig::SetUploads;
	_parseMap["upload_dir"] = &HttpServerConfig::SetUploadDir;
	_parseMap["dirlist"] = &HttpServerConfig::SetDirList;
	_parseMap["cgi"] = &HttpServerConfig::SetCgi;
	_parseMap["methods"] = &HttpServerConfig::SetMethods;

	_root = "/var/www/html";
	_methods = static_cast<methods_t>(GET | POST | PUT | DELETE);
	_max_size_post = 1000000;
	_sessionpath = "/tmp";
	_timeout = 300;
	_cookies = true;
	_uploads = true;
	_uploaddir = "/tmp";
	_dirlist = true;
	_errpages[E400].code = 400;
	_errpages[E400].path = "default";
	_errpages[E401].code = 401;
	_errpages[E401].path = "default";
	_errpages[E403].code = 403;
	_errpages[E403].path = "default";
	_errpages[E404].code = 404;
	_errpages[E404].path = "default";
	_errpages[E405].code = 405;
	_errpages[E405].path = "default";
	_errpages[E500].code = 500;
	_errpages[E500].path = "default";
	_errpages[E501].code = 501;
	_errpages[E501].path = "default";
	_errpages[E505].code = 505;
	_errpages[E505].path = "default";
}

HttpServerConfig::HttpServerConfig(const std::string& path)
{
	(void)path;
}

HttpServerConfig::HttpServerConfig(const HttpServerConfig& c)
{
	if (this == &c) return ;

	*this = c;
}

