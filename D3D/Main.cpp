#include "Base.h"

int WINAPI wWinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPWSTR lpCmdLine, int nShowCmd)
{
	if (!Renderer3D::Initialize(hInstance))
		return -1;
	
	Renderer3D::Run();
	Renderer3D::Shutdown();

	return 0;
}