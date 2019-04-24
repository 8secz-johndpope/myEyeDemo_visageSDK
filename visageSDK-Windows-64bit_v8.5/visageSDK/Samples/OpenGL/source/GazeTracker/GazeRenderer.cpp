#include "GazeRenderer.h"
#include <sys/timeb.h>

GazeRenderer::GazeRenderer(void)
{
	inRecalibSequence = false;
	recalibSequenceEnd = false;
	window = 0;
	setRenderingContext = false;
}

GazeRenderer::~GazeRenderer(void)
{
}

void GazeRenderer::SetDrawWindow(Window* window)
{
	this->window = window;
	window->SetRenderingContext();
	window->Show();
}

void GazeRenderer::Redraw(ScreenSpaceGazeData* gazeData, FaceData* data, VsImage* image, int trackingStatus)
{
	POINT mousePos;
	GetCursorPos(&mousePos);

	if(image == 0)
	{
		return;
	}

	if(!setRenderingContext && image != 0)
	{
		width = GetSystemMetrics(SM_CXSCREEN);
		height = GetSystemMetrics(SM_CYSCREEN);

		initFrameTexture(image);

		glViewport(0,0,width, height);	

		setRenderingContext = true; 
	}
	else
	{	
		glClear(GL_COLOR_BUFFER_BIT);

		GLdouble modelview[16];
		GLdouble projection[16];
		GLint viewport[4];
	
		glGetDoublev(GL_MODELVIEW_MATRIX, modelview);
		glGetDoublev(GL_PROJECTION_MATRIX, projection);
		glGetIntegerv(GL_VIEWPORT, viewport);
	
		float z;
 
		glReadPixels(mousePos.x,viewport[3] - mousePos.y, 1, 1, GL_DEPTH_COMPONENT, GL_FLOAT, &z);

		GLdouble glx, gly, glz;
			
		gluUnProject(mousePos.x, viewport[3] - mousePos.y, z, modelview, projection, viewport, &glx, &gly, &glz);

		glMousePosX = glx;
		glMousePosY = gly;

		if(trackingStatus != TRACK_STAT_OFF)
		{			
			setFrameTexture(image);
			DrawImage();
		}

		if(trackingStatus == TRACK_STAT_OK)
		{
			glMatrixMode(GL_PROJECTION);
			glPushMatrix();
			glLoadIdentity();									// Reset The Projection Matrix

			glOrtho(0.0f,4.0f,0.0f,4.0f,-10.0f,10.0f);

			glMatrixMode(GL_MODELVIEW);
			glPushMatrix();
			glLoadIdentity();									// Reset The ModelView Matrix
			glTranslatef(1.5f, 1.5f, 0.0f);
			glLineWidth(2);

			drawFaceFeatures(data->featurePoints2D);
			
			SetupFrustum(image->width, image->height, data->cameraFocus);
			drawGazeDirection(data->gazeDirectionGlobal, data->featurePoints3D);
		}

		if(trackingStatus != 1)
		{
			std::string displayText;
			displayText = "Wait for tracker to initalize. Please look straight into camera. Gaze tracking can be started after tracker initializes";
			int len = displayText.length();
			int size = glutBitmapWidth(GLUT_BITMAP_TIMES_ROMAN_24, 'I');
			DisplayText((char*)displayText.c_str(),  -1.0f + 2.0f * ((float)(width / 2) - (len * size / 2)) / (float)width, -0.9);
		}

		if(trackingStatus == 1)
		{
			std::string displayText;
			displayText = "Tracker initialized. Press SPACE to start gaze tracking";
			int len = displayText.length();
			int size = glutBitmapWidth(GLUT_BITMAP_TIMES_ROMAN_24, 'I');
			DisplayText((char*)displayText.c_str(), -1.0f + 2.0f * ((float)(width / 2) - (len * size / 2)) / (float)width, -0.9);
		}

		if(recalibSequenceEnd)
		{
			DisplayGazeData(*gazeData);
		}

		DisplayGazeData(*gazeData);

		std::string displayText;
		std::ostringstream ssx, ssy;
		ssx << data->gazeDirection[0] * 180.0f / 3.14159f; 
		ssy << data->gazeDirection[1] * 180.0f / 3.14159f;
		displayText = "x = " + ssx.str()  + ", y = " + ssy.str();
		int len = displayText.length();
		int size = glutBitmapWidth(GLUT_BITMAP_TIMES_ROMAN_24, 'I');
		DisplayText((char*)displayText.c_str(), -1.0f + 2.0f * ((float)(width / 2) - (len * size / 2)) / (float)width, -0.75);
		DisplayText((char*)displayText.c_str(), -1.0f + 2.0f * ((float)(width / 2) - (len * size / 2)) / (float)width, 0.75);

		if(inRecalibSequence)
		{
			struct _timeb end;
			_ftime(&end);
			timestamp = end.time * 1000 + end.millitm;
			int counter = timestamp - recalibTimestamp;
			std::string displayText;
				
			if(counter < 1000)
			{
				displayText = "3";
			}

			if(counter > 1000 && counter < 2000)
			{
				displayText = "2";
			}

			if(counter > 2000 && counter < 3000)
			{
				displayText = "1";
			}

			if(counter > 3000)
			{
				inRecalibSequence = false;
					
				glMatrixMode(GL_PROJECTION);
				glLoadIdentity();
				glOrtho(0, width, 0, height, 0.1f, 100);
				recalibSequenceEnd = true;
			}
	
			int len = displayText.length();
			int size = glutBitmapWidth(GLUT_BITMAP_TIMES_ROMAN_24, 'I');
			DisplayText((char*)displayText.c_str(), -1.0f + 2.0f * ((float)(width / 2) - (len * size / 2)) / (float)width, 0);
		}	

		if(window->recalibStarted)
		{
			RecalibStarted(trackingStatus);
			window->recalibStarted = false;
		}

		window->Swap();
	}	
}

