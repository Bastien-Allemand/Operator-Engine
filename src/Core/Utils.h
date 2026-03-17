#pragma once	

Vector<Vector2f> ProjectToPlane(Vector<Vector3f> const& points);

	
Vector<uint32> Triangulate(Vector<Vector2f> const& pos);

	
Vector<String> SplitString(String const& toSplit, char separator);

	
Vector<String> EmptyRemover(Vector<String> vect);