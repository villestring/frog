/*
 * This enhanced version let user to press 'n' to restart the game
 *  from level 1, and all blocks at initial position.
 */

#include <cstdlib>

#include <unistd.h>
#include <sys/time.h>
#include <math.h>

#include <iostream>
#include <list>
#include <sstream>
#include <string>
#include <X11/Xlib.h>
#include <X11/Xutil.h>

using namespace std;



// get microseconds
unsigned long now() {
	timeval tv;
	gettimeofday(&tv, NULL);
	return tv.tv_sec * 1000000 + tv.tv_usec;
}

// x globals
struct XInfo {
	Display * display;
	Window window;
	GC gc;
};

XInfo xinfo;

// Default frame per second is set to 30
int FPS = 30;

// abstract class of displayable things
class Displayable {
public:
	virtual void paint(XInfo& xinfo) = 0;
};

// text displayable
class Text : public Displayable {
	int x;
	int y;
	string s; 

public:
	virtual void paint(XInfo& xinfo) {
		GC gc = XCreateGC(xinfo.display, xinfo.window, 0, 0);
		int screen = DefaultScreen(xinfo.display);
		XSetForeground(xinfo.display, gc, BlackPixel(xinfo.display, screen));
		XSetBackground(xinfo.display, gc, WhitePixel(xinfo.display, screen));
		XSetFillStyle(xinfo.display, gc, FillSolid);
		XDrawImageString( xinfo.display, xinfo.window, gc,
			this->x, this->y, this->s.c_str(), this->s.length() );
	}
	Text(int x, int y, string s) : x(x), y(y), s(s) {}
	void changeLv(int Lv) {
		ostringstream convert;
		convert << Lv;
		this->s = "Level: " + convert.str();
	}
};

// rectangle
class Rectangle : public Displayable {
	int x;
	int y;
	unsigned int width;
	unsigned int height;

public:
	virtual void paint(XInfo& xinfo) {
		GC gc = XCreateGC(xinfo.display, xinfo.window, 0, 0);
		int screen = DefaultScreen(xinfo.display);
		XSetForeground(xinfo.display, gc, BlackPixel(xinfo.display, screen));
		XSetBackground(xinfo.display, gc, WhitePixel(xinfo.display, screen));
		XSetFillStyle(xinfo.display, gc, FillSolid);
		XSetLineAttributes(xinfo.display, gc, 0, LineSolid, CapButt, JoinRound);

		// draw the rectangle
		XFillRectangle(xinfo.display, xinfo.window, gc, x, y, width, height);
	}
	int getXPos(void) {
		return this->x;
	}
	int getYPos(void) {
		return this->y;
	}
	int getWidth(void) {
		return this->width;
	}
	void updateX(int xChange) {
		this->x += xChange;
	}
	void updateY(int yChange) {
		this->y += yChange;
	}
	void setPos(int newX, int newY) {
		this->x = newX;
		this->y = newY;
	}
	Rectangle(int x, int y, int width, int height) : x(x), y(y), width(width), height(height) {}

};

/*
 * Function to repaint the display list
 */

void repaint( list<Displayable*> dList, XInfo& xinfo) {
	//cout << "painting components" <<endl;

	list<Displayable*>::const_iterator begin = dList.begin();
	list<Displayable*>::const_iterator end = dList.end();
	// cout << dList.size() << endl;

	XClearWindow(xinfo.display, xinfo.window);
	while(begin != end) {
		Displayable* d = *begin;
		//cout << "attempt to paint" <<endl;			
		d->paint(xinfo);	
		begin++;
		//cout << "painted" <<endl;
	}
	XFlush(xinfo.display);
	//cout << "all components painted" <<endl;
}

/*
 * Function that test if the frog hits a block
 */
bool isHit(Rectangle* block, Rectangle* frog) {
	int blockLeft = block->getXPos();
	int blockRight = blockLeft + block->getWidth();

	int frogLeft = frog->getXPos();
	int frogRight = frogLeft + 50;
	return ((blockLeft <= frogLeft && blockRight >= frogLeft) || (frogLeft <= blockLeft && frogRight >= blockLeft));
}


/*
 * output the error message
 */
void error(string str) {
	cerr << str << endl;
	exit(0);
}




