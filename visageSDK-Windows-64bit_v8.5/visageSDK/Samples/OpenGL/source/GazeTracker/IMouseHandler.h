#ifndef MOUSEHANDLER_H
#define MOUSEHANDLER_H

class IMouseHandler
{
	public:
		virtual void OnLeftButtonDown(float x, float y) = 0;
};

#endif