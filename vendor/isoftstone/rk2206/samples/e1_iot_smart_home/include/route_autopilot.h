#define MAX_WAYPOINTS 3

typedef struct {
    double lng;
    double lat;
} GpsPoint;

GpsPoint waypointList[MAX_WAYPOINTS];
int waypointCount = 0;
