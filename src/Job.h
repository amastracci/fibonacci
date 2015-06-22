#ifndef JOB_H
#define JOB_H

#include <boost/asio.hpp>

#include "api.h"

struct Job
{
	Job(boost::asio::ip::tcp::socket sock) :
		sock(std::move(sock)),
		error(fibonacci_api::ERR_OK)
	{
	}
	boost::asio::ip::tcp::socket sock;
	fibonacci_api::error_t	error;
	uint8_t		query;
	uint64_t	reply;
};

#endif // JOB_H
