//#	include "polygon.h"
#	include "convexpoly2.h"

#	include <cmath>
#	include <climits>
#	include <stdio.h>
#	include <memory.h>
#	include <assert.h>
#	include <float.h>
#	include <list>
#	include <vector>

// TAKEN FROM BOX2D.

namespace mt
{
	class triangle
	{
	public:
		float* x;
		float* y;

		triangle::triangle(float x1, float y1, float x2, float y2, float x3, float y3)
		{
			x = new float[3];
			y = new float[3];
			float dx1 = x2-x1;
			float dx2 = x3-x1;
			float dy1 = y2-y1;
			float dy2 = y3-y1;
			float cross = dx1*dy2-dx2*dy1;

			bool ccw = (cross > 0);

			if (ccw)
			{
				x[0] = x1; x[1] = x2; x[2] = x3;
				y[0] = y1; y[1] = y2; y[2] = y3;
			}
			else
			{
				x[0] = x1; x[1] = x3; x[2] = x2;
				y[0] = y1; y[1] = y3; y[2] = y2;
			}
		}
		    
		triangle::triangle()
		{
			x = new float[3];
			y = new float[3];
		}

		triangle::~triangle()
		{
			delete[] x;
			delete[] y;
		}

		void triangle::Set(const triangle& toMe)
		{
			for (signed int i=0; i<3; ++i)
			{
				x[i] = toMe.x[i];
				y[i] = toMe.y[i];
			}
		}

		bool triangle::IsInside(float _x, float _y)
		{
			if (_x < x[0] && _x < x[1] && _x < x[2]) return false;
			if (_x > x[0] && _x > x[1] && _x > x[2]) return false;
			if (_y < y[0] && _y < y[1] && _y < y[2]) return false;
			if (_y > y[0] && _y > y[1] && _y > y[2]) return false;

			float vx2 = _x-x[0]; float vy2 = _y-y[0];
			float vx1 = x[1]-x[0]; float vy1 = y[1]-y[0];
			float vx0 = x[2]-x[0]; float vy0 = y[2]-y[0];

			float dot00 = vx0*vx0+vy0*vy0;
			float dot01 = vx0*vx1+vy0*vy1;
			float dot02 = vx0*vx2+vy0*vy2;
			float dot11 = vx1*vx1+vy1*vy1;
			float dot12 = vx1*vx2+vy1*vy2;
			float invDenom = 1.0f / (dot00*dot11 - dot01*dot01);
			float u = (dot11*dot02 - dot01*dot12)*invDenom;
			float v = (dot00*dot12 - dot01*dot02)*invDenom;

			return ((u>0)&&(v>0)&&(u+v<1));    
		}
	};

	class b2Polygon 
	{
	public:
		const static signed int maxVerticesPerPolygon = 8;

		float * x;
		float * y;

		signed int nVertices;
		
		float area;
		bool areaIsSet;
		
		b2Polygon(float* _x, float* _y, signed int nVert);
		b2Polygon(mt::vec2f* v, signed int nVert);
		b2Polygon();
		~b2Polygon();
		
		float GetArea();
		
		void MergeParallelEdges(float tolerance);
		mt::vec2f* GetVertexVecs();
		b2Polygon(triangle& t);
		void Set(const b2Polygon& p);
		bool IsConvex();
		bool IsCCW();
		b2Polygon* Add(triangle& t);
	};

	const float b2_angularSlop = 2.0f / 180.0f * 3.14159265359f;

	static const float toiSlop = 0.0f;

	signed int remainder(signed int x, signed int modulus)
	{
		signed int rem = x % modulus;
		while (rem < 0)
		{
			rem += modulus;
		}
		return rem;
	}

	b2Polygon::b2Polygon(float* _x, float* _y, signed int nVert) 
	{
		nVertices = nVert;
		x = new float[nVertices];
		y = new float[nVertices];

		for (signed int i = 0; i < nVertices; ++i) 
		{
			x[i] = _x[i];
			y[i] = _y[i];
		}
		areaIsSet = false;
	}
		
	b2Polygon::b2Polygon(mt::vec2f* v, signed int nVert) 
	{
		nVertices = nVert;
		x = new float[nVertices];
		y = new float[nVertices];

		for (signed int i = 0; i < nVertices; ++i) 
		{
			x[i] = v[i].x;
			y[i] = v[i].y;

		}
		areaIsSet = false;
	}

	b2Polygon::b2Polygon() 
	{
		x = NULL;
		y = NULL;
		nVertices = 0;
		areaIsSet = false;
	}
		
