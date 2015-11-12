#include <iptocoord.h>

using namespace std;

/* reference
 * GeoIP - MaxMind, LLC
 */

#ifndef GEOFILES
#define GEOFILES
const char * dirname1 = "./res/GeoLiteCity_20151006/\0";
const char * dirname2 = "../res/GeoLiteCity_20151006/\0";
const char * blocksname = "GeoLiteCity-Blocks.csv\0";
const char * locationsname = "GeoLiteCity-Location.csv\0";
#endif

static unsigned long __addr_to_num(const char * addr)
{
	unsigned int c = 0, octet = 0, t = 0;
	unsigned long ipnum = 0;
	int i = 3;
	while ((c = (unsigned int)*addr++))
	{
		if (c == '.')
		{
			if (octet > 255)
			{
				return (0);
			}
			ipnum <<= 8;
			ipnum += octet;
			--i;
			octet &= 0x00000000;
		}
		else
		{
			t = octet;
			octet <<= 3;
			octet += t;
			octet += t;
			c -= '0';
			if (c > 9)
			{
				return (0);
			}
			octet += c;
		}
	}
	if ((octet > 255) || (i != 0))
	{
		return (0);
	}
	ipnum <<= 8;
	return (ipnum + octet);
}

bool ipToCoord(Vec & latlon, const char * ip)
{
	if (!ip)
	{
		return (false);
	}
	ifstream blocks, locations;
	string fname = "";
	fname = dirname1;
	fname += locationsname;
	locations.open(fname);
	if (locations.fail())
	{
		fname = dirname2;
		fname += locationsname;
		locations.open(fname);
		if (locations.fail())
		{
			return (false);
		}
	}
	fname = dirname1;
	fname += blocksname;
	blocks.open(fname);
	if (blocks.fail())
	{
		fname = dirname2;
		fname += blocksname;
		blocks.open(fname);
		if (blocks.fail())
		{
			return (false);
		}
	}

	string line = ip;
	unsigned long target_block = __addr_to_num(ip), target_loc = 0;
	Flt lat = 0.f, lon = 0.f;
	string tmp = "";
	if (target_block == 0)
	{
		return (false);
	}
	while (blocks.good() && getline(blocks, line))
	{
		if (line.length() < 5 || line[0] != '"')
		{
			continue;
		}
		line.erase(0, 1);
		if (stoul(line.substr(0, line.find_first_of('"'))) <= target_block && stoul(line.substr(line.find_first_of(',') + 2, line.find_last_of(',') - 2)) >= target_block)
		{
			target_loc = stoul(line.substr(line.find_last_of(',') + 2, line.find_last_of('"') - 1));
			break;
		}
	}
	if (blocks.fail() || target_loc == 0)
	{
		return (false);
	}
	while (locations.good() && getline(locations, line))
	{
		if (line.length() < 5 || !isdigit(line[0]))
		{
			continue;
		}
		if (stoul(line.substr(0, line.find_first_of(','))) == target_loc)
		{
			try
			{
				tmp = (line.substr(line.find_last_of('"') + 2, line.find_last_of('.')));
				lat = stof(tmp);
				tmp = line.substr(line.find_last_of('.') - 4, line.find_last_of('.') + 4);
				if (tmp[0] != '-')
				{
					tmp.erase(0, 1);
					lon = stof(tmp);
				}
				else
				{
					lon = stof(tmp);
				}
			}
			catch(...)
			{
				lat = 0.f;
				lon = 0.f;
				return (false);
			}
			latlon[0] = lat;
			latlon[1] = lon;
			break;
		}
	}
	if (locations.fail())
	{
		return (false);
	}

	locations.close();
	blocks.close();
	return (true);
}

