#include <capture.h>

using namespace std;

int fib(int n)
{
	if (n == 0)
		return 0;
	if (n == 1)
		return 1;
	return (fib(n-1) + fib(n-2));
}

void * simulateCapture(void * args)
{
	threadStuff * arg = (threadStuff *)args;
	while (!done)
	{
		Vec tmp1, tmp2;
		char src_ip [16] = {0};
		char dst_ip [16] = {0};
		int src_port, dst_port;
		char * tmp;
		Line * line = 000;
		pthread_mutex_lock(&(arg->live_data_mutex));
		if (arg->live_data.size() < 5)
		{
			switch (rand() % 2)
			{
				case 0:
					tmp = itoa((rand() % 254) + 1);
					if (!tmp)
						break;
					while (strstr(tmp, "192") != 0 || strstr(tmp, "172") != 0 || strstr(tmp, "10") != 0)
					{
						tmp = itoa((rand() % 254) + 1);
						if (!tmp)
							break;
					}
					switch(strlen(tmp))
					{
						case 1:
							dst_ip[0] = '0';
							dst_ip[1] = '0';
							dst_ip[2] = tmp[0];
							break;
						case 2:
							dst_ip[0] = '0';
							dst_ip[1] = tmp[0];
							dst_ip[2] = tmp[1];
							break;
						case 3:
							dst_ip[0] = tmp[0];
							dst_ip[1] = tmp[1];
							dst_ip[2] = tmp[2];
							break;
						default:
							// uhh...?
							break;
					}
					dst_ip[3] = '.';
					tmp = itoa((rand() % 254) + 1);
					if (!tmp)
						break;
					switch(strlen(tmp))
					{
						case 1:
							dst_ip[4] = '0';
							dst_ip[5] = '0';
							dst_ip[6] = tmp[0];
							break;
						case 2:
							dst_ip[4] = '0';
							dst_ip[5] = tmp[0];
							dst_ip[6] = tmp[1];
							break;
						case 3:
							dst_ip[4] = tmp[0];
							dst_ip[5] = tmp[1];
							dst_ip[6] = tmp[2];
							break;
						default:
							// uhh...?
							break;
					}
					dst_ip[7] = '.';
					tmp = itoa((rand() % 254) + 1);
					if (!tmp)
						break;
					switch(strlen(tmp))
					{
						case 1:
							dst_ip[8] = '0';
							dst_ip[9] = '0';
							dst_ip[10] = tmp[0];
							break;
						case 2:
							dst_ip[8] = '0';
							dst_ip[9] = tmp[0];
							dst_ip[10] = tmp[1];
							break;
						case 3:
							dst_ip[8] = tmp[0];
							dst_ip[9] = tmp[1];
							dst_ip[10] = tmp[2];
							break;
						default:
							// uhh...?
							break;
					}
					dst_ip[11] = '.';
					tmp = itoa((rand() % 254) + 1);
					if (!tmp)
						break;
					switch(strlen(tmp))
					{
						case 1:
							dst_ip[12] = '0';
							dst_ip[13] = '0';
							dst_ip[14] = tmp[0];
							break;
						case 2:
							dst_ip[12] = '0';
							dst_ip[13] = tmp[0];
							dst_ip[14] = tmp[1];
							break;
						case 3:
							dst_ip[12] = tmp[0];
							dst_ip[13] = tmp[1];
							dst_ip[14] = tmp[2];
							break;
						default:
							// uhh...?
							break;
					}
					dst_ip[15] = '\0';
					tmp = itoa((rand() % 254) + 1);
					if (!tmp)
						break;
					while (strstr(tmp, "192") != 0 || strstr(tmp, "172") != 0 || strstr(tmp, "10") != 0)
					{
						tmp = itoa((rand() % 254) + 1);
						if (!tmp)
							break;
					}
					switch(strlen(tmp))
					{
						case 1:
							src_ip[0] = '0';
							src_ip[1] = '0';
							src_ip[2] = tmp[0];
							break;
						case 2:
							src_ip[0] = '0';
							src_ip[1] = tmp[0];
							src_ip[2] = tmp[1];
							break;
						case 3:
							src_ip[0] = tmp[0];
							src_ip[1] = tmp[1];
							src_ip[2] = tmp[2];
							break;
						default:
							// uhh...?
							break;
					}
					src_ip[3] = '.';
					tmp = itoa((rand() % 254) + 1);
					if (!tmp)
						break;
					switch(strlen(tmp))
					{
						case 1:
							src_ip[4] = '0';
							src_ip[5] = '0';
							src_ip[6] = tmp[0];
							break;
						case 2:
							src_ip[4] = '0';
							src_ip[5] = tmp[0];
							src_ip[6] = tmp[1];
							break;
						case 3:
							src_ip[4] = tmp[0];
							src_ip[5] = tmp[1];
							src_ip[6] = tmp[2];
							break;
						default:
							// uhh...?
							break;
					}
					src_ip[7] = '.';
					tmp = itoa((rand() % 254) + 1);
					if (!tmp)
						break;
					switch(strlen(tmp))
					{
						case 1:
							src_ip[8] = '0';
							src_ip[9] = '0';
							src_ip[10] = tmp[0];
							break;
						case 2:
							src_ip[8] = '0';
							src_ip[9] = tmp[0];
							src_ip[10] = tmp[1];
							break;
						case 3:
							src_ip[8] = tmp[0];
							src_ip[9] = tmp[1];
							src_ip[10] = tmp[2];
							break;
						default:
							// uhh...?
							break;
					}
					src_ip[11] = '.';
					tmp = itoa((rand() % 254) + 1);
					if (!tmp)
						break;
					switch(strlen(tmp))
					{
						case 1:
							src_ip[12] = '0';
							src_ip[13] = '0';
							src_ip[14] = tmp[0];
							break;
						case 2:
							src_ip[12] = '0';
							src_ip[13] = tmp[0];
							src_ip[14] = tmp[1];
							break;
						case 3:
							src_ip[12] = tmp[0];
							src_ip[13] = tmp[1];
							src_ip[14] = tmp[2];
							break;
						default:
							// uhh...?
							break;
					}
					src_ip[15] = '\0';
					// if bad ip, try again next iteration
					if (!ipToCoord(tmp1, src_ip))
					{
						break;
					}
					if (!ipToCoord(tmp2, dst_ip))
					{
						break;
					}
					src_port = ((rand() % 65535) + 1);
					dst_port = ((rand() % 65535) + 1);
					try
					{
						line = new Line(tmp1, tmp2, src_ip, dst_ip, src_port, dst_port);
					}
					catch(...)
					{
						line = 000;
					}
					break;
				case 1:
					fib(25);
					break;
			}
			if (line != 000)
			{
				line->valid = 1;
				arg->live_data.push(*line);
			}
			if (!arg->live_data.empty())
			{
				if (pthread_mutex_trylock(&lines_mutex) == 0) // got the lock
				{
					// dump to ipc channel
					while (!arg->live_data.empty())
					{
						lines.push_back(arg->live_data.front());
						arg->live_data.pop();
					}
					pthread_mutex_unlock(&lines_mutex);
				}
			}
			if (line != 000)
			{
				delete(line);
				line = 000;
			}
			pthread_mutex_unlock(&arg->live_data_mutex);
		}
		else
		{
			if (pthread_mutex_lock(&lines_mutex) == 0) // got the lock
			{
				// dump to ipc channel
				while (!arg->live_data.empty())
				{
					lines.push_back(arg->live_data.front());
					arg->live_data.pop();
				}
				pthread_mutex_unlock(&lines_mutex);
			}
			if (line != 000)
			{
				delete(line);
				line = 000;
			}
			pthread_mutex_unlock(&arg->live_data_mutex);
		}
	}
	pthread_mutex_lock(&arg->live_data_mutex);
	while (!arg->live_data.empty())
	{
		arg->live_data.pop();
	}
	pthread_mutex_unlock(&arg->live_data_mutex);
	return ((void *)0);
}

