/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   wait.cpp                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: ldournoi <ldournoi@student.42angouleme.fr  +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/04/12 15:10:21 by ldournoi          #+#    #+#             */
/*   Updated: 2023/05/24 16:39:46 by stales           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "http_request.hpp"
#include "http_server.hpp"
#include "http_status.hpp"
#include "http_utils.hpp"
#include "http_colors.hpp"
#include "http_response.hpp"
#include "webserver.hpp"
#include <asm-generic/ioctls.h>
#include <cstdio>
#include <new>
#include <signals.hpp>
#include <streambuf>
#include <sys/epoll.h>
#include <unistd.h>
#include <iostream>
#include <cstring>
#include <fstream>
#include <sys/ioctl.h>
#include <sys/timerfd.h>
#include <string>

static unsigned long long	lowest_heap_address(void)
{
	std::stringstream	ss;
	std::ifstream		regions("/proc/self/maps");
	std::string			line;
	std::string			addr;
	unsigned long long	ret = 0;

	while (std::getline(regions, line))
	{
		if (line.find("[heap]") != std::string::npos)
		{
			addr = line.substr(0, line.find("-"));
			ss << std::hex << addr;
			ss >> ret;
			break;
		}
	}
	return (!ret ? 0xFFFFFF : ret);
}

t_status	WebServer::_acceptClient(ev_t *e)
{
	Socket	*client = NULL;
	ev_t	tev;
	int		tfd = 0;

	if (!e) return (STATUS_FAIL);

	struct itimerspec its;
	its.it_value.tv_sec = 10;
	its.it_value.tv_nsec = 0;
	its.it_interval.tv_sec = 0;
	its.it_interval.tv_nsec = 0;

	FOREACH_VECTOR(HttpServer*, this->_srv, srv)
	{
		if (e->data.fd == (*srv)->getSocket().Getfd())
		{
			client = (*srv)->getSocket().Accept();
			if (client) 
			{
				client->SetSrvPort((*srv)->getPort());
				(*srv)->getClients().push_back(client);

				tev.data.ptr = client;
				tev.events = EPOLLIN;
				fcntl(client->Getfd(), F_SETFL, fcntl(client->Getfd(), F_GETFL, 0) | O_NONBLOCK);
				this->_epoll.Ctl(EPOLL_CTL_ADD, client->Getfd(), &tev);

				tfd = timerfd_create(CLOCK_MONOTONIC, 0);
				timerfd_settime(tfd, 0, &its, NULL);
				this->_timerfds.insert(std::pair<int, int>(client->Getfd(), tfd));
				tev.events = EPOLLIN | EPOLLET;
				tev.data.fd = tfd;
				this->_epoll.Ctl(EPOLL_CTL_ADD, tfd, &tev);
				client->SetTfd(tfd);

				_clients.push_back(client);
				std::cout << SUCCESS << "[WebServer::Wait] New client Accepted ! " << client->InetNtoa(client->GetSin()->sin_addr.s_addr) << ":" << client->Ntohs(client->GetSin()->sin_port) << std::endl;
				return (STATUS_OK);
			}
		}
	}

	return (STATUS_FAIL);
}

t_status	WebServer::_waitSrvs(void)
{
	std::string	bufs[MAX_EVENT];
	char		*tmpbufs[MAX_EVENT] = {NULL};

	size_t	size = 0;
	epoll_event evs[MAX_EVENT];
	int	nfds = 0;
	int i = 0;
	int sock_fd = 0;

	// TODO: Setup Signals
	sig_setup();
	this->_run = 1;

	while (_run)
	{
		nfds = _epoll.Wait(evs, MAX_EVENT, -1);

		for (i = 0; i < nfds; i++) {

			if (_acceptClient(&evs[i]) == STATUS_OK)
				continue;

			if (evs[i].events & EPOLLIN) {
				if ((unsigned long long)evs[i].data.fd < lowest_heap_address()) { //black magic: epoll_event.data is union, so if we store an fd instead on a ptr, it will always be inferior to a memory address
					#if DEBUG
						std::cout << DBG << "[WebServer::_waitSrvs()] EPOLLIN : timerfd. Sending timeout response." << std::endl;
					#endif
					::read(evs[i].data.fd, &size, sizeof(size));
					HttpResponse timeout(HTTP_STATUS_REQUEST_TIMEOUT);
					FOREACH_VECTOR(Socket*, _clients, client)
					{
						if (evs[i].data.fd == this->_timerfds[(*client)->Getfd()])
						{
							write((*client)->Getfd(), timeout.getResponse().c_str(), timeout.getResponse().size());
							::close((*client)->Getfd());
							this->_epoll.Ctl(EPOLL_CTL_DEL, (*client)->Getfd(), NULL);
							this->_epoll.Ctl(EPOLL_CTL_DEL, this->_timerfds[(*client)->Getfd()], NULL);
							this->_timerfds.erase((*client)->Getfd());
							bufs[(*client)->Getfd() % MAX_EVENT].clear();
							break;
						}
					}
					::close(evs[i].data.fd);
					continue;
				}

				sock_fd = static_cast<Socket*>(evs[i].data.ptr)->Getfd();
				::ioctl(sock_fd, FIONREAD, &size);
				if (!size){
					continue;
				}

					#if DEBUG
					std::cout << DBG << "[WebServer::_waitSrvs()] EPOLLIN : size of packet: " << size << std::endl;
					#endif
				tmpbufs[sock_fd % MAX_EVENT] = new char[size + 1];

				if (!tmpbufs[sock_fd % MAX_EVENT]) {
					std::cout << WARN << "[WebServer::_waitSrvs()] Bad Alloc" << std::endl;
					continue ;
				}

				if (::read(sock_fd, tmpbufs[sock_fd % MAX_EVENT], size) != (int)size)
				{
						#if DEBUG
						std::cout << DBG << WARN << "[WebServer::_waitSrvs()] Read error" << std::endl;
						#endif
					continue ;
				}
				bufs[sock_fd % MAX_EVENT].reserve(size);
				for (size_t j = 0; j < size; j++)
					bufs[sock_fd % MAX_EVENT].push_back(tmpbufs[sock_fd % MAX_EVENT][j]);

					#if DEBUG
					std::cout << DBG << "[WebServer::_waitSrvs()] EPOLLIN : request size now of: " << bufs[sock_fd % MAX_EVENT].size() << std::endl;
					#endif

				delete[] tmpbufs[sock_fd % MAX_EVENT];
				tmpbufs[sock_fd % MAX_EVENT] = NULL;

				//we reset the timerfd
				#if DEBUG
					std::cout << DBG << "[WebServer::_waitSrvs()] EPOLLIN : Resetting timerfd" << std::endl;
				#endif
				struct itimerspec new_value;
				new_value.it_value.tv_sec = 10;
				new_value.it_value.tv_nsec = 0;
				new_value.it_interval.tv_sec = 0;
				new_value.it_interval.tv_nsec = 0;
				timerfd_settime(this->_timerfds[sock_fd], 0, &new_value, NULL);

				if (!(evs[i].events & EPOLLOUT) && (bufs[sock_fd % MAX_EVENT].find("\r\n\r\n") != std::string::npos || bufs[sock_fd % MAX_EVENT].find("\n\n") != std::string::npos))
				{
					if (bufs[sock_fd % MAX_EVENT].substr(0, 4) == "POST")
					{
						#if DEBUG
							std::cout << DBG << "[WebServer::_waitSrvs()] EPOLLIN : Found end of header (\\r\\n\\r\\n), but method POST. Searching content-length..." << std::endl;
						#endif
						if (   bufs[sock_fd % MAX_EVENT].find("Content-Length: ") > bufs[sock_fd % MAX_EVENT].find("\r\n\r\n")
								&& bufs[sock_fd % MAX_EVENT].find("content-length: ") > bufs[sock_fd % MAX_EVENT].find("\r\n\r\n"))
						{
						#if DEBUG
								std::cout << DBG << "[WebServer::_waitSrvs()] No content-length found. Returning 411" << std::endl;
						#endif
							HttpResponse res(411);
							::write(sock_fd, res.getResponse().c_str(), res.getResponse().size());
							::close(sock_fd);
							this->_epoll.Ctl(EPOLL_CTL_DEL, sock_fd, NULL);
							this->_epoll.Ctl(EPOLL_CTL_DEL, this->_timerfds[sock_fd], NULL);
							this->_timerfds.erase(sock_fd);
							bufs[sock_fd % MAX_EVENT].clear();
							continue;
						}
						else
						{
								#if DEBUG
								std::cout << DBG << "[WebServer::_waitSrvs()] Content-length found. parsing value..." << std::endl;
								#endif
							size_t pos = bufs[sock_fd % MAX_EVENT].find("Content-Length:");
							if (pos == std::string::npos)
								pos = bufs[sock_fd % MAX_EVENT].find("content-length:");
							size_t pos2 = bufs[sock_fd % MAX_EVENT].find("\r\n", pos);
							std::string content_length = bufs[sock_fd % MAX_EVENT].substr(pos + 15, pos2 - pos - 15);
								#if DEBUG
								std::cout << DBG << "[WebServer::_waitSrvs()] EPOLLIN : Content-length parsed. Content-length: " << content_length << std::endl;
								#endif
							if (StringToNumber<unsigned long>(content_length) > bufs[sock_fd % MAX_EVENT].size() - bufs[sock_fd % MAX_EVENT].find("\r\n\r\n") - 4)
							{
								#if DEBUG
									std::cout << DBG << "[WebServer::_waitSrvs()] Content-length is bigger than what we have. Keeping EPOLLIN and rolling again!" << std::endl;
								#endif
								continue;
							} else
							{
								#if DEBUG
									std::cout << DBG << "[WebServer::_waitSrvs()] Content-length is smaller than what we have (" <<  bufs[sock_fd % MAX_EVENT].size() - bufs[sock_fd % MAX_EVENT].find("\r\n") - 4 << "). Setting EPOLLOUT." << std::endl;
								#endif
								evs[i].events = EPOLLIN | EPOLLOUT;
								this->_epoll.Ctl(EPOLL_CTL_MOD, sock_fd, &evs[i]);
								continue ;
							}
						}
					}
						#if DEBUG
						std::cout << DBG << "[WebServer::_waitSrvs()] EPOLLIN : Found end of header (\\r\\n\\r\\n). Setting EPOLLOUT. If there's data to be read, it should be read before going to EPOLLOUT" << std::endl;
						#endif
					evs[i].events = EPOLLIN | EPOLLOUT;
					this->_epoll.Ctl(EPOLL_CTL_MOD, sock_fd, &evs[i]);
				}
			}
			else if (evs[i].events & EPOLLOUT) {

				HttpRequest req(bufs[sock_fd % MAX_EVENT], static_cast<Socket*>(evs[i].data.ptr)->GetSrvPort(), const_cast<char*>(static_cast<Socket*>(evs[i].data.ptr)->InetNtoa(static_cast<Socket*>(evs[i].data.ptr)->GetSin()->sin_addr.s_addr).c_str()));
					#if DEBUG
					std::cout << DBG << "[WebServer::Wait] Parsed request: " << std::endl << req << std::endl
					#endif

				HttpResponse res(req);
					#if DEBUG
					std::cout << DBG << "[WebServer::Wait] Generated response, status code: " << res.getStatusCodeStr() << std::endl;
					#endif
				write(static_cast<Socket*>(evs[i].data.ptr)->Getfd(), res.getResponse().c_str(), res.getResponse().size());

				::close(static_cast<Socket*>(evs[i].data.ptr)->Getfd());
				_epoll.Ctl(EPOLL_CTL_DEL, static_cast<Socket*>(evs[i].data.ptr)->Getfd(), NULL);
				#if DEBUG
					std::cout << DBG << "[WebServer::_wait] _clients.size(): " << _clients.size() << std::endl;
				#endif
				FOREACH_VECTOR(Socket*, _clients, it){
					if (*it == evs[i].data.ptr) _clients.erase(it);
				}
				::close(static_cast<Socket*>(evs[i].data.ptr)->GetTfd());
				_epoll.Ctl(EPOLL_CTL_DEL, static_cast<Socket*>(evs[i].data.ptr)->GetTfd(), NULL);
				this->_timerfds.erase(static_cast<Socket*>(evs[i].data.ptr)->Getfd());
				delete static_cast<Socket*>(evs[i].data.ptr);
				bufs[sock_fd % MAX_EVENT].clear();
				#if DEBUG
					std::cout << DBG << "[WebServer::_wait] response sent and cleanup done." << std::endl;
				#endif
			}

		}
	}

	FOREACH_VECTOR(Socket*, this->_clients, it){ delete *it; }
	for (int i = 0; i < MAX_EVENT; i++)
		if (tmpbufs[i]) delete tmpbufs[i];
	return (STATUS_OK);
}
