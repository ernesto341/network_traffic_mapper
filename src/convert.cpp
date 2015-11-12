/* Source:
 * http://www.codeproject.com/Questions/626899/Converting-Latitude-And-Longitude-to-an-X-Y-Coordi
 * http://stackoverflow.com/questions/16080225/convert-lat-long-to-x-y-coordinates-c
 */

#include <convert.h>

using namespace std;

bool convert (const Flt & lat, const Flt & lon, Flt & x, Flt & y)
{
    x = fmod((image_dims[0] * (180. + lon) / 360.), (image_dims[0] + (image_dims[0] / 2.)));
    y = (image_dims[1] / 2.) - (image_dims[0] * (log(tan((PI/4.)+((lat*PI/180.)/2.)))) / (2. * PI));
    return (true);
}

bool convert (const Vec & latlon, Vec & xy)
{
    if (!latlon || !xy)
    {
        return (false);
    }
    xy[0] = fmod((image_dims[0] * (180. + latlon[1]) / 360.), (image_dims[0] + (image_dims[0] / 2.)));
    xy[1] = (image_dims[1] / 2.) /*-*/+ (image_dims[0] * (log(tan((PI/4.)+((latlon[0]*PI/180.)/2.)))) / (2. * PI));
    return (true);
}