void GazeRenderer::DisplayText(char* displayText, float x, float y)
{
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
	glColor3f( 1.0f, 1.0f, 1.0f );
	
	int len = strlen(displayText);

	glRasterPos2f(x, y);

	for (int i = 0; i < len; i++) {
		glutBitmapCharacter(GLUT_BITMAP_TIMES_ROMAN_24, displayText[i]);
  }
}

void GazeRenderer::RecalibStarted(int trackingStatus)
{
	if(trackingStatus == 1)
	{
		struct _timeb start;
		_ftime(&start);		

		recalibTimestamp = start.time * 1000 + start.millitm;
		inRecalibSequence = true;
		recalibSequenceEnd = false;
	}
}


void GazeRenderer::DisplayGazeData(ScreenSpaceGazeData data)
{
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();

	GLdouble modelview[16];
	GLdouble projection[16];
	GLint viewport[4];
	
	glGetDoublev(GL_MODELVIEW_MATRIX, modelview);
	glGetDoublev(GL_PROJECTION_MATRIX, projection);
	glGetIntegerv(GL_VIEWPORT, viewport);
	
	float psize = 10.0f;
	glPointSize(psize);;
	glBegin(GL_POINTS);
	
	float x = data.x * width;
	float y = height - data.y * height;
	float z = 0;

	if(data.x >= 1.0f)
	{
		x = width -	psize;
	}

	if(data.x <= 0.0f)
	{
		x = psize;
	}

	if(data.y >= 1.0f)
	{
		y = psize;
	}

	if(data.y <= 0.0f)
	{
		y = height - psize;
	}

	glReadPixels(x, y, 1, 1, GL_DEPTH_COMPONENT, GL_FLOAT, &z);

	GLdouble glx, gly, glz;
			
	gluUnProject(x, y, z, modelview, projection, viewport, &glx, &gly, &glz);

	if(data.inState == 0)
	{
		return;
	}

	if(data.inState == 1)
	{
		glColor4f(1, 0, 0, 0);
	}

	if(data.inState == 2)
	{
		glColor4f(0, 0, 1, 0);
	}

	glVertex3f(glx, gly, glz);

	glEnd();

}

