#include <iostream>
#include <string>
#include <stdexcept>
#include <cmath>
#include <ostream>

using namespace std;

const int screenWidth = 800;
const int screenHeight = 600;

class Point2d
{
private: 
	int x;
	int y;

public:
	//def constructor
	Point2d() : Point2d(0,0) {}
	//main constructor linked to setters 
	Point2d(int x, int y)
	{
        setX(x);
        setY(y);

	}

	int getX() const { return x; }

	int getY() const { return y; }

    void setX(int x)
	{
        if (x < 0 || x >= screenWidth )
		{
			throw invalid_argument("Координаты должны быть внутри окна (начало координат левый нижний угол); Point2dX");
		}

		this->x = x;
    }

    void setY(int y) 
	{
		if (y < 0 || y >= screenHeight)
		{
			throw invalid_argument("Координаты должны быть внутри окна (начало координат левый нижний угол);  Point2dY");
		}
		this->y = y;
    }

	bool operator==(const Point2d& other) const {
		return x == other.x && y == other.y; 
	}

	bool operator!=(const Point2d& other) const {
		return !(*this == other);
	}

	string pointToString() const
	{
		return "point(x=" + to_string(x) + ", y=" + to_string(y) + ")";
	}

	friend ostream& operator<<(ostream& os, const Point2d& p)
	{
		return os << p.pointToString();
	}

};


class Vector2d
{
private:
	int x;
	int y;

public:
	//def constructor linked to main constructor
	Vector2d() : Vector2d(0,0) {}
	     
	//constructor by points linked to setters
	Vector2d(Point2d headPoint, Point2d endPoint)
	{
		x = headPoint.getX() - endPoint.getX();
		setCoordX(x);

		y = headPoint.getY() - endPoint.getY();
		setCoordY(y);
	}

	//main constructor linked to setters
	Vector2d(int x, int y)
	{
		setCoordX(x);
		setCoordY(y);

	}

	void setCoordX(int x) { 
		if (x <= 0 || x >= screenWidth)
		{
			throw invalid_argument("Координаты должны быть внутри окна (начало координат левый нижний угол); Vecotr2dX ");
		}
		
		this->x = x;
	}
	void setCoordY(int y) { 
		if (y <= 0 || y >= screenHeight)
		{
 			throw invalid_argument("Координаты должны быть внутри окна (начало координат левый нижний угол); Vecotr2dY");
		}

		this->y = y;
	 }
	
	int getCoordX() const { return x; }
	int getCoordY() const { return y; }

	double lenght() const 
	{
		return sqrt(pow(2, x) + pow(2, y));
	}
	
	int dotProduct(const Vector2d& other) const
	{
		return x * other.x + y * other.y;
	}

	int crossProduct(const Vector2d& other) const
	{
		return x * other.y - other.x * y;
	}

	int mixedProduct(Vector2d& firVec, Vector2d& secVec, Vector2d& thirVec) const
	{
		//voprosiki
		return 0;
	}

	Vector2d operator+(const Vector2d& other) const
	{
		return Vector2d(x + other.x, y + other.y);
	}

	Vector2d operator-(const Vector2d& other) const
	{
		return Vector2d(x - other.x, y - other.y);
	}

	string vectorToString() const
	{
		return "vector(x= "+ to_string(x) + ", y= " + to_string(y) + ")";
	}
	
	friend ostream& operator<<(ostream& os, const Vector2d& v) {
	return os << v.vectorToString();
}

	bool operator==(const Vector2d& other) const {
		return x == other.x && y == other.y; 
	}

	bool operator!=(const Vector2d& other) const {
		return!(*this == other);
	}

	Vector2d operator*(int k) const
	{
		return Vector2d(x * k, y * k);
	}
};

int main()
{
	setlocale(LC_ALL, "Russian");

	try{
		Point2d point(300, 200);
		cout << point << endl;
		Vector2d coordsVector(50, 50);
		cout << coordsVector << endl;
	}
	catch (const invalid_argument& e) {
		cerr << e.what() << endl;
	}

	Point2d headPoint(200, 300);
	Point2d endPoint(15, 50);

	Vector2d pointVector(headPoint, endPoint);
	Vector2d secCoordsVector(8, 10);
	
	cout << "Вектор по двум точкам: " << pointVector << endl;
	cout << "Длинна вектора по двум точкам: " << pointVector.lenght() << endl;
	cout << "Вектор по координатам: " << secCoordsVector << endl;
	cout << "Длинна вектора по координатам: " << secCoordsVector.lenght() << endl;
	
	pointVector.setCoordX(100); 
	secCoordsVector.setCoordY(30);

	cout << "Координата x вектора по двум точкам: " << pointVector.getCoordX() << endl;
	cout << "Координата y вектора по координатам: " << secCoordsVector.getCoordY() << endl;
	
	cout << "Скалярное произведение: " << pointVector.dotProduct(secCoordsVector) << endl;
	cout << "Векторное произведение: " << secCoordsVector.crossProduct(pointVector) << endl;
	
	Vector2d summ = secCoordsVector + pointVector;
	Vector2d remainder = pointVector - secCoordsVector;
	
	cout << "Вектор суммы: " << summ << endl;
	cout << "Вектор разности: " << remainder << endl;

	Point2d somePoint(200, 300);
	Point2d anotherPoint(500, 400);
	Point2d samePoint (200, 300);

	cout << "Проверка isEqual: " << (somePoint == anotherPoint) << endl;
	cout << "Проверка isEqual: " << (somePoint == samePoint) << endl;

	cout << "Проверка isNotEqual: " << (somePoint != anotherPoint) << endl;
	cout << "Проверка isNotEqual: " << (somePoint != samePoint) << endl;

	Vector2d coordVecSome(200,300);
	Vector2d coordVecSame(200,300);
	Vector2d coordVecAnother(400,500);

	cout << "Проверка isEqual Vector2d: " << (coordVecAnother == coordVecSome) << endl;
	cout << "Проверка isEqual Vector2d: " << (coordVecSome == coordVecSame) << endl;

	cout << "Проверка isNotEqual Vector2d: " << (coordVecAnother != coordVecSome) << endl;
	cout << "Проверка isNotEqual Vecotr2d: " << (coordVecSame != coordVecSome) << endl;


	Point2d firstPoint(100, 200);
	Point2d secondPoint(50, 30);
	Point2d thirdPoint(300, 150);
	Point2d fourthPoint(10,44);
	Vector2d pointVecSome(firstPoint, secondPoint);
	Vector2d pointVecSame(firstPoint, secondPoint);
	Vector2d pointVecAnother(thirdPoint, fourthPoint);

	cout  << "Проверка isEqual Vector2d point: " << (pointVecSome == pointVecAnother) << endl;
	cout  << "Проверка isEqual Vector2d point: " << (pointVecSome == pointVecSame) << endl;
	
	cout  << "Проверка isNotEqual Vector2d point: " << (pointVecSome != pointVecAnother) << endl;
	cout  << "Проверка isEqual Vector2d point: " << (pointVecSome != pointVecSame) << endl;
}