	b2Polygon::~b2Polygon() 
	{
		delete[] x;
		delete[] y;
	}

	float b2Polygon::GetArea() 
	{
		if (areaIsSet) 
		{
			return area;
		}

		area = 0.0f;
		
		area += x[nVertices - 1] * y[0] - x[0] * y[nVertices - 1];

		for (int i = 0; i < nVertices - 1; ++i)
		{
			area += x[i] * y[i + 1] - x[i + 1] * y[i];
		}

		area *= .5f;
		areaIsSet = true;
		return area;
	}

	bool b2Polygon::IsCCW() 
	{
		return (GetArea() > 0.0f);
	}
		
	void b2Polygon::MergeParallelEdges(float tolerance)
	{
		if (nVertices <= 3) 
		{
			return; 
		}

		bool* mergeMe = new bool[nVertices];
		signed int newNVertices = nVertices;
		for (signed int i = 0; i < nVertices; ++i) 
		{
			signed int lower = (i == 0) ? (nVertices - 1) : (i - 1);
			signed int middle = i;
			signed int upper = (i == nVertices - 1) ? (0) : (i + 1);
			float dx0 = x[middle] - x[lower];
			float dy0 = y[middle] - y[lower];
			float dx1 = x[upper] - x[middle];
			float dy1 = y[upper] - y[middle];
			float norm0 = sqrtf(dx0 * dx0 + dy0 * dy0);
			float norm1 = sqrtf(dx1 * dx1 + dy1 * dy1);
			if ( !(norm0 > 0.0f && norm1 > 0.0f) && newNVertices > 3 )
			{
				mergeMe[i] = true;
				--newNVertices;
			}
			dx0 /= norm0; dy0 /= norm0;
			dx1 /= norm1; dy1 /= norm1;
			float cross = dx0 * dy1 - dx1 * dy0;
			float dot = dx0 * dx1 + dy0 * dy1;
			if (fabs(cross) < tolerance && dot > 0 && newNVertices > 3) {
				mergeMe[i] = true;
				--newNVertices;
			} else {
				mergeMe[i] = false;
			}
		}
		if(newNVertices == nVertices || newNVertices == 0)
		{
			delete[] mergeMe;
			return;
		}
		float* newx = new float[newNVertices];
		float* newy = new float[newNVertices];
		signed int currIndex = 0;
		for (int i = 0; i < nVertices; ++i) 
		{
			if (mergeMe[i] || newNVertices == 0 || currIndex == newNVertices) continue;
			assert(currIndex < newNVertices);
			newx[currIndex] = x[i];
			newy[currIndex] = y[i];
			++currIndex;
		}
		delete[] x;
		delete[] y;
		delete[] mergeMe;
		x = newx;
		y = newy;
		nVertices = newNVertices;
	}

	b2Polygon::b2Polygon(triangle& t) {
		nVertices = 3;
		x = new float[nVertices];
		y = new float[nVertices];
		for (signed int i = 0; i < nVertices; ++i) {
			x[i] = t.x[i];
			y[i] = t.y[i];
		}
	}
		
	void b2Polygon::Set(const b2Polygon& p) {
		if (nVertices != p.nVertices)
		{
			nVertices = p.nVertices;
			if (x) delete[] x;
			if (y) delete[] y;
			x = new float[nVertices];
			y = new float[nVertices];
		}
		
		for (signed int i = 0; i < nVertices; ++i)
		{
			x[i] = p.x[i];
			y[i] = p.y[i];
		}
		areaIsSet = false;
	}
		
	bool b2Polygon::IsConvex()
	{
		bool isPositive = false;
		for (int i = 0; i < nVertices; ++i) 
		{
			int lower = (i == 0) ? (nVertices - 1) : (i - 1);
			int middle = i;
			int upper = (i == nVertices - 1) ? (0) : (i + 1);
			float dx0 = x[middle] - x[lower];
			float dy0 = y[middle] - y[lower];
			float dx1 = x[upper] - x[middle];
			float dy1 = y[upper] - y[middle];
			float cross = dx0 * dy1 - dx1 * dy0;

			bool newIsP = (cross >= 0) ? true : false;

			if (i == 0)
			{
				isPositive = newIsP;
			}
			else if (isPositive != newIsP) 
			{
				return false;
			}
		}
		return true;
	}

