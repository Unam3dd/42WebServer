/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   constructors.cpp                                   :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: stales <stales@student.42angouleme.fr>     +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/03/05 15:58:42 by stales            #+#    #+#             */
/*   Updated: 2023/03/09 17:48:47 by stales           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "http_config.hpp"
#include "http_config_parser.hpp"

HttpConfigParser::HttpConfigParser(void): HttpConfig()
{
}

HttpConfigParser::HttpConfigParser(const HttpConfigParser& h): HttpConfig(h)
{
	(void)h;
}

HttpConfigParser::HttpConfigParser(const HttpConfig& c): HttpConfig(c)
{
	(void)c;
}

