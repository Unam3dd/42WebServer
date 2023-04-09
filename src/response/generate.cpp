/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   generate.cpp                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: ldournoi <ldournoi@student.42angouleme.fr  +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/04/09 19:02:59 by ldournoi          #+#    #+#             */
/*   Updated: 2023/04/09 19:03:00 by ldournoi         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "http_response.hpp"

/* *
 * @brief: generate the full http response from the response object. at this 
 * point, the response object should have all the necessary information to
 * generate the response, populated by _prepareXXXresponse() functions.
 *
 * @param: none
 * @return: none
 *
 */
void HttpResponse::_generateResponse(void){
	time_t now = time(NULL);
	struct tm *t = gmtime(&now);
	char buf[0x100];
	strftime(buf, sizeof(buf), "%a, %d %b %Y %H:%M:%S %Z", t);

	this->_fullresponse += "HTTP/1.1 " + NumberToString(_status) + " " + get_http_status_msg(_status) + "\r\n";
	this->_fullresponse += "Server: WebServ " + std::string(VERSION) + "\r\n";
	this->_fullresponse += "Date: " + std::string(buf) + "\r\n";
	_contenttype != "" ? this->_fullresponse += "Content-Type: " + this->_contenttype + "\r\n" : "";
	_body.size() != 0  ? this->_fullresponse += "Content-Length: " + NumberToString(this->_body.size()) + "\r\n" : "";
	this->_fullresponse += "Connection: close\r\n";
	this->_fullresponse += "\r\n";
	this->_fullresponse += this->_body;
}
