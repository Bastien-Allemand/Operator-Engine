#include "Utils.h"
#include "pch.h"

#include "delaunay.hpp"


Vector<Vector2f> ProjectToPlane(Vector<Vector3f> const& points)
{

    Vector3f centroid = { 0, 0, 0 };
    for (Vector3f p : points)
        centroid = centroid + p;
    centroid = centroid / float(points.size());

    Vector3f xAxis = Normalize((points[1] - points[0]));
    Vector3f temp = Normalize((points[2] - points[0]));

    Vector3f zAxis = Normalize(Cross(xAxis,temp));
    Vector3f yAxis = Normalize(Cross(zAxis,xAxis));

    Vector<Vector2f> projected;
    for (Vector3f p : points)
    {
        Vector3f local = p - centroid;
        float x = Dot(local,xAxis);
        float y = Dot(local,yAxis);
        projected.push_back({ x, y });
    }

    return projected;
}

Vector<uint32> Triangulate(Vector<Vector2f> const& pos)
{
    Vector<uint32> indices;
    Tessellator meshIndices(pos);

    for (Triangle tri : meshIndices.GetTriangles())
    {
        indices.push_back(tri.a_);
        indices.push_back(tri.b_);
        indices.push_back(tri.c_);
    }

    return indices;
}

Vector<String> SplitString(String const& toSplit, char separator)
{
    Vector<String> result;
    String currentSegment;

    for (uint32 i = 0; i < toSplit.size(); i++)
    {
        if (toSplit[i] != separator)
        {
            if (toSplit[i] == '\n' || toSplit[i] == '\r')
                continue;
            currentSegment += toSplit[i];
        }
        else
        {
            result.push_back(currentSegment);
            currentSegment.clear();
        }

    }

    result.push_back(currentSegment);
    return result;
}

Vector<String> EmptyRemover(Vector<String> vect)
{
    Vector<String> emptyDel;

    for (uint64 i = 0; i < vect.size(); i++)
        if (vect[i].empty() == false)
            emptyDel.push_back(vect[i]);
    return emptyDel;
}