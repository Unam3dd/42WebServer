#include "http_request.hpp"
#include "http_utils.hpp"
#include "http_colors.hpp"
#include "http_response.hpp"
#include "webserver.hpp"
#include <signals.hpp>
#include <streambuf>
#include <sys/epoll.h>
#include <unistd.h>
#include <iostream>
#include <cstring>
#include <fstream>

t_status	WebServer::_acceptClient(ev_t *e)
{
	Socket	*client = NULL;
	ev_t	tev;

	if (!e) return (STATUS_FAIL);

	FOREACH_VECTOR(HttpServer*, this->_srv, it){
		if (e->data.fd == (*it)->getSocket().Getfd()) {
			client = (*it)->getSocket().Accept();
			if (client) {
				(*it)->getClients().push_back(client);
				tev.data.ptr = client;
				tev.events = EPOLLIN;
				this->_epoll.Ctl(EPOLL_CTL_ADD, client->Getfd(), &tev);
				std::cout << SUCCESS << "[WebServer::Wait] New client Accepted ! " << client->InetNtoa(client->GetSin()->sin_addr.s_addr) << ":" << client->Ntohs(client->GetSin()->sin_port) << std::endl;
				_clients.push_back(client);
				return (STATUS_OK);
			}
		}
	}

	return (STATUS_FAIL);
}

t_status	WebServer::_waitSrvs(void)
{
	char	buf[0x10000];
	epoll_event evs[MAX_EVENT];
	int	nfds = 0;
	int i = 0;

	// TODO: Setup Signals
	sig_setup();
	this->_run = 1;

	while (_run)
	{
		nfds = _epoll.Wait(evs, MAX_EVENT, -1);

		for (i = 0; i < nfds; i++) {

			if (_acceptClient(&evs[i]) == STATUS_OK)
				continue ;

			memset(buf, 0, sizeof(buf));
			if (::read(static_cast<Socket*>(evs[i].data.ptr)->Getfd(), buf, sizeof(buf)) < 0)
				return (STATUS_FAIL);

			HttpRequest req(buf, static_cast<Socket*>(evs[i].data.ptr)->Ntohs(static_cast<Socket*>(evs[i].data.ptr)->GetSin()->sin_port));
			HttpResponse res(req);

			if (DEBUG)
				std::cout << DBG << "[WebServer::Wait] Received and parsed request: " << std::endl << req << std::endl;
			std::cout << res.getBody() << std::endl;
			write(static_cast<Socket*>(evs[i].data.ptr)->Getfd(), res.getBody().c_str(), res.getBody().size());

			::close(static_cast<Socket*>(evs[i].data.ptr)->Getfd());
			_epoll.Ctl(EPOLL_CTL_DEL, static_cast<Socket*>(evs[i].data.ptr)->Getfd(), NULL);
			if (DEBUG)
				std::cout << DBG << "[WebServer::_wait] _clients.size(): " << _clients.size() << std::endl;
			FOREACH_VECTOR(Socket*, _clients, it){
				if (*it == evs[i].data.ptr) _clients.erase(it);
			}
			delete static_cast<Socket*>(evs[i].data.ptr);
		}
	}

	FOREACH_VECTOR(Socket*, _clients, it) _clients.erase(it);

	return (STATUS_OK);
}
