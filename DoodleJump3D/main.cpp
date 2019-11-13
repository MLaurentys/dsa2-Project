// Include standard headers
#include "Main.h"

int main(void)
{
	ApplicationManager* app = new ApplicationManager();
    app->Run("Test", Simplex::RES_C_1280x720_16x9_HD, false, false);
    delete app;
	return 0;
}