	b2Polygon* b2Polygon::Add(triangle& t) 
	{
		signed int firstP = -1;
		signed int firstT = -1;
		signed int secondP = -1;
		signed int secondT = -1;
		for (signed int i = 0; i < nVertices; i++)
		{
			if (t.x[0] == x[i] && t.y[0] == y[i]) {
				if (firstP == -1) {
					firstP = i;
					firstT = 0;
				}
				else {
					secondP = i;
					secondT = 0;
				}
			}
			else if (t.x[1] == x[i] && t.y[1] == y[i]) {
				if (firstP == -1) {
					firstP = i;
					firstT = 1;
				}
				else {
					secondP = i;
					secondT = 1;
				}
			}
			else if (t.x[2] == x[i] && t.y[2] == y[i]) {
				if (firstP == -1) {
					firstP = i;
					firstT = 2;
				}
				else {
					secondP = i;
					secondT = 2;
				}
			}
			else {
			}
		}
		// Fix ordering if first should be last vertex of poly
		if (firstP == 0 && secondP == nVertices - 1) {
			firstP = nVertices - 1;
			secondP = 0;
		}
		
		// Didn't find it
		if (secondP == -1) {
			return NULL;
		}
		
		signed int tipT = 0;
		if (tipT == firstT || tipT == secondT)
			tipT = 1;
		if (tipT == firstT || tipT == secondT)
			tipT = 2;
		
		float* newx = new float[nVertices + 1];
		float* newy = new float[nVertices + 1];
		signed int currOut = 0;
		for (signed int i = 0; i < nVertices; i++) {
			newx[currOut] = x[i];
			newy[currOut] = y[i];
			if (i == firstP) {
				++currOut;
				newx[currOut] = t.x[tipT];
				newy[currOut] = t.y[tipT];
			}
			++currOut;
		}
		b2Polygon* result = new b2Polygon(newx, newy, nVertices+1);
		delete[] newx;
		delete[] newy;
		return result;
	}
		
	bool ResolvePinchPoint(const b2Polygon& pin, b2Polygon& poutA, b2Polygon& poutB)
	{
		if (pin.nVertices < 3) return false;
		float tol = .001f;
		bool hasPinchPoint = false;
		signed int pinchIndexA = -1;
		signed int pinchIndexB = -1;
		for (int i=0; i<pin.nVertices; ++i)
		{
			for (int j=i+1; j<pin.nVertices; ++j)
			{
				if (fabs(pin.x[i]-pin.x[j])<tol&&fabs(pin.y[i]-pin.y[j])<tol&&j!=i+1)
				{
					pinchIndexA = i;
					pinchIndexB = j;
					hasPinchPoint = true;
					break;
				}
			}
			if (hasPinchPoint) break;
		}
		if (hasPinchPoint){
			signed int sizeA = pinchIndexB - pinchIndexA;
			if (sizeA == pin.nVertices) return false;
			float* xA = new float[sizeA];
			float* yA = new float[sizeA];
			for (signed int i=0; i < sizeA; ++i){
				signed int ind = remainder(pinchIndexA+i,pin.nVertices);
				xA[i] = pin.x[ind];
				yA[i] = pin.y[ind];
			}
			b2Polygon tempA(xA,yA,sizeA);
			poutA.Set(tempA);
			delete[] xA;
			delete[] yA;
			
			signed int sizeB = pin.nVertices - sizeA;
			float* xB = new float[sizeB];
			float* yB = new float[sizeB];
			for (signed int i=0; i<sizeB; ++i){
				signed int ind = remainder(pinchIndexB+i,pin.nVertices);
				xB[i] = pin.x[ind];
				yB[i] = pin.y[ind];
			}
			b2Polygon tempB(xB,yB,sizeB);
			poutB.Set(tempB);
			delete[] xB;
			delete[] yB;
		}
		return hasPinchPoint;
	}