void GazeRenderer::initFrameTexture(VsImage *image)
{
	int xTexSize = 1;
    int yTexSize = 1;
    while((image->width / xTexSize) > 0) xTexSize <<= 1;
    while((image->height / yTexSize) > 0) yTexSize <<= 1;

	glGenTextures(1, &frameTexId);
	frameTexId += 50;
 
	glBindTexture(GL_TEXTURE_2D, frameTexId);

	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);

	switch (image->nChannels) 
	{
		case 1:
			glTexImage2D(GL_TEXTURE_2D, 0, GL_LUMINANCE, xTexSize, yTexSize, 0, GL_LUMINANCE, GL_UNSIGNED_BYTE, 0);
			break;
		case 3:
		case 4:
		default:
			glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, xTexSize, yTexSize, 0, GL_BGR_EXT, GL_UNSIGNED_BYTE, 0);
			break;
	}
	xTexCoord = image->width / (float) xTexSize;
	yTexCoord = image->height / (float) yTexSize;

    float videoAspect = image->width / (float) image->height;
	float tmp;
    if(image->width < image->height)
    {    
		tmp = glHeight;
    	glHeight = glWidth / videoAspect;
		if (glHeight > tmp)
		{
			glWidth  = glWidth*tmp/glHeight;
			glHeight = tmp;
		}
    }
    else
    {
		tmp = glWidth;
    	glWidth = glHeight * videoAspect;
		if (glWidth > tmp)
		{
			glHeight  = glHeight*tmp/glWidth;
			glWidth = tmp;
		}		
    }
}

void GazeRenderer::setFrameTexture(VsImage *image)
{
	//glClear(GL_COLOR_BUFFER_BIT);
	glBindTexture(GL_TEXTURE_2D, frameTexId);	
	switch (image->nChannels) 
	{
		case 3:
			glTexSubImage2D(GL_TEXTURE_2D, 0, 0, 0, image->width, image->height, GL_BGR_EXT, GL_UNSIGNED_BYTE, image->imageData);
			break;
		case 4:
			//glTexSubImage2D(GL_TEXTURE_2D, 0, 0, 0, image->width, image->height, GL_BGRA, GL_UNSIGNED_BYTE, image->imageData);
			break;
		case 1:
			glTexSubImage2D(GL_TEXTURE_2D, 0, 0, 0, image->width, image->height, GL_LUMINANCE, GL_UNSIGNED_BYTE, image->imageData);
			break;
		default:
			return; 
		} 
}
static float zoomFactor = 2.0f;
void GazeRenderer::DrawImage()
{
	glDisable(GL_DEPTH_TEST);

	glEnable(GL_TEXTURE_2D);
	
	glMatrixMode(GL_PROJECTION);
	glPushMatrix();
	glLoadIdentity();									// Reset The Projection Matrix
	glOrtho(-zoomFactor, zoomFactor, -zoomFactor, zoomFactor, -10.0f, 10.0f);
	
	glMatrixMode(GL_MODELVIEW);
	glPushMatrix();
	glLoadIdentity();	// Reset The Projection Matrix
		
	
	const GLfloat vertices[] = {
		-1.0f,-1.0f,-5.0f,
		1.0f,-1.0f,-5.0f,
		-1.0f,1.0f,-5.0f,
		1.0f,1.0f,-5.0f,
	};
	
	//tex coords are flipped upside down instead of an image
	const GLfloat texcoords[] = {
		0.0f,		yTexCoord,
		xTexCoord,	yTexCoord,
		0.0f,		0.0f,
		xTexCoord,	0.0f,
	};
				
	glColor4f(1.0f, 1.0f, 1.0f, 1.0f);					// Set Color To White
	
	glEnableClientState(GL_TEXTURE_COORD_ARRAY);
	glEnableClientState(GL_VERTEX_ARRAY);
	
	glVertexPointer(3, GL_FLOAT, 0, vertices);
	glTexCoordPointer(2, GL_FLOAT, 0, texcoords);
	
	glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);
	
	//LOGI ("Frame viewport %d %d", glWidth, glHeight);
	glDisableClientState(GL_TEXTURE_COORD_ARRAY);
	glDisableClientState(GL_VERTEX_ARRAY);
	
	glDisable(GL_TEXTURE_2D);
	
	glPopMatrix();
	glMatrixMode(GL_MODELVIEW);
	glPopMatrix();

	glClear(GL_DEPTH_BUFFER_BIT);
	glEnable(GL_DEPTH_TEST);
}

