#ifndef _GAME_OBJECT_DEFINED
#define _GAME_OBJECT_DEFINED

class GameObject
{
public:
	GameObject();
	~GameObject();

	// Getters
	double getX() { return x; }
	double getY() { return y; }
	double getWidth() { return width; }
	double getHeight() { return height; }

	// Setters
	void setX(double x) { this->x = x; }
	void setY(double y) { this->y = y; }
	void setWidth(double width) { this->width = width; }
	void setHeight(double height) { this->height = height; }

protected:
	double x, y, width, height;

};

#endif