	bool IsEar(signed int i, float* xv, float* yv, signed int xvLength) 
	{
		float dx0, dy0, dx1, dy1;
		dx0 = dy0 = dx1 = dy1 = 0;
		if (i >= xvLength || i < 0 || xvLength < 3) {
			return false;
		}
		signed int upper = i + 1;
		signed int lower = i - 1;
		if (i == 0) {
			dx0 = xv[0] - xv[xvLength - 1];
			dy0 = yv[0] - yv[xvLength - 1];
			dx1 = xv[1] - xv[0];
			dy1 = yv[1] - yv[0];
			lower = xvLength - 1;
		}
		else if (i == xvLength - 1) {
			dx0 = xv[i] - xv[i - 1];
			dy0 = yv[i] - yv[i - 1];
			dx1 = xv[0] - xv[i];
			dy1 = yv[0] - yv[i];
			upper = 0;
		}
		else {
			dx0 = xv[i] - xv[i - 1];
			dy0 = yv[i] - yv[i - 1];
			dx1 = xv[i + 1] - xv[i];
			dy1 = yv[i + 1] - yv[i];
		}
		float cross = dx0 * dy1 - dx1 * dy0;
		if (cross > 0)
			return false;
		triangle myTri(xv[i], yv[i], xv[upper], yv[upper],
								  xv[lower], yv[lower]);
		for (signed int j = 0; j < xvLength; ++j) {
			if (j == i || j == lower || j == upper)
				continue;
			if (myTri.IsInside(xv[j], yv[j]))
				return false;
		}
		return true;
	}

	signed int TriangulatePolygon(float* xv, float* yv, signed int vNum, triangle* results) 
	{
		if (vNum < 3)
			return 0;

		b2Polygon pA,pB;
		b2Polygon pin(xv,yv,vNum);
		if (ResolvePinchPoint(pin,pA,pB)){
			triangle* mergeA = new triangle[pA.nVertices];
			triangle* mergeB = new triangle[pB.nVertices];
			signed int nA = TriangulatePolygon(pA.x,pA.y,pA.nVertices,mergeA);
			signed int nB = TriangulatePolygon(pB.x,pB.y,pB.nVertices,mergeB);
			if (nA==-1 || nB==-1){
				delete[] mergeA;
				delete[] mergeB;
				return -1;
			}
			for (signed int i=0; i<nA; ++i){
				results[i].Set(mergeA[i]);
			}
			for (signed int i=0; i<nB; ++i){
				results[nA+i].Set(mergeB[i]);
			}
			delete[] mergeA;
			delete[] mergeB;
			return (nA+nB);
		}

		triangle* buffer = new triangle[vNum-2];
		signed int bufferSize = 0;
		float* xrem = new float[vNum];
		float* yrem = new float[vNum];
		for (signed int i = 0; i < vNum; ++i) {
			xrem[i] = xv[i];
			yrem[i] = yv[i];
		}
		
		int xremLength = vNum;
		
		while (vNum > 3)
		{
			signed int earIndex = -1;
			float earMaxMinCross = -10.0f;

			for (signed int i = 0; i < vNum; ++i) 
			{
				if (IsEar(i, xrem, yrem, vNum)) 
				{
					signed int lower = remainder(i-1,vNum);
					signed int upper = remainder(i+1,vNum);
					mt::vec2f d1(xrem[upper]-xrem[i],yrem[upper]-yrem[i]);
					mt::vec2f d2(xrem[i]-xrem[lower],yrem[i]-yrem[lower]);
					mt::vec2f d3(xrem[lower]-xrem[upper],yrem[lower]-yrem[upper]);

					d1 = mt::norm_safe_v2(d1);
					d2 = mt::norm_safe_v2(d2);
					d3 = mt::norm_safe_v2(d3);
					
					float cross12 = fabs( pseudo_cross_v2(d1,d2) );
					float cross23 = fabs( pseudo_cross_v2(d2,d3) );
					float cross31 = fabs( pseudo_cross_v2(d3,d1) );
					//Find the maximum minimum angle
					float minCross = std::min(cross12, std::min(cross23,cross31));
					if (minCross > earMaxMinCross){
						earIndex = i;
						earMaxMinCross = minCross;
					}
				}
			}
			
			if (earIndex == -1){
				for (signed int i = 0; i < bufferSize; i++) {
					results[i].Set(buffer[i]);
				}
		
				delete[] buffer;
		
				if (bufferSize > 0) return bufferSize;
				else return -1;
			}
			
			--vNum;
			float* newx = new float[vNum];
			float* newy = new float[vNum];
			signed int currDest = 0;
			for (signed int i = 0; i < vNum; ++i) {
				if (currDest == earIndex) ++currDest;
				newx[i] = xrem[currDest];
				newy[i] = yrem[currDest];
				++currDest;
			}
			
			signed int under = (earIndex == 0) ? (vNum) : (earIndex - 1);
			signed int over = (earIndex == vNum) ? 0 : (earIndex + 1);
			triangle toAdd = triangle(xrem[earIndex], yrem[earIndex], xrem[over], yrem[over], xrem[under], yrem[under]);
			buffer[bufferSize].Set(toAdd);
			++bufferSize;
			
			delete[] xrem;
			delete[] yrem;
			xrem = newx;
			yrem = newy;
		}
		
		triangle toAdd = triangle(xrem[1], yrem[1], xrem[2], yrem[2],
								  xrem[0], yrem[0]);
		buffer[bufferSize].Set(toAdd);
		++bufferSize;
		
		delete[] xrem;
		delete[] yrem;
		
		assert(bufferSize == xremLength-2);
		
		for (signed int i = 0; i < bufferSize; i++) 
		{
			results[i].Set(buffer[i]);
		}
		
		delete[] buffer;
		
		return bufferSize;
	}