void GazeRenderer::SetupFrustum(int width, int height, int focus)
{
	GLfloat x_offset = 1;
	GLfloat y_offset = 1;
	
	if(width > height)
	{
		x_offset = ((GLfloat)width) / ((GLfloat)height);
	}
	else if (width < height)
	{
		y_offset = ((GLfloat)height) / ((GLfloat)width);
	}
	
	GLfloat frustum_near = 0.001f;
	GLfloat frustum_far = 30;
	
	GLfloat frustum_x = x_offset * frustum_near / focus;
	GLfloat frustum_y = y_offset * frustum_near / focus;
	
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	
	glFrustum(-frustum_x * zoomFactor, frustum_x * zoomFactor, -frustum_y * zoomFactor, frustum_y * zoomFactor, frustum_near, frustum_far);
	
	glMatrixMode(GL_MODELVIEW);
	
	glLoadIdentity();
	
	gluLookAt(0, 0, 0, 0, 0, 1, 0, 1, 0);

	POINT mousePos;
	GetCursorPos(&mousePos);

	GLdouble modelview[16];
	GLdouble projection[16];
	GLint viewport[4];
	
	glGetDoublev(GL_MODELVIEW_MATRIX, modelview);
	glGetDoublev(GL_PROJECTION_MATRIX, projection);
	glGetIntegerv(GL_VIEWPORT, viewport);
	
	float z;

	glReadPixels(mousePos.x, viewport[3] - mousePos.y, 1, 1, GL_DEPTH_COMPONENT, GL_FLOAT, &z);

	GLdouble glx, gly, glz;
			
	gluUnProject(mousePos.x, viewport[3] - mousePos.y, z, modelview, projection, viewport, &glx, &gly, &glz);
}

