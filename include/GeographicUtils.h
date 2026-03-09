#ifndef GEOGRAPHICUTILS_H
#define GEOGRAPHICUTILS_H

#include "StreetMap.h"
#include <vector>

struct SGeographicUtils{
    static double DegreesToRadians(double deg); // Convert an angle from degrees to radians. Many math functions in C++ (sin, cos, atan2) expect radians instead of degrees.
    static double RadiansToDegrees(double rad); // Convert an angle from radians back to degrees.
    static double Normalize360(double deg); // Normalize an angle so that it lies within the range [0, 360).
    static double Normalize180180(double deg);// Normalize an angle so that it lies within the range (-180, 180].
    static double HaversineDistanceInMiles(CStreetMap::SLocation loc1, CStreetMap::SLocation loc2); // Compute the great-circle distance between two geographic coordinates
    static double CalculateBearing(CStreetMap::SLocation src, CStreetMap::SLocation dest); // Calculate the bearing (direction angle) from src to dest. This is used when determining navigation directions or turns.
    static double CalculateExternalBisector(double bear1, double bear2); // Calculate the external angle bisector between two bearings.
    static bool CalculateExtents(const std::vector<CStreetMap::SLocation> &locations, CStreetMap::SLocation &lowerleft, CStreetMap::SLocation &upperright); // Compute the bounding box (minimum and maximum coordinates) for a list of geographic locations.
    static std::vector<CStreetMap::SLocation> FilterLocations(const std::vector<CStreetMap::SLocation> &locations, const CStreetMap::SLocation &lowerleft, const CStreetMap::SLocation &upperright); // Filter a list of locations and return only those that lie within the specified bounding box.
    static std::string BearingToDirection(double bearing);// Convert a bearing angle into a compass direction string.
    static std::string CalcualteExternalBisectorDirection(CStreetMap::SLocation src, CStreetMap::SLocation mid, CStreetMap::SLocation dest); // Determine the compass direction of the external angle bisector formed by three locations: src -> mid -> dest.Used to determine turn directions in navigation instructions
    static std::string ConvertLLToDMS(CStreetMap::SLocation loc);// Convert latitude/longitude in decimal degrees to Degrees-Minutes-Seconds (DMS) format string.
};

#endif
