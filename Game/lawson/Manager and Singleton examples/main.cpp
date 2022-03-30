
#include <iostream>
#include <string>

#include <Trackable.h>
#include <MemoryTracker.h>

#include "WidgetManager.h"
#include "Widget.h"
#include "Singleton.h"
using namespace std;
int main()
{
	
	//Singleton::init();
	
	Singleton* ptr = Singleton::getInstance();
	cout << ptr->getString() << " " << ptr->getInt() << endl;

	WidgetManager* pWidgetManager = new WidgetManager;

	pWidgetManager->createAndManageWidget( "PI", 3.14f );
	pWidgetManager->createAndManageWidget( "Square Root of 2", 1.414f );
	Widget* pSine = pWidgetManager->createAndManageWidget( "sine of 45", 0.707f );
	pWidgetManager->createAndManageWidget( "tangent of 30", 0.577f );

	cout << "PI " << pWidgetManager->getWidget("PI")->getContents() << endl;

	pWidgetManager->deleteWidget( pSine );
	pWidgetManager->deleteWidget("tangent of 30");
	delete pWidgetManager;
	
	
	//Singleton::cleanup();
	
	MemoryTracker::getInstance()->reportAllocations(std::cout);
	system("pause");

	return 0;
}