int main(int argc, char* argv[]) {

	// change FPS if needed.
	if (argc >= 2) {
		// cout << "arg is " << argv[1] <<endl;
		stringstream ss(argv[1]);

		ss >> FPS;
	}
	// cout << FPS << endl;

	// initialize window
	xinfo.display = XOpenDisplay("");
	if (!xinfo.display) {
		error("Can't open display.");
	}

	int screen = DefaultScreen(xinfo.display);

	unsigned long white, black;
	white = XWhitePixel(xinfo.display, screen);
	black = XBlackPixel(xinfo.display, screen);

  	xinfo.window = XCreateSimpleWindow(
    	xinfo.display,       // display where window appears
    	DefaultRootWindow( xinfo.display ), // window's parent in window tree
    	10, 10,                  // upper left corner location
    	850, 250,                  // size of the window
    	5,               // width of window's border
    	black,           // window border colour
    	white );             // window background colour
  	// extra window properties like a window title
 	XSetStandardProperties(
   		xinfo.display,    // display containing the window
  		xinfo.window,   // window whose properties are set
  		"Frog",  // window's title
    	"FG",       // icon's title
    	None,       // pixmap for the icon
    	argv, argc,     // applications command line args
    	None );         // size hints for the window


 	XSelectInput(xinfo.display, xinfo.window, KeyPressMask);

 	XMapRaised(xinfo.display, xinfo.window);

 	XFlush(xinfo.display);

 	sleep(1);

 	XEvent event;


 	list<Displayable*> dList;



 	// list of Displayables
 	// make one extra Rectangle in each line to give the allusion of
 	//  "neverending loop"
 	// first line
 	int inter1 = 850/3;
 	Rectangle* row11 = new Rectangle(0, 50, 50, 50);
 	Rectangle* row12 = new Rectangle(283, 50, 50, 50);
 	Rectangle* row13 = new Rectangle(567, 50, 50, 50);
 	Rectangle* row14 = new Rectangle(-283, 50, 50, 50);
 	// second line
 	int inter2 = 850/4;
 	Rectangle* row21 = new Rectangle(0, 100, 20, 50);
 	Rectangle* row22 = new Rectangle(212, 100, 20, 50);
 	Rectangle* row23 = new Rectangle(425, 100, 20, 50);
 	Rectangle* row24 = new Rectangle(638, 100, 20, 50);
 	Rectangle* row25 = new Rectangle(850, 100, 20, 50);
 	// third line
 	int inter3 = 850/2;
 	Rectangle* row31 = new Rectangle(0, 150, 100, 50);
 	Rectangle* row32 = new Rectangle(425, 150, 100, 50);
 	Rectangle* row33 = new Rectangle(-425, 150, 100, 50);
 	dList.push_back(row11);
 	dList.push_back(row12);
 	dList.push_back(row13);
 	dList.push_back(row14);
 	dList.push_back(row21);
 	dList.push_back(row22);
 	dList.push_back(row23);
 	dList.push_back(row24);
 	dList.push_back(row25);
 	dList.push_back(row31);
 	dList.push_back(row32);
 	dList.push_back(row33);

 	 // level
 	int lv = 1;
 	stringstream ss;
 	ss << lv;
 	string textTemp = "Level: " + ss.str();
 	Text* level = new Text(700, 10, textTemp);
 	dList.push_back(level);
 	
  	// frog initial position
 	Rectangle* frog = new Rectangle(400, 200, 50, 50);
 	dList.push_back(frog);

 	repaint(dList,xinfo);
 	XFlush(xinfo.display);


 	// gc for drawing
 	xinfo.gc = XCreateGC (xinfo.display, xinfo.window, 0, 0);

 	// time of last xinfo.window paint
 	unsigned long lastRepaint = 0;

 	XWindowAttributes w;
 	XGetWindowAttributes(xinfo.display, xinfo.window, &w);

 	// eventloop
 	while(true) {
 		// cout << "XPending: " << XPending(xinfo.display) << endl;
 		// cout << lastRepaint <<endl;

 		bool hitted = false;
 		// yPos of the frog
 		int fY = frog->getYPos();
 		if (fY == 50) {
 			hitted = isHit(row11, frog) || isHit(row12, frog) ||
 					 isHit(row13, frog) || isHit(row14, frog);
 		}
 		else if (fY == 100) {
 			hitted = isHit(row21, frog) || isHit(row22, frog) ||
 					 isHit(row23, frog) || isHit(row24, frog) ||
 					 isHit(row25, frog);
 		}
 		else if (fY == 150) {
 			hitted = isHit(row31, frog) || isHit(row32, frog) ||
 					 isHit(row33, frog);
 		}

 		if (hitted) {
 			lv = 1;
 			frog->setPos(400, 200);
 			level->changeLv(1);
 		}

 		if (XPending(xinfo.display) > 0) {
 			XNextEvent(xinfo.display, &event);
 			KeySym key;
 			char text[10];
 			int i = XLookupString ((XKeyEvent*)&event, text, 10, &key, 0);
 			// "q" to quit
 			if (i==1 && text[0] == 'q') {
 				XCloseDisplay(xinfo.display);
 				exit(0);
 			}
 			if (i==1 && text[0] == 'n') {
 				if (frog->getYPos() == 0) {
 					lv++;

 					level->changeLv(lv);

 					frog->setPos(400, 200);
 				}
 			}

 			// "r" to restart the game
 			if (i==1 && text[0] == 'r') {
 				lv = 1;
 				frog->setPos(400, 200);
 				level->changeLv(1);
 				row11->setPos(0, 50);
 				row12->setPos(283, 50);
 				row13->setPos(567, 50);
 				row14->setPos(-283, 50);
 				row21->setPos(0, 100);
 				row22->setPos(212, 100);
 				row23->setPos(425, 100);
 				row24->setPos(638, 100);
 				row25->setPos(850, 100);
 				row31->setPos(0, 150);
 				row32->setPos(425, 150);
 				row33->setPos(-425, 150);

 			}
 			switch(key){
 				case XK_Left:
 				//cout << "left" <<endl;
 					if (!(frog->getXPos() < 50)) {
 						frog->updateX(-50);
 						repaint(dList,xinfo);
	 				}
	 				break;
	 			case XK_Right:
	 			//cout << "right" <<endl;
	 				if (!(frog->getXPos() >= 800)) {
	 					frog->updateX(50);
	 					repaint(dList,xinfo);
	 				}
	 				break;
	 			case XK_Up:
	 			//cout << "up" <<endl;
	 				if (!(frog->getYPos() < 50)) {
	 					frog->updateY(-50);
	 					repaint(dList,xinfo);	
	 				}
	 				break;
	 			case XK_Down:
	 			//cout << "down" <<endl;
	 				if (!((frog->getYPos() < 50) || (frog->getYPos() >= 200))) {

	 					frog->updateY(50);
	 					repaint(dList,xinfo);
	 				}
	 				break;
 			}
 			
 		}

 		unsigned long end = now();

 		if (end - lastRepaint > 1000000 / FPS) {
 			XClearWindow(xinfo.display, xinfo.window);

 			// update first line
 			row11->updateX(lv);
 			if(row11->getXPos() >= 850) {
 				row11->setPos(row11->getXPos() - 850 - inter1, 50);
 			}
 			row12->updateX(lv);
 			if(row12->getXPos() >= 850) {
 				row12->setPos(row12->getXPos() - 850 - inter1, 50);
 			}
 			row13->updateX(lv);
 			if(row13->getXPos() >= 850) {
 				row13->setPos(row13->getXPos() - 850 - inter1, 50);
 			}
 			row14->updateX(lv);
 			if(row14->getXPos() >= 850) {
 				row14->setPos(row14->getXPos() - 850 - inter1, 50);
 			}

 			// update second line
 			row21->updateX(-lv);
 			if (row21->getXPos() <= -20) {
 				row21->setPos(850 + inter2 - row21->getXPos(), 100);
 			}
 			row22->updateX(-lv);
 			if (row22->getXPos() <= -20) {
 				row22->setPos(850 + inter2 - row22->getXPos(), 100);
 			}
 			row23->updateX(-lv);
 			if (row23->getXPos() <= -20) {
 				row23->setPos(850 + inter2 - row23->getXPos(), 100);
 			}
 			row24->updateX(-lv);
 			if (row24->getXPos() <= -20) {
 				row24->setPos(850 + inter2 - row24->getXPos(), 100);
 			}
 			row25->updateX(-lv);
 			if (row25->getXPos() <= -20) {
 				row25->setPos(850 + inter2 - row25->getXPos(), 100);
 			}

 			// update third line
 			row31->updateX(lv);
 			if (row31->getXPos() >= 850) {
 				row31->setPos(row31->getXPos() - 850 - inter3, 150);
 			}
 			row32->updateX(lv);
 			if (row32->getXPos() >= 850) {
 				row32->setPos(row32->getXPos() - 850 - inter3, 150);
 			}
 			row33->updateX(lv);
 			if (row33->getXPos() >= 850) {
 				row33->setPos(row33->getXPos() - 850 - inter3, 150);
 			}

 			repaint(dList,xinfo);
 			XFlush(xinfo.display);
 			lastRepaint = now();
 		}

 		 if (XPending(xinfo.display) == 0) {
 		 	usleep(1000000/FPS - (end-lastRepaint));
 		 }
 	}
 	XCloseDisplay(xinfo.display);
}