void * capture(void * args)
{
	threadStuff * arg = (threadStuff *)args;
	if (!arg)
	{
		done = 1;
		return ((void *)0);
	}
	int src_port = 0;
	int dst_port = 0;
	char src_ip[16] = {0};
	char dst_ip[16] = {0};
	char errbuf[PCAP_ERRBUF_SIZE];
	const u_char * packet;
	struct pcap_pkthdr header;
	struct ip * ip;
	struct tcphdr * tcp;
	Vec tmp1, tmp2;
	Line * line;
	pcap_t * handle = pcap_open_live(arg->dev, 65535, 1, 0, errbuf);
	if (handle == 000)
	{
		done = 1;
		return ((void *)0);
	}
	// capture loop
	while (!done && (packet = pcap_next(handle, &header)) != 0)
	{
		ip = (struct ip*)(packet + sizeof(struct ether_header));
		tcp = (struct tcphdr*)(packet + sizeof(struct ether_header) + sizeof(struct ip));
		if (ip->ip_hl * 4 < (int)sizeof(struct ip)) // invalid header len
		{
			continue;
		}
		strncpy(src_ip, inet_ntoa(ip->ip_src), 16);
		strncpy(dst_ip, inet_ntoa(ip->ip_dst), 16);
		src_port = tcp->th_sport;
		dst_port = tcp->th_dport;
		if (!ipToCoord(tmp1, src_ip))
		{
			break;
		}
		if (!ipToCoord(tmp2, dst_ip))
		{
			break;
		}
		src_port = ((rand() % 65535) + 1);
		dst_port = ((rand() % 65535) + 1);
		try
		{
			line = new Line(tmp1, tmp2, src_ip, dst_ip, src_port, dst_port);
		}
		catch(...)
		{
			line = 000;
		}
		if (line != 000)
		{
			pthread_mutex_lock(&(arg->live_data_mutex));
			line->valid = 1;
			arg->live_data.push(*line);
			switch (rand() % 3) // dont hog the lines vector. hopefully the dump doesnt take too long...
			{
				case 0:
					if (pthread_mutex_trylock(&lines_mutex) == 0) // got the lock
					{
						// dump to ipc channel
						while (!arg->live_data.empty())
						{
							lines.push_back(arg->live_data.front());
							arg->live_data.pop();
						}
						pthread_mutex_unlock(&lines_mutex);
					}
					break;
				default:
					break;
			}
			if (line != 000)
			{
				delete(line);
				line = 000;
			}
			pthread_mutex_unlock(&(arg->live_data_mutex));
		}
	}
	pthread_mutex_lock(&arg->live_data_mutex);
	while (!arg->live_data.empty())
	{
		arg->live_data.pop();
	}
	pthread_mutex_unlock(&arg->live_data_mutex);
	return ((void *)0);
}