void GazeRenderer::drawFaceFeatures(FDP* featurePoints2D)
{
	//sizes of the fdp's groups
    int GrSize[13] = {0,0,14,14,6,4,4,1,10,15,10,5,12};

	glPointSize(3);

	//draw chin points
	glColor4ub(128,128,255,255);

	static int chinPoints[] = {
		2,	14,
		2,	12,
		2,	1,
		2,	11,
		2,	13,
		2,	10,
	};
	static int chinLinesPoints[] = {
		2,	14,
		2,	12,
		2,	12,
		2,	1,
		2,	1,
		2,	11,
		2,	11,
		2,	13,
		2,	1,
		2,	10,
	};

	drawPoints2D(chinPoints, 6, GL_POINTS, featurePoints2D);
	drawPoints2D(chinLinesPoints, 10, GL_LINES, featurePoints2D);

	// draw inner lip points
	glColor4ub(236,0,0,255);

	static int innerLipPoints[] = {
		2,	2,
		2,	6,
		2,	4,
		2,	8,
		2,	3,
		2,	9,
		2,	5,
		2,	7,
	};

	drawPoints2D(innerLipPoints, 8, GL_POINTS, featurePoints2D);
	drawPoints2D(innerLipPoints, 8, GL_LINE_LOOP, featurePoints2D);

	// draw outer lip points
	glColor4ub(236,0,0,255);

	static int outerLipPoints[] = {
		8,	1,
		8,	10,
		8,	5,
		8,	3,
		8,	7,
		8,	2,
		8,	8,
		8,	4,
		8,	6,
		8,	9,
	};
	drawPoints2D(outerLipPoints, 10, GL_POINTS, featurePoints2D);
	drawPoints2D(outerLipPoints, 10, GL_LINE_LOOP, featurePoints2D);

	//draw nose
	glColor4ub(100,100,100,255);

	static int nosePoints[] = {
		9,	1,
		9,	2,
		9,	3,
		9,	4,
		9,	5,
		9,	6,
		9,	7,
		9,	8,
		9,	9,
		9,	10,
		9,	11,
		9,	12,
		9,	13,
		9,	14,
		9,	15,
	};
	static int noseLinesPoints1[] = {
		9,	15,		//part one
		9,	4,
		9,	2,
		9,	3,
		9,	1,
		9,	5,
	};
	static int noseLinesPoints2[] = {
		9,	6,		//part two
		9,	7,
		9,	13,
		9,	12,
		9,	14,
	};
	static int noseLinesPoints3[] = {
		9,	14,		//part three
		9,	2,
		9,	13,
		9,	1,
	};

	drawPoints2D(nosePoints, 15, GL_POINTS, featurePoints2D);
	drawPoints2D(noseLinesPoints1, 6, GL_LINE_LOOP, featurePoints2D);
	drawPoints2D(noseLinesPoints2, 5, GL_LINE_LOOP, featurePoints2D);
	drawPoints2D(noseLinesPoints3, 4, GL_LINES, featurePoints2D);

	//draw eyes
	glColor4ub(255,255,0,255);

	static int irisPoints[] = {
		3,	5,
		3,	6,
	};
	drawPoints2D(irisPoints, 2, GL_POINTS, featurePoints2D);

	glColor4ub(255,143,32,255);

	static int eyesPoints[] = {
		3,	1,
		3,	2,
		3,	3,
		3,	4,
		3,	7,
		3,	8,
		3,	9,
		3,	10,
		3,	11,
		3,	12,
		3,	13,
		3,	14,
	};
	static int eyesLinesPoints1[] = {
		3,	12,
		3,	14,
		3,	8,
		3,	10,
	};
	static int eyesLinesPoints2[] = {
		3,	12,
		3,	2,
		3,	8,
		3,	4,
	};
	static int eyesLinesPoints3[] = {
		3,	11,
		3,	13,
		3,	7,
		3,	9,
	};
	static int eyesLinesPoints4[] = {
		3,	11,
		3,	1,
		3,	7,
		3,	3,
	};

	drawPoints2D(eyesPoints, 12, GL_POINTS, featurePoints2D);
	drawPoints2D(eyesLinesPoints1, 4, GL_LINE_LOOP, featurePoints2D);
	drawPoints2D(eyesLinesPoints2, 4, GL_LINE_LOOP, featurePoints2D);
	drawPoints2D(eyesLinesPoints3, 4, GL_LINE_LOOP, featurePoints2D);
	drawPoints2D(eyesLinesPoints4, 4, GL_LINE_LOOP, featurePoints2D);


	//draw cheeks
	glColor4ub(100,100,100,255);

	static int cheekPoints[] = {
		5,	1,
		5,	2,
		5,	3,
		5,	4,
	};

	drawPoints2D(cheekPoints, 4, GL_POINTS, featurePoints2D);

	//draw ears
	static int earPoints[] = {
		10,	1,
		10,	2,
		10,	3,
		10,	4,
		10,	5,
		10,	6,
		10,	7,
		10,	8,
		10,	9,
		10,	10,
	};

	drawPoints2D(earPoints, 10, GL_POINTS, featurePoints2D);

	//draw lines connecting ears and cheeks
	static int earcheekLinesPoints1[] = {
		5,	2,
		5,	4,
		10,	10,
		10,	8,
	};
	static int earcheekLinesPoints2[] = {
		5,	1,
		5,	3,
		10,	9,
		10,	7,
	};

	drawPoints2D(earcheekLinesPoints1, 4, GL_LINE_LOOP, featurePoints2D);
	drawPoints2D(earcheekLinesPoints2, 4, GL_LINE_LOOP, featurePoints2D);

	//draw eyebrows
	glColor4ub(227,254,73,255);
	static int eyebrowPoints[] = {
		4,	1,
		4,	2,
		4,	3,
		4,	4,
		4,	5,
		4,	6,
	};
	static int eyebrowLinesPoints[] = {
		4,	6,
		4,	4,
		4,	4,
		4,	2,
		4,	1,
		4,	3,
		4,	3,
		4,	5,
	};
	drawPoints2D(eyebrowPoints, 6, GL_POINTS, featurePoints2D);
	drawPoints2D(eyebrowLinesPoints, 8, GL_LINES, featurePoints2D);

	//draw head/hair
	glColor4ub(100,100,100,255);

	static int hairPoints[] = {
		11,	1,
		11,	2,
		11,	3,
		11,	4,
		11,	5,
		11,	6,
	};
	static int hairLinesPoints[] = {
		11,	2,
		11,	1,
		11,	1,
		11,	3,
	};

	drawPoints2D(hairPoints, 6, GL_POINTS, featurePoints2D);
	drawPoints2D(hairLinesPoints, 4, GL_LINES, featurePoints2D);
}