	signed int PolygonizeTriangles(triangle* triangulated, signed int triangulatedLength, std::vector<convexpoly2> & /* b2Polygon*/polys, signed int polysLength) 
	{
		signed int polyIndex = 0;
		
		if (triangulatedLength <= 0)
		{
			return 0;
		}
		else 
		{
			int* covered = new int[triangulatedLength];

			for (signed int i = 0; i < triangulatedLength; ++i)
			{
				covered[i] = 0;

				if ( ( (triangulated[i].x[0] == triangulated[i].x[1]) && (triangulated[i].y[0] == triangulated[i].y[1]) )
					 || ( (triangulated[i].x[1] == triangulated[i].x[2]) && (triangulated[i].y[1] == triangulated[i].y[2]) )
					 || ( (triangulated[i].x[0] == triangulated[i].x[2]) && (triangulated[i].y[0] == triangulated[i].y[2]) ) ) 
				{
					covered[i] = 1;
				}
			}
			
			bool notDone = true;
			while (notDone) 
			{
				signed int currTri = -1;
				for (signed int i = 0; i < triangulatedLength; ++i) 
				{
					if (covered[i])
					{
						continue;
					}

					currTri = i;
					break;
				}
				if (currTri == -1) 
				{
					notDone = false;
				}
				else 
				{
					b2Polygon poly(triangulated[currTri]);
					covered[currTri] = 1;
					signed int index = 0;
					for (signed int i = 0; i < 2*triangulatedLength; ++i,++index) 
					{
						while (index >= triangulatedLength)
						{
							index -= triangulatedLength;
						}

						if (covered[index]) 
						{
							continue;
						}
						b2Polygon* newP = poly.Add(triangulated[index]);
						if (!newP) {
							continue;
						}
						if (newP->nVertices > b2Polygon::maxVerticesPerPolygon)
						{
							delete newP;
							newP = NULL;
							continue;
						}
						if (newP->IsConvex())
						{ 
							poly.Set(*newP);
							delete newP;
							newP = NULL;
							covered[index] = 1;
						} 
						else 
						{
							delete newP;
							newP = NULL;
						}
					}

					if (polyIndex < polysLength)
					{
						poly.MergeParallelEdges(b2_angularSlop);
						if (poly.nVertices >= 3) 
						{
							//polys[polyIndex].Set(poly);
							polys.push_back(convexpoly2());
							
							for(int i = 0; i < poly.nVertices; i++)
							{
								polys.back().add_point(mt::vec2f(poly.x[i],poly.y[i]));
							}
							
							//polys.back().Set(poly);
						}
					}

					if (poly.nVertices >= 3) 
					{
						polyIndex++; 
					}
				}
			}
			delete[] covered;
		}
		return polyIndex;
	}

