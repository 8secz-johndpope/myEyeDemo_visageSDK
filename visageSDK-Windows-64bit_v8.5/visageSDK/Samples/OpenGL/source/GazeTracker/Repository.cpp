#include "Repository.h"
#include <direct.h>

Repository::Repository()
{
	calibrator = new ScreenSpaceGazeRepository();
}

void Repository::ReadFromFile(char *filename)
{
	FILE* f = fopen(filename, "r");
	
	char name[100];
	_getcwd(name, 100);

	char input[300];
	while(fgets(input, 300, f))
	{
		int idx = 0;
		float x = 0; float y = 0;
		sscanf(input, "%d %f %f", &idx, &x, &y);

		ScreenSpaceGazeData* data = new ScreenSpaceGazeData();
		data->x = x;
		data->y = y;
		data->index = idx;
		data->inState = 1;

		calibrator->Add(data);
	}
	fclose(f);
}

int Repository::GetCount()
{
	return calibrator->GetCount();
}

ScreenSpaceGazeData* Repository::Get(int index)
{
	return calibrator->Get(index);
}