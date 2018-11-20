#include "App.h"

int main(int argc, char *argv[])
{
	App app;

	while (app.isActive())
	{
		app.loop();
	}

	return 1;
}