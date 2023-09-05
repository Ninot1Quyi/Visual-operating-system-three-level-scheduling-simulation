#include "OS_View.h"
#include <QApplication>

//-----------------------


int main(int argc,char*argv[])
{


	QApplication a(argc,argv);

	OS_View w;
 
	w.setGeometry(15,50,1500,700); 

	w.show();

	return a.exec();
}

