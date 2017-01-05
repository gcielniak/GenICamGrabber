/************************************************************************
	STEMMER IMAGING GmbH
 ------------------------------------------------------------------------
  Program  :  VCGenICamGrabConsoleExample.exe
  Author   :
  Date     :  12/2009
  Purpose  :  C++ Console example which shows how easy it is to Grab Images
			  with the CVB Grab2 Interface and the GenICam vin driver.

  Revision :  1.1
  Updates  :  07/2010 CKS Adapted to CVB 2010
			  02/2011 SDo Added -scan parameter
 ************************************************************************/

 //#include <Windows.h>
#include "ImageViewer.h"
#include "GenICamera.h"

using namespace std;

int main(int argc, char* argv[])
{
	GenICamera camera;

	ImageViewer viewer("IViewer");

	camera.Connect(viewer);

	try {
		camera.Start();

		while (viewer.IsRunning());

		camera.Stop();
	}
	catch (LibException& e)	{
		cerr << e.what() << endl;
		return 0;
	}

	return 0;
}
