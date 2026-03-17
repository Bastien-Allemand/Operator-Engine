/*
	MIT License

	Copyright (c) 2019 Cordell Barron

	Permission is hereby granted, free of charge, to any person obtaining a copy
	of this software and associated documentation files (the "Software"), to deal
	in the Software without restriction, including without limitation the rights
	to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
	copies of the Software, and to permit persons to whom the Software is
	furnished to do so, subject to the following conditions:

	The above copyright notice and this permission notice shall be included in all
	copies or substantial portions of the Software.

	THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
	IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
	FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
	AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
	LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
	OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
	SOFTWARE.

	part of https://github.com/spiroyster/delaunay
*/
#pragma once

#include "define.h"
#include <assert.h>
#include <algorithm>
#include <limits>

	struct Edge { uint32 i_, j_; };
	struct Triangle { uint32 a_, b_, c_; };

	static float64 const SUPER_TRIANGLE_SIZE = 2000.0;

	namespace convenience
	{
		void Unique(Vector<Vector2f>& vertices)
		{
			std::sort(vertices.begin(), vertices.end(), [](Vector2f const& a, Vector2f const& b) { return a.x == b.x ? a.y < b.y : a.x < b.x; });
			vertices.erase(std::unique(vertices.begin(), vertices.end(), [](Vector2f const& a, Vector2f const& b) { return a.x == b.x && a.y == b.y; }), vertices.end());
		}
	}

	class Tessellator
	{
		struct WorkingVertex
		{
			WorkingVertex(Vector2f const* pV, uint32 index) :	v_(pV), index_(index) { }

			float64 Dist(Vector2f const&  V) const
			{
				float64 const dx = v_->x - V.x;
				float64 const dy = v_->y - V.y;
				return dx * dx + dy * dy;
			}

			Vector2f const* v_;
			uint32 index_;
		};

		static bool OrEquals(uint32 a, uint32 b, uint32 c, uint32 equals)
		{
			return (a == equals || b == equals || c == equals);
		}

		struct WorkingTriangle
		{
			WorkingTriangle(WorkingVertex const* pA, WorkingVertex const* pB, WorkingVertex const* pC)
				: pA_(pA), pB_(pB), pC_(pC), valid_(true) {}

			bool ContainsVertex(uint32 index)
			{
				return OrEquals(pA_->index_, pB_->index_, pC_->index_, index);
			}

			WorkingVertex const* pA_;
			WorkingVertex const* pB_;
			WorkingVertex const* pC_;
			bool valid_;

			bool IsWithinCircumcircle(WorkingVertex const& v) const
			{
				float64 const ab = pA_->v_->x * pA_->v_->x + pA_->v_->y * pA_->v_->y;
				float64 const cd = pB_->v_->x * pB_->v_->x + pB_->v_->y * pB_->v_->y;
				float64 const ef = pC_->v_->x * pC_->v_->x + pC_->v_->y * pC_->v_->y;

				float64 const ax = pA_->v_->x;
				float64 const ay = pA_->v_->y;
				float64 const bx = pB_->v_->x;
				float64 const by = pB_->v_->y;
				float64 const cx = pC_->v_->x;
				float64 const cy = pC_->v_->y;

				float64 const circum_x = (ab * (cy - by) + cd * (ay - cy) + ef * (by - ay)) / (ax * (cy - by) + bx * (ay - cy) + cx * (by - ay));
				float64 const circum_y = (ab * (cx - bx) + cd * (ax - cx) + ef * (bx - ax)) / (ay * (cx - bx) + by * (ax - cx) + cy * (bx - ax));

				Vector2f circum(circum_x * 0.5, circum_y * 0.5);
				return  v.Dist(circum) <= pA_->Dist(circum);
			}
		};

		struct WorkingEdge
		{
			WorkingEdge(WorkingVertex const* pI, WorkingVertex const* pJ)
				: i_(pI), j_(pJ), valid_(true)
			{
			}

			bool operator==(WorkingEdge const& rhs) const
			{
				return (rhs.i_ == i_ && rhs.j_ == j_) || (rhs.i_ == j_ && rhs.j_ == i_);
			}

			WorkingVertex const* i_;
			WorkingVertex const* j_;
			bool valid_;
		};

		float64 AngleDifference(Vector2f const& a, Vector2f const& b)
		{
			float64 angle = atan2(b.y, b.x) - atan2(a.y, a.x);

			if (angle > PI)
				angle -= 2 * PI;
			else if (angle <= -PI)
				angle += 2 * PI;
			
			return angle;
		}

		bool SegmentSegmentIntersection(Vector2f const& a, Vector2f const& b, Vector2f const& c, Vector2f const& d)
		{
			float64 s1_x, s1_y, s2_x, s2_y;
			s1_x = b.x - a.x;     s1_y = b.y - a.y;
			s2_x = d.x - c.x;     s2_y = d.y - c.y;
			
			float64 sDenom = (-s2_x * s1_y + s1_x * s2_y);
			float64 tDenom = (-s2_x * s1_y + s1_x * s2_y);
			
			if (!sDenom || !tDenom)
				return false;
			
			float64 s = (-s1_y * (a.x - c.x) + s1_x * (a.y - c.y)) / sDenom;
			float64 t = (s2_x * (a.y - c.y) - s2_y * (a.x - c.x)) / tDenom;
			
			return (s >= 0 && s <= 1 && t >= 0 && t <= 1);
		}
		

	public:
		Tessellator(Vector<Vector2f> const& points)
			: vertices_(points)
		{
			#ifdef NDEBUG
			Vector<Vector2f32> verticesCopy = points;
			assert(verticesCopy.size() == points.size());
			#endif

			Vector<uint32> allVertexIndexes(points.size(), 0);	
			for (uint32 i = 0; i < points.size(); ++i)
				allVertexIndexes[i] = i;
			
			Tessellate(allVertexIndexes);

			// Remove our duplicate edges...
			RemoveDuplicateEdges();
		}

		void AddConstraint(Edge const& constraint)
		{
			// assert the requested constraint indexes are valid...
			//assert(constraint.i_ < vertices_.Size() && constraint.j_ < vertices_.Size());

			// Check if this edge is already present, if so do nothing...
			if (std::find_if(edges_.begin(), edges_.end(), [&constraint](Edge const& ee) { return (constraint.i_ == ee.i_ && constraint.j_ == ee.j_) || (constraint.i_ == ee.j_ && constraint.j_ == ee.i_); }) != edges_.end())
				return;

			Vector<uint32> leftSide, rightSide;

			// find out which edges this constraint straddles...
			for (auto e = edges_.begin(); e != edges_.end();)
			{
				// Don't check edges which share a constraint vertex...
				if (constraint.i_ != e->i_ && constraint.i_ != e->j_ && constraint.j_ != e->i_ && constraint.j_ != e->j_)
				{
					Vector2f const& ci = vertices_[constraint.i_];
					Vector2f const& cj = vertices_[constraint.j_];
					Vector2f const& ei = vertices_[e->i_];
					Vector2f const& ej = vertices_[e->j_];
					
					float64 iAngle = AngleDifference(Vector2f(ei.x - ci.x, ei.y - ci.y), Vector2f(cj.x - ci.x, cj.y - ci.y));
					float64 jAngle = AngleDifference(Vector2f(ej.x - ci.x, ej.y - ci.y), Vector2f(cj.x - ci.x, cj.y - ci.y));

					if ((iAngle < 0 && jAngle > 0) || (iAngle > 0 && jAngle < 0))
					{
						// constraint straddles an edge, so check for edge intersection...
						if (SegmentSegmentIntersection(ei, ej, ci, cj))
						{
							leftSide.push_back(iAngle > 0 ? e->i_ : e->j_);
							rightSide.push_back(iAngle < 0 ? e->i_ : e->j_);
							
							// Remove any triangles using the edge, and the edge itself...
							triangles_.erase(std::remove_if(triangles_.begin(), triangles_.end(), [&e](Triangle& t) { return OrEquals(t.a_, t.b_, t.c_, e->i_) && OrEquals(t.a_, t.b_, t.c_, e->j_); }), triangles_.end());
							e = edges_.erase(e);

							continue;
						}
					}
				}

				++e;
			}

			// add the constraint edge to both sides, and tessellate subsets...
			leftSide.push_back(constraint.i_);
			leftSide.push_back(constraint.j_);
			rightSide.push_back(constraint.i_);
			rightSide.push_back(constraint.j_);

			// remove duplicate indexes...
			std::sort(leftSide.begin(), leftSide.end());
			leftSide.erase(std::unique(leftSide.begin(), leftSide.end()), leftSide.end());
			std::sort(rightSide.begin(), rightSide.end());
			rightSide.erase(std::unique(rightSide.begin(), rightSide.end()), rightSide.end());

			// retessellate both the right and left side
			Tessellate(leftSide);
			Tessellate(rightSide);

			// Remove our duplicate edges...
			RemoveDuplicateEdges();
		}

		List<Triangle> const& GetTriangles() const { return triangles_; }
		Vector<Vector2f> const& GetVertices() const { return vertices_; }
		List<Edge> const& GetEdges() const { return edges_; }

	private:
		void Tessellate(Vector<uint32> const& vertexIndexes)
		{
			#ifdef NDEBUG
			assert(!vertexIndexes.empty());
			assert(!vertices_.empty());
			for (uint32 vi = 0; vi < vertexIndexes.size(); ++vi)
				assert(vertexIndexes[vi] < vertices_.size());
			Vector<uint32> vertexIndexesCopy = vertexIndexes;
			std::sort(vertexIndexesCopy.begin(), vertexIndexesCopy.end(), [](uint32 const& a, uint32 const& b) { return a < b; });
			assert(std::unique(vertexIndexesCopy.begin(), vertexIndexesCopy.end()) == vertexIndexesCopy.end());
			#endif

			// create our sublist that we tessellate to....
			Vector<WorkingVertex> workingVertices(vertexIndexes.size() + 3, WorkingVertex(0, 0));
			for (uint32 i = 0; i < vertexIndexes.size(); ++i)
				workingVertices[i] = WorkingVertex(&vertices_[vertexIndexes[i]], vertexIndexes[i]);

			// determine the super triangle...
			float64 minX = workingVertices[0].v_->x;
			float64 minY = workingVertices[0].v_->y;
			float64 maxX = minX;
			float64 maxY = minY;

			for (uint64 i = 0; i < vertexIndexes.size(); ++i)
			{
				if (workingVertices[i].v_->x < minX) minX = workingVertices[i].v_->x;
				if (workingVertices[i].v_->y < minY) minY = workingVertices[i].v_->y;
				if (workingVertices[i].v_->x > maxX) maxX = workingVertices[i].v_->x;
				if (workingVertices[i].v_->y > maxY) maxY = workingVertices[i].v_->y;
			}

			float64 dx = maxX - minX;
			float64 dy = maxY - minY;
			float64 deltaMax = (std::max)(dx, dy);
			float64 midx = (minX + maxX) * 0.5;
			float64 midy = (minY + maxY) * 0.5;

			// Add our three super triangle vertices...
			uint32 p1Index = static_cast<uint32>(vertices_.size());
			uint32	p2Index = static_cast<uint32>(vertices_.size() + 1);
			uint32	p3Index = static_cast<uint32>(vertices_.size() + 2);

			Vector2f p1(midx - SUPER_TRIANGLE_SIZE * deltaMax, midy - deltaMax);
			Vector2f p2(midx, midy + SUPER_TRIANGLE_SIZE * deltaMax);
			Vector2f p3(midx + SUPER_TRIANGLE_SIZE * deltaMax, midy - deltaMax);

			workingVertices[vertexIndexes.size()] = WorkingVertex(&p1, p1Index);
			workingVertices[vertexIndexes.size()+1] = WorkingVertex(&p2, p2Index);
			workingVertices[vertexIndexes.size()+2] = WorkingVertex(&p3, p3Index);

			// Create a list of triangles, and add the supertriangle in it
			List<WorkingTriangle> workingTriangles;
			workingTriangles.push_back(WorkingTriangle(&workingVertices[vertexIndexes.size()], &workingVertices[vertexIndexes.size()+1], &workingVertices[vertexIndexes.size()+2]));

			std::for_each(workingVertices.begin(), workingVertices.end(),
				[&workingTriangles](WorkingVertex& v) 
				{
					Vector<WorkingEdge> polygon;

					std::for_each(workingTriangles.begin(), workingTriangles.end(), [&v, &polygon](WorkingTriangle& t) 
						{
							if (t.IsWithinCircumcircle(v))
							{
								t.valid_ = false;
								polygon.push_back(WorkingEdge(t.pA_, t.pB_));
								polygon.push_back(WorkingEdge(t.pB_, t.pC_));
								polygon.push_back(WorkingEdge(t.pC_, t.pA_));
							}
						});

					workingTriangles.erase(std::remove_if(workingTriangles.begin(), workingTriangles.end(), [](WorkingTriangle& t) { return !t.valid_; }), workingTriangles.end());

					for (auto e1 = polygon.begin(); e1 != polygon.end(); ++e1)
					{
						for (auto e2 = e1 + 1; e2 != polygon.end(); ++e2)
						{
							if (*e1 == *e2)
							{
								e1->valid_ = false;
								e2->valid_ = false;
							}
						}
					}

					polygon.erase(std::remove_if(polygon.begin(), polygon.end(), [](WorkingEdge& e) { return !e.valid_; }), polygon.end());

					std::for_each(polygon.begin(), polygon.end(), [&workingTriangles, &v](WorkingEdge& e) { workingTriangles.push_back(WorkingTriangle(e.i_, e.j_, &v)); });
				
				});

			// Remove the original super triangle...
			workingTriangles.erase(std::remove_if(workingTriangles.begin(), workingTriangles.end(), 
				[&p1Index, &p2Index, &p3Index](WorkingTriangle& t) 
				{ 
					return (t.ContainsVertex(p1Index) ||
						t.ContainsVertex(p2Index) ||
						t.ContainsVertex(p3Index));
				}), workingTriangles.end());

			// Create our edges (required for constraint functionality) and triangles...
			for (auto tItr = workingTriangles.begin(); tItr != workingTriangles.end(); ++tItr)
			{
				triangles_.push_back(Triangle{ tItr->pA_->index_, tItr->pB_->index_, tItr->pC_->index_ });
				edges_.push_back(Edge{ tItr->pA_->index_, tItr->pB_->index_ });
				edges_.push_back(Edge{ tItr->pB_->index_, tItr->pC_->index_ });
				edges_.push_back(Edge{ tItr->pC_->index_, tItr->pA_->index_ });
			}
		}

		void RemoveDuplicateEdges()
		{
			std::for_each(edges_.begin(), edges_.end(),
				[](Edge& e)
				{
					if (e.j_ < e.i_)
					{
						uint32 j = e.j_;
						e.j_ = e.i_;
						e.i_ = j;
					}
				});
			edges_.sort([](Edge& e, Edge& ee)
				{
					return e.i_ == ee.i_ ? e.j_ < ee.j_ : e.i_ < ee.i_;
				});
			edges_.unique([](Edge& e, Edge& ee)
				{
					return (e.i_ == ee.i_ && e.j_ == ee.j_) || (e.i_ == ee.j_ && e.j_ == ee.i_);
				});
		}

		List<Triangle> triangles_;
		List<Edge> edges_;
		Vector<Vector2f> vertices_;
	};