void GazeRenderer::drawGazeDirection(const float* gazeDirection ,FDP* featurePoints3D)
{
		
    static float vertices[] = {
        0.0f, 0.0f, 0.0f,
        0.0f, 0.0f, 0.05f
    };

    FeaturePoint* fp1;
    FeaturePoint* fp2;

	float tr[6] = {0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f};

    fp1 = const_cast<FeaturePoint*>( &featurePoints3D->getFP(3,5) );
    fp2 = const_cast<FeaturePoint*>( &featurePoints3D->getFP(3,6) );
    if(fp1->defined && fp2->defined)
    {
        tr[0] = fp1->pos[0];
        tr[1] = fp1->pos[1];
        tr[2] = fp1->pos[2];
        tr[3] = fp2->pos[0];
        tr[4] = fp2->pos[1];
        tr[5] = fp2->pos[2];
    }

    float h_rot = (gazeDirection[1] + 3.1415926f) * 180.0f/3.1415926f;
    float v_rot = gazeDirection[0] * 180.0f/3.1415926f;
    float roll = gazeDirection[2] * 180.0f/3.1415926f;


    glEnableClientState(GL_VERTEX_ARRAY);
    glPushMatrix();

    glTranslatef(tr[0],tr[1],tr[2]);
    glRotatef(h_rot, 0.0f, 1.0f, 0.0f);
    glRotatef(v_rot, 1.0f, 0.0f, 0.0f);
    glRotatef(roll, 0.0f, 0.0f, 1.0f);

    glLineWidth(2);

    glColor4ub(0,255,0,255);


    glVertexPointer(3, GL_FLOAT, 0, vertices);
    glDrawArrays(GL_LINES, 0, 2);

    glPopMatrix();

    glPushMatrix();

    glTranslatef(tr[3],tr[4],tr[5]);
    glRotatef(h_rot, 0.0f, 1.0f, 0.0f);
    glRotatef(v_rot, 1.0f, 0.0f, 0.0f);
    glRotatef(roll, 0.0f, 0.0f, 1.0f);


    glVertexPointer(3, GL_FLOAT, 0, vertices);
    glDrawArrays(GL_LINES, 0, 2);
    glDisableClientState(GL_VERTEX_ARRAY);

    glPopMatrix();

    glLineWidth(1);

    glMatrixMode(GL_PROJECTION);
    glPopMatrix();
    glMatrixMode(GL_MODELVIEW);
    glPopMatrix();
}