	signed int PolygonizeTriangles(triangle* triangulated, signed int triangulatedLength, std::vector<polygon> & /* b2Polygon*/polys, signed int polysLength) 
	{
		signed int polyIndex = 0;
		
		if (triangulatedLength <= 0)
		{
			return 0;
		}
		else 
		{
			int* covered = new int[triangulatedLength];

			for (signed int i = 0; i < triangulatedLength; ++i)
			{
				covered[i] = 0;

				if ( ( (triangulated[i].x[0] == triangulated[i].x[1]) && (triangulated[i].y[0] == triangulated[i].y[1]) )
					 || ( (triangulated[i].x[1] == triangulated[i].x[2]) && (triangulated[i].y[1] == triangulated[i].y[2]) )
					 || ( (triangulated[i].x[0] == triangulated[i].x[2]) && (triangulated[i].y[0] == triangulated[i].y[2]) ) ) 
				{
					covered[i] = 1;
				}
			}
			
			bool notDone = true;
			while (notDone) 
			{
				signed int currTri = -1;
				for (signed int i = 0; i < triangulatedLength; ++i) 
				{
					if (covered[i])
					{
						continue;
					}

					currTri = i;
					break;
				}
				if (currTri == -1) 
				{
					notDone = false;
				}
				else 
				{
					b2Polygon poly(triangulated[currTri]);
					covered[currTri] = 1;
					signed int index = 0;
					for (signed int i = 0; i < 2*triangulatedLength; ++i,++index) 
					{
						while (index >= triangulatedLength)
						{
							index -= triangulatedLength;
						}

						if (covered[index]) 
						{
							continue;
						}
						b2Polygon* newP = poly.Add(triangulated[index]);
						if (!newP) {
							continue;
						}
						if (newP->nVertices > b2Polygon::maxVerticesPerPolygon)
						{
							delete newP;
							newP = NULL;
							continue;
						}
						if (newP->IsConvex())
						{ 
							poly.Set(*newP);
							delete newP;
							newP = NULL;
							covered[index] = 1;
						} 
						else 
						{
							delete newP;
							newP = NULL;
						}
					}

					if (polyIndex < polysLength)
					{
						poly.MergeParallelEdges(b2_angularSlop);
						if (poly.nVertices >= 3) 
						{
							//polys[polyIndex].Set(poly);
							polys.push_back(polygon());
							
							for(int i = 0; i < poly.nVertices; i++)
							{
								polys.back().add_point(mt::vec2f(poly.x[i],poly.y[i]));
							}
							
							//polys.back().Set(poly);
						}
					}

					if (poly.nVertices >= 3) 
					{
						polyIndex++; 
					}
				}
			}
			delete[] covered;
		}
		return polyIndex;
	}
		
	void ReversePolygon(float* x, float* y, int n)
	{
		if (n == 1)
		{
			return;
		}

		signed int low = 0;
		signed int high = n - 1;

		while (low < high)
		{
			float buffer = x[low];
			x[low] = x[high];
			x[high] = buffer;
			buffer = y[low];
			y[low] = y[high];
			y[high] = buffer;
			++low;
			--high;
		}
	}

	static const int maxPolys = 10;

/*	int decompose_concave(std::vector<mt::vec2f> & points, std::vector<mt::convexpoly2> & results)
	{
		if(points.empty())
		{
			return 0;
		}

		mt::b2Polygon p(&points[0],points.size());

		if (p.nVertices < 3) 
		{
			results.push_back(convexpoly2());
							
			for(int i = 0; i < p.nVertices; i++)
			{
				results.back().add_point(mt::vec2f(p.x[i],p.y[i]));
			}

			return p.nVertices;
		}

		triangle * triangulated = new triangle[p.nVertices - 2];

		int nTri = 0;

		if (p.IsCCW())
		{
			b2Polygon tempP;
			tempP.Set(p);
			ReversePolygon(tempP.x, tempP.y, tempP.nVertices);
			nTri = TriangulatePolygon(tempP.x, tempP.y, tempP.nVertices, triangulated);
		}
		else 
		{
			nTri = TriangulatePolygon(p.x, p.y, p.nVertices, triangulated);
		}

		if (nTri < 1) 
		{
			return -1;
		}

		signed int nPolys = PolygonizeTriangles(triangulated, nTri, results, maxPolys);
		delete[] triangulated;
		return nPolys;
	}*/

	int decompose_concave(std::vector<mt::vec2f> & points, std::vector<mt::polygon> & results)
	{
		if(points.empty())
		{
			return 0;
		}

		mt::b2Polygon p(&points[0],points.size());

		if (p.nVertices < 3) 
		{
			results.push_back(polygon());
							
			for(int i = 0; i < p.nVertices; i++)
			{
				results.back().add_point(mt::vec2f(p.x[i],p.y[i]));
			}

			return p.nVertices;
		}

		triangle * triangulated = new triangle[p.nVertices - 2];

		int nTri = 0;

		if (p.IsCCW())
		{
			b2Polygon tempP;
			tempP.Set(p);
			ReversePolygon(tempP.x, tempP.y, tempP.nVertices);
			nTri = TriangulatePolygon(tempP.x, tempP.y, tempP.nVertices, triangulated);
		}
		else 
		{
			nTri = TriangulatePolygon(p.x, p.y, p.nVertices, triangulated);
		}

		if (nTri < 1) 
		{
			return -1;
		}

		signed int nPolys = PolygonizeTriangles(triangulated, nTri, results, maxPolys);
		delete[] triangulated;
		return nPolys;
	}
}