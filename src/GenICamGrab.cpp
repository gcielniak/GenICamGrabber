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
	GenICamera camera, camera2;

	ImageViewer viewer("Viewer 1"), viewer2("Viewer 2");

	camera.Connect(viewer);
	camera2.Connect(viewer2);

	try {
		camera2.Init();
		camera2.SetCamera(1);

		camera.Start();
		camera2.Start();

		while (viewer.IsRunning() && viewer2.IsRunning());

		camera.Stop();
		camera2.Stop();
	}
	catch (LibException& e)	{
		cerr << e.what() << endl;
		return 0;
	}

	return 0;
}