void GazeRenderer::drawPoints2D(int *points, int num, int drawType, FDP* featurePoints2D)
{
	float *vertices;
	vertices = new float[num*3];
	int n = 0;
	FeaturePoint* fp;
	for (int i = 0; i < num; i++) {
		fp = const_cast<FeaturePoint*>( &featurePoints2D->getFP(points[2*i],points[2*i+1]) );
		if(fp->defined)
			if(fp->pos[0]!=0 && fp->pos[1]!=0)
			{
				vertices[3*n+0] = fp->pos[0];
				vertices[3*n+1] = fp->pos[1];
				vertices[3*n+2] = 0.0f;
				n++;
			}
	}

	glEnableClientState(GL_VERTEX_ARRAY);
	glVertexPointer(3, GL_FLOAT, 0, vertices);
	glDrawArrays(drawType, 0, n);
	glDisableClientState(GL_VERTEX_ARRAY);

	//clean-up
	delete[] vertices;
}

void GazeRenderer::DrawPoint(float x, float y, int r, int g, int b, int size)
{
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();

	GLdouble modelview[16];
	GLdouble projection[16];
	GLint viewport[4];
	
	glGetDoublev(GL_MODELVIEW_MATRIX, modelview);
	glGetDoublev(GL_PROJECTION_MATRIX, projection);
	glGetIntegerv(GL_VIEWPORT, viewport);
	
	float psize = size;
	glPointSize(psize);;
	glBegin(GL_POINTS);
	
	float z = 0;

	x = x * width;
	y = y * height;

	//x = x >= width ? width - psize : x;
	//x = x <= 0.0f ? psize : x;
	//
	//y = y >= height ? psize : y;
	//y = y <= 0.0f ? height - psize : y;

	glReadPixels(x, y, 1, 1, GL_DEPTH_COMPONENT, GL_FLOAT, &z);

	GLdouble glx, gly, glz;
			
	gluUnProject(x, y, z, modelview, projection, viewport, &glx, &gly, &glz);

	glColor4f(r, g, b, 0);

		glVertex3f(glx, gly, glz);

	glEnd();
}

void GazeRenderer::DrawTrackingData(FaceData* data, VsImage* frame)
{
	glMatrixMode(GL_PROJECTION);
	glPushMatrix();
	glLoadIdentity();									// Reset The Projection Matrix

	glOrtho(0.0f,zoomFactor,0.0f,zoomFactor,-10.0f,10.0f);

	glMatrixMode(GL_MODELVIEW);
	glPushMatrix();
	glLoadIdentity();									// Reset The ModelView Matrix
	glTranslatef(zoomFactor/2 -0.5f, zoomFactor/2 -0.5f, 0.0f);
	glLineWidth(2);

	drawFaceFeatures(data->featurePoints2D);
			
	SetupFrustum(frame->width, frame->height, data->cameraFocus);
	drawGazeDirection(data->gazeDirectionGlobal, data->featurePoints3D);
}

void GazeRenderer::DrawImage(VsImage* image)
{
	if(!setRenderingContext && image != 0)
	{
		width = GetSystemMetrics(SM_CXSCREEN);
		height = GetSystemMetrics(SM_CYSCREEN);

		initFrameTexture(image);

		glViewport(0,0,width, height);	

		setRenderingContext = true; 
	}
	else
	{
		setFrameTexture(image);
		DrawImage();
	}
}

void GazeRenderer::Clear()
{
	glClear(GL_COLOR_BUFFER_BIT);
}

void GazeRenderer::Swap()
{
	window->Swap();
}