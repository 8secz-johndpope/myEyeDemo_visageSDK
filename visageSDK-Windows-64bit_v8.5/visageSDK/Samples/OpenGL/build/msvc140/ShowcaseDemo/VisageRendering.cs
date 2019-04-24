using System;
using System.Collections.Generic;
using System.Linq;
using SharpGL;
using VisageCSWrapper;


namespace ShowcaseDemo
{
    public static class ListExtra
    {
        public static void Resize<T>(this List<T> list, int sz, T c = default(T))
        {
            int cur = list.Count;
            if (sz < cur)
                list.RemoveRange(sz, cur - sz);
            else if (sz > cur)
                list.AddRange(Enumerable.Repeat(c, sz - cur));
        }
    }

    class VisageRendering
    {
        public VisageRendering(OpenGL new_gl)
        {
            gl = new_gl;
            currentPosition = 3;
        }

        uint[] frame_tex_id = new uint[1];
        uint[] model_tex_id = new uint[1];
        static float tex_x_coord = 0;
        static float tex_y_coord = 0;
        static bool video_texture_inited = false;
        static int video_texture_width = 0;
        static int video_texture_height = 0;
        static int numberOfVertices = 0;
        static VSImage texture;

        static List<int> output = new List<int>();

        const float VS_PI = 3.1415926535897932384626433832795f;
        const int EMOTIONS_NUM = 7;

        public const int DISPLAY_FEATURE_POINTS = 1;
        public const int DISPLAY_SPLINES = 2;
        public const int DISPLAY_GAZE = 4;
        public const int DISPLAY_AXES = 8;
        public const int DISPLAY_FRAME = 16;
        public const int DISPLAY_WIRE_FRAME = 32;
        public const int DISPLAY_EMOTION = 64;
        public const int DISPLAY_GENDER = 128;
        public const int DISPLAY_AGE = 256;
        public const int DISPLAY_TEXTURED_MODEL = 512;
        public const int DISPLAY_TRACKING_QUALITY = 1024;
        public const int DISPLAY_POINT_QUALITY = 2048;
        public const int DISPLAY_NAME = 4096;
        public const int DISPLAY_ALL = DISPLAY_FEATURE_POINTS + DISPLAY_SPLINES + DISPLAY_GAZE + DISPLAY_AXES +
                                DISPLAY_FRAME + DISPLAY_WIRE_FRAME + DISPLAY_EMOTION + DISPLAY_GENDER + DISPLAY_AGE + DISPLAY_NAME + DISPLAY_TEXTURED_MODEL + DISPLAY_TRACKING_QUALITY + DISPLAY_POINT_QUALITY;

        public const float BCKG_LENGTH_PERC = 0.3f;
        public static double VS_DEG2RAD(double a)
        {
            return (a * VS_PI / 180.0);
        }
        public static double VS_RAD2DEG(double a)
        {
            return (a * 180.0f / VS_PI);
        }

        public OpenGL gl;

        private enum DISPLAY_POS : int
        {
            UP = 0,
            RIGHT,
            LEFT,
            DOWN
        }

        public int currentPosition = (int)DISPLAY_POS.DOWN;



        struct Vec2D
        {
            public Vec2D(float _x, float _y)
            {
                x = _x;
                y = _y;
            }
            public float x, y;
        }

        struct CubicPoly
        {
            public float c0, c1, c2, c3;
            public float eval(float t)
            {
                float t2 = t * t;
                float t3 = t2 * t;
                return c0 + c1 * t + c2 * t2 + c3 * t3;
            }
        }


        public void ClearGL()
        {
            gl.ClearColor(0, 0, 0, 0);
            gl.Clear(OpenGL.GL_COLOR_BUFFER_BIT);
        }

        int NearestPow2(int n)
        {
            int v; // compute the next highest power of 2 of 32-bit v 

            v = n;
            v--;
            v |= v >> 1;
            v |= v >> 2;
            v |= v >> 4;
            v |= v >> 8;
            v |= v >> 16;
            v++;

            return v;
        }

        void InitFrameTex(int x_size, int y_size, VSImage image)
        {
            //Create The Texture
            gl.GenTextures(1, frame_tex_id);

            //Typical Texture Generation Using Data From The Bitmap
            gl.BindTexture(OpenGL.GL_TEXTURE_2D, frame_tex_id[0]);
            gl.TexParameter(OpenGL.GL_TEXTURE_2D, OpenGL.GL_TEXTURE_WRAP_S, OpenGL.GL_REPEAT);
            gl.TexParameter(OpenGL.GL_TEXTURE_2D, OpenGL.GL_TEXTURE_WRAP_T, OpenGL.GL_REPEAT);
            gl.TexParameter(OpenGL.GL_TEXTURE_2D, OpenGL.GL_TEXTURE_MAG_FILTER, OpenGL.GL_LINEAR);
            gl.TexParameter(OpenGL.GL_TEXTURE_2D, OpenGL.GL_TEXTURE_MIN_FILTER, OpenGL.GL_LINEAR);

            gl.TexImage2D(OpenGL.GL_TEXTURE_2D, 0, OpenGL.GL_RGBA, x_size, y_size, 0, OpenGL.GL_LUMINANCE, OpenGL.GL_UNSIGNED_BYTE, null);

            tex_x_coord = (float)image.width / (float)x_size;
            tex_y_coord = (float)image.height / (float)y_size;

            //Create The Texture
            gl.GenTextures(1, model_tex_id);

            //Typical Texture Generation Using Data From The Bitmap
            gl.BindTexture(OpenGL.GL_TEXTURE_2D, model_tex_id[0]);
            gl.TexParameter(OpenGL.GL_TEXTURE_2D, OpenGL.GL_TEXTURE_WRAP_S, OpenGL.GL_REPEAT);
            gl.TexParameter(OpenGL.GL_TEXTURE_2D, OpenGL.GL_TEXTURE_WRAP_T, OpenGL.GL_REPEAT);
            gl.TexParameter(OpenGL.GL_TEXTURE_2D, OpenGL.GL_TEXTURE_MAG_FILTER, OpenGL.GL_LINEAR);
            gl.TexParameter(OpenGL.GL_TEXTURE_2D, OpenGL.GL_TEXTURE_MIN_FILTER, OpenGL.GL_LINEAR);
            gl.Enable(OpenGL.GL_BLEND);

            gl.TexImage2D(OpenGL.GL_TEXTURE_2D, 0, OpenGL.GL_RGBA, texture.width, texture.height, 0, OpenGL.GL_BGRA, OpenGL.GL_UNSIGNED_BYTE, texture.imageData);
        }

        public static void Swap<T>(ref T left, ref T right)
        {
            T temp;
            temp = left;
            left = right;
            right = temp;
        }

        public void DisplayFrame(VSImage image, int width, int height)
        {
            if (video_texture_inited && (video_texture_width != image.width || video_texture_height != image.height))
            {
                gl.DeleteTextures(1, frame_tex_id);
                video_texture_inited = false;
                currentPosition = 3;
            }

            if (!video_texture_inited)
            {
                InitFrameTex(NearestPow2(image.width), NearestPow2(image.height), image);
                video_texture_width = image.width;
                video_texture_height = image.height;
                video_texture_inited = true;
            }

            gl.BindTexture(OpenGL.GL_TEXTURE_2D, frame_tex_id[0]);

            switch (image.nChannels)
            {
                case 3:
                    gl.TexImage2D(OpenGL.GL_TEXTURE_2D, 0, (int)OpenGL.GL_RGBA, image.width, image.height, 0, OpenGL.GL_BGR_EXT, OpenGL.GL_UNSIGNED_BYTE, image.imageData);
                    break;
                case 4:
                    gl.TexImage2D(OpenGL.GL_TEXTURE_2D, 0, (int)OpenGL.GL_RGBA, image.width, image.height, 0, OpenGL.GL_BGRA, OpenGL.GL_UNSIGNED_BYTE, image.imageData);
                    break;
                case 1:
                    gl.TexImage2D(OpenGL.GL_TEXTURE_2D, 0, (int)OpenGL.GL_RGBA, image.width, image.height, 0, OpenGL.GL_LUMINANCE, OpenGL.GL_UNSIGNED_BYTE, image.imageData);
                    break;
                default:
                    return;
            }

            gl.TexParameter(OpenGL.GL_TEXTURE_2D, OpenGL.GL_TEXTURE_MIN_FILTER, OpenGL.GL_LINEAR);

            gl.PushAttrib(OpenGL.GL_DEPTH_BUFFER_BIT | OpenGL.GL_VIEWPORT_BIT | OpenGL.GL_ENABLE_BIT | OpenGL.GL_FOG_BIT | OpenGL.GL_STENCIL_BUFFER_BIT | OpenGL.GL_TRANSFORM_BIT | OpenGL.GL_TEXTURE_BIT);

            gl.Disable(OpenGL.GL_ALPHA_TEST);
            gl.Disable(OpenGL.GL_DEPTH_TEST);
            gl.Disable(OpenGL.GL_BLEND);
            gl.Disable(OpenGL.GL_DITHER);
            gl.Disable(OpenGL.GL_FOG);
            gl.Disable(OpenGL.GL_SCISSOR_TEST);
            gl.Disable(OpenGL.GL_STENCIL_TEST);
            gl.Disable(OpenGL.GL_LIGHTING);
            gl.Disable(OpenGL.GL_LIGHT0);
            gl.Enable(OpenGL.GL_TEXTURE_2D);

            gl.BindTexture(OpenGL.GL_TEXTURE_2D, frame_tex_id[0]);

            gl.MatrixMode(OpenGL.GL_MODELVIEW);
            gl.PushMatrix();
            gl.LoadIdentity();

            gl.MatrixMode(OpenGL.GL_PROJECTION);
            gl.PushMatrix();
            gl.LoadIdentity();

            gl.Ortho(0.0f, 1.0f, 0.0f, 1.0f, -10.0f, 10.0f);

            float[] vertices = {
                0.0f,0.0f,-5.0f,
                1.0f,0.0f,-5.0f,
                0.0f,1.0f,-5.0f,
                1.0f,1.0f,-5.0f,
            };

            float[] texcoords = {
                0.0f,   1.0f,
                1.0f,   1.0f,
                0.0f,   0.0f,
                1.0f,   0.0f,
            };

            gl.Color(1.0f, 1.0f, 1.0f, 1.0f);

            gl.EnableClientState(OpenGL.GL_TEXTURE_COORD_ARRAY);
            gl.EnableClientState(OpenGL.GL_VERTEX_ARRAY);

            gl.VertexPointer(3, 0, vertices); // default type: GL_FLOAT
            gl.TexCoordPointer(2, OpenGL.GL_FLOAT, 0, texcoords);

            gl.Viewport(0, 0, width, height);
            gl.DrawArrays(OpenGL.GL_TRIANGLE_STRIP, 0, 4);

            gl.DisableClientState(OpenGL.GL_TEXTURE_COORD_ARRAY);
            gl.DisableClientState(OpenGL.GL_VERTEX_ARRAY);

            gl.Disable(OpenGL.GL_TEXTURE_2D);

            gl.PopMatrix();
            gl.MatrixMode(OpenGL.GL_MODELVIEW);
            gl.PopMatrix();

            gl.BindTexture(OpenGL.GL_TEXTURE_2D, 0);


            gl.PopAttrib();

            gl.Clear(OpenGL.GL_DEPTH_BUFFER_BIT);
        }

        static void InitCubicPoly(float x0, float x1, float t0, float t1, ref CubicPoly p)
        {
            p.c0 = x0;
            p.c1 = t0;
            p.c2 = -3 * x0 + 3 * x1 - 2 * t0 - t1;
            p.c3 = 2 * x0 - 2 * x1 + t0 + t1;
        }

        static void InitNonuniformCatmullRom(float x0, float x1, float x2, float x3, float dt0, float dt1, float dt2, ref CubicPoly p)
        {
            float t1 = (x1 - x0) / dt0 - (x2 - x0) / (dt0 + dt1) + (x2 - x1) / dt1;
            float t2 = (x2 - x1) / dt1 - (x3 - x1) / (dt1 + dt2) + (x3 - x2) / dt2;

            t1 *= dt1;
            t2 *= dt1;

            InitCubicPoly(x1, x2, t1, t2, ref p);
        }

        static float VecDistSquared(ref Vec2D p, ref Vec2D q)
        {
            float dx = q.x - p.x;
            float dy = q.y - p.y;
            return dx * dx + dy * dy;
        }

        static void InitCentripetalCR(ref Vec2D p0, ref Vec2D p1, ref Vec2D p2, ref Vec2D p3, ref CubicPoly px, ref CubicPoly py)
        {
            float dt0 = (float)Math.Pow(VecDistSquared(ref p0, ref p1), 0.25f);
            float dt1 = (float)Math.Pow(VecDistSquared(ref p1, ref p2), 0.25f);
            float dt2 = (float)Math.Pow(VecDistSquared(ref p2, ref p3), 0.25f);

            if (dt1 < 1e-4f) dt1 = 1.0f;
            if (dt0 < 1e-4f) dt0 = dt1;
            if (dt2 < 1e-4f) dt2 = dt1;

            InitNonuniformCatmullRom(p0.x, p1.x, p2.x, p3.x, dt0, dt1, dt2, ref px);
            InitNonuniformCatmullRom(p0.y, p1.y, p2.y, p3.y, dt0, dt1, dt2, ref py);
        }

        void CalcSpline(ref List<float> inputPoints, int ratio, ref List<float> outputPoints)
        {
            int nPoints, nPointsToDraw, nLines;

            nPoints = inputPoints.Count / 2 + 2;
            nPointsToDraw = inputPoints.Count / 2 + (inputPoints.Count() / 2 - 1) * ratio;
            nLines = nPoints - 1 - 2;

            inputPoints.Insert(0, inputPoints[1] + (inputPoints[1] - inputPoints[3]));
            inputPoints.Insert(0, inputPoints[1] + (inputPoints[1] - inputPoints[3]));
            inputPoints.Insert(inputPoints.Count, inputPoints[inputPoints.Count / 2 - 2] + (inputPoints[inputPoints.Count / 2 - 2] - inputPoints[inputPoints.Count / 2 - 4]));
            inputPoints.Insert(inputPoints.Count, inputPoints[inputPoints.Count / 2 - 1] + (inputPoints[inputPoints.Count / 2 - 1] - inputPoints[inputPoints.Count / 2 - 3]));

            Vec2D p0 = new Vec2D(0, 0);
            Vec2D p1 = new Vec2D(0, 0);
            Vec2D p2 = new Vec2D(0, 0);
            Vec2D p3 = new Vec2D(0, 0);

            CubicPoly px = new CubicPoly();
            CubicPoly py = new CubicPoly();

            ListExtra.Resize<float>(outputPoints, 2 * nPointsToDraw);

            for (int i = 0; i < nPoints - 2; i++)
            {
                outputPoints[i * 2 * (ratio + 1)] = inputPoints[2 * i + 2];
                outputPoints[i * 2 * (ratio + 1) + 1] = inputPoints[2 * i + 1 + 2];
            }

            for (int i = 0; i < 2 * nLines; i = i + 2)
            {
                p0.x = inputPoints[i];
                p0.y = inputPoints[i + 1];
                p1.x = inputPoints[i + 2];
                p1.y = inputPoints[i + 3];
                p2.x = inputPoints[i + 4];
                p2.y = inputPoints[i + 5];
                p3.x = inputPoints[i + 6];
                p3.y = inputPoints[i + 7];

                InitCentripetalCR(ref p0, ref p1, ref p2, ref p3, ref px, ref py);

                for (int j = 1; j <= ratio; j++)
                {
                    outputPoints[i * (ratio + 1) + 2 * j] = (px.eval(1.00f / (ratio + 1) * (j)));
                    outputPoints[i * (ratio + 1) + 2 * j + 1] = (py.eval(1.00f / (ratio + 1) * (j)));
                }

            }

            inputPoints.RemoveRange(0, 2);
            inputPoints.RemoveRange(inputPoints.Count - 2, 2);
        }

        void SetupCamera(int width, int height, float f)
        {
            float x_offset = 1;
            float y_offset = 1;
            if (width > height)
                x_offset = ((float)width) / ((float)height);
            else if (width < height)
                y_offset = ((float)height) / ((float)width);

            //Note:
            // FOV in radians is: fov*0.5 = arctan ((top-bottom)*0.5 / near)
            // In this case: FOV = 2 * arctan(frustum_y / frustum_near)
            //set frustum specs
            float frustum_near = 0.001f;
            float frustum_far = 30; //hard to estimate face too far away
            float frustum_x = x_offset * frustum_near / f;
            float frustum_y = y_offset * frustum_near / f;
            //set frustum
            gl.MatrixMode(OpenGL.GL_PROJECTION);
            gl.LoadIdentity();
            gl.Frustum(-frustum_x, frustum_x, -frustum_y, frustum_y, frustum_near, frustum_far);
            gl.MatrixMode(OpenGL.GL_MODELVIEW);
            //clear matrix
            gl.LoadIdentity();
            //camera in (0,0,0) looking at (0,0,1) up vector (0,1,0)
            gl.LookAt(0, 0, 0, 0, 0, 1, 0, 1, 0); //uLookAt
        }

        public bool withinConstraints(ref FaceData trackingData)
        {
            float head_pitch_value_rad = trackingData.FaceRotation[0] - (float)Math.Atan2(trackingData.FaceTranslation[1], trackingData.FaceTranslation[2]);
            float head_yaw_value_rad = trackingData.FaceRotation[1] - (float)Math.Atan2(trackingData.FaceTranslation[0], trackingData.FaceTranslation[2]);
            float head_roll_value_rad = trackingData.FaceRotation[2];

            double head_pitch_value_deg = VS_RAD2DEG(head_pitch_value_rad);
            double head_yaw_value_deg = VS_RAD2DEG(head_yaw_value_rad);
            double head_roll_value_deg = VS_RAD2DEG(head_roll_value_rad);

            const double CONSTRAINT_ANGLE = 40;

            if (Math.Abs(head_pitch_value_deg) > CONSTRAINT_ANGLE ||
                Math.Abs(head_yaw_value_deg) > CONSTRAINT_ANGLE ||
                Math.Abs(head_roll_value_deg) > CONSTRAINT_ANGLE ||
                trackingData.FaceScale < 40)
                return false;

            return true;
        }


        void DrawElipse(float x, float y, float radiusX, float radiusY)
        {
            gl.MatrixMode(OpenGL.GL_MODELVIEW);
            gl.PushMatrix();
            gl.LoadIdentity();

            gl.Translate(x, y, 0.0f);
            const int circle_points = 100;
            const float angle = 2.0f * 3.1416f / circle_points;

            gl.Begin(OpenGL.GL_POLYGON);
            double angle1 = 0.0;
            gl.Vertex(radiusX * Math.Cos(0.0), radiusY * Math.Sin(0.0)); //Vertex2d ?
            for (int i = 0; i < circle_points; i++)
            {
                gl.Vertex(radiusX * Math.Cos(angle1), radiusY * Math.Sin(angle1));
                angle1 += angle;
            }
            gl.End();
            gl.PopMatrix();
        }

        void DrawSpline2D(int[] points, int num, FDP fdp)
        {
            if (num < 2)
                return;

            List<float> pointCoords = new List<float>(); //vector<float>
            int n = 0;

            for (int i = 0; i < num; i++)
            {
                FeaturePoint fp = fdp.GetFP(points[2 * i], points[2 * i + 1]);
                if (fp.Defined == 1 && fp.Pos[0] != 0 && fp.Pos[1] != 0)
                {
                    pointCoords.Add(fp.Pos[0]);
                    pointCoords.Add(fp.Pos[1]);
                    n++;
                }
            }

            if (pointCoords.Count() == 0 || n <= 2)
                return;

            int factor = 10;
            List<float> pointsToDraw = new List<float>();
            CalcSpline(ref pointCoords, factor, ref pointsToDraw);
            int nVert = (int)pointsToDraw.Count / 2;
            float[] vertices = new float[nVert * 3];
            int cnt = 0;
            for (int i = 0; i < nVert; ++i)
            {
                vertices[3 * i + 0] = pointsToDraw[cnt++];
                vertices[3 * i + 1] = pointsToDraw[cnt++];
                vertices[3 * i + 2] = 0.0f;
            }
            gl.EnableClientState(OpenGL.GL_VERTEX_ARRAY);
            gl.VertexPointer(3, 0, vertices);
            gl.DrawArrays(OpenGL.GL_LINE_STRIP, 0, nVert);
            gl.DisableClientState(OpenGL.GL_VERTEX_ARRAY);
        }

        void DrawPoints2D(int[] points, int num, bool singleColor, int faceScale, int frameWidth, int frameHeight, FDP fdp, bool drawQuality = true)
        {
            float radius = (faceScale / (float)frameWidth) * 30;

            float radiusX = (faceScale / (float)frameWidth) * 0.017f;
            float radiusY = (faceScale / (float)frameHeight) * 0.017f;

            for (int i = 0; i < num; i++)
            {
                FeaturePoint fp = fdp.GetFP(points[2 * i], points[2 * i + 1]);

                if (fp.Defined == 1 && fp.Pos[0] != 0 && fp.Pos[1] != 0) //fp.defined ?
                {
                    float x = fp.Pos[0];
                    float y = fp.Pos[1];

                    gl.Color(0, 0, 0, 1.0f);
                    DrawElipse(x, y, radiusX, radiusY);

                    if (!singleColor)
                    {
                        if (fp.Quality >= 0 && drawQuality)
                            gl.Color((1 - fp.Quality) * 1.0f, fp.Quality * 1.0f, 0, 1.0f);
                        else
                            gl.Color(0, 1.0f, 1.0f, 1.0f);

                        DrawElipse(x, y, 0.6f * radiusX, 0.6f * radiusY);
                    }
                }
            }
        }

        void DrawPoints3D(int[] points, int num, bool singleColor, int faceScale, int frameWidth, int frameHeight, FDP fdp, bool relative)
        {
            float radius = (faceScale / (float)frameWidth) * 30;

            float[] vertices = new float[num * 3];
            int n = 0;
            for (int i = 0; i < num; i++)
            {
                FeaturePoint fp = fdp.GetFP(points[2 * i], points[2 * i + 1]);
                if (fp.Defined == 1 && fp.Pos[0] != 0 && fp.Pos[1] != 0)
                {
                    vertices[3 * n + 0] = fp.Pos[0];
                    vertices[3 * n + 1] = fp.Pos[1];
                    vertices[3 * n + 2] = fp.Pos[2];
                    n++;
                }
            }

            gl.Enable(OpenGL.GL_POINT_SMOOTH);
            gl.EnableClientState(OpenGL.GL_VERTEX_ARRAY);
            gl.VertexPointer(3, 0, vertices);
            gl.PointSize(radius);
            gl.Color(0, 0, 0, 1.0f);
            gl.DrawArrays(OpenGL.GL_POINTS, 0, n);
            if (!singleColor)
            {
                gl.PointSize(0.8f * radius);
                gl.Color(0, 1.0f, 1.0f, 1.0f);
                gl.DrawArrays(OpenGL.GL_POINTS, 0, n);
            }
            gl.DisableClientState(OpenGL.GL_VERTEX_ARRAY);
        }

        public void DisplayFeaturePoints(VSImage frame, FaceData trackingData, int width, int height, bool _3D, bool relative, bool drawQuality)
        {
            gl.Viewport(0, 0, width, height);

            gl.MatrixMode(OpenGL.GL_MODELVIEW);
            gl.PushMatrix();
            gl.LoadIdentity();

            gl.MatrixMode(OpenGL.GL_PROJECTION);
            gl.PushMatrix();
            gl.LoadIdentity();

            gl.Ortho(0.0f, 1.0f, 0.0f, 1.0f, -10.0f, 10.0f);

            if (_3D)
            {
                SetupCamera(width, height, trackingData.CameraFocus);

                if (relative)
                {
                    float[] r = trackingData.FaceRotation;
                    float[] t = trackingData.FaceTranslation;

                    gl.Translate(t[0], t[1], t[2]);
                    gl.Rotate(VS_RAD2DEG(r[1] + VS_PI), 0.0f, 1.0f, 0.0f);
                    gl.Rotate(VS_RAD2DEG(r[0]), 1.0f, 0.0f, 0.0f);
                    gl.Rotate(VS_RAD2DEG(r[2]), 0.0f, 0.0f, 1.0f);
                }
            }

            int faceScale = trackingData.FaceScale;
            FDP featurePoints2D = trackingData.FeaturePoints2D;
            FDP featurePoints3D = trackingData.FeaturePoints3D;
            int frameWidth = frame.width;
            int frameHeight = frame.height;

            int[] chinPoints = {
                2,  1
            };

            if (_3D) DrawPoints3D(chinPoints, 1, false, faceScale, frameWidth, frameHeight, featurePoints3D, relative);
            else DrawPoints2D(chinPoints, 1, false, faceScale, frameWidth, frameHeight, featurePoints2D, drawQuality);

            int[] innerLipPoints = {
                2,  2,
                2,  6,
                2,  4,
                2,  8,
                2,  3,
                2,  9,
                2,  5,
                2,  7,
            };
            if (_3D) DrawPoints3D(innerLipPoints, 8, false, faceScale, frameWidth, frameHeight, featurePoints2D, relative);
            else DrawPoints2D(innerLipPoints, 8, false, faceScale, frameWidth, frameHeight, featurePoints2D, drawQuality);

            int[] outerLipPoints = {
                8,  1,
                8,  10,
                8,  5,
                8,  3,
                8,  7,
                8,  2,
                8,  8,
                8,  4,
                8,  6,
                8,  9,
            };
            if (_3D) DrawPoints3D(outerLipPoints, 10, false, faceScale, frameWidth, frameHeight, featurePoints2D, relative);
            else DrawPoints2D(outerLipPoints, 10, false, faceScale, frameWidth, frameHeight, featurePoints2D, drawQuality);

            int[] nosePoints = {
                9,  5,
                9,  4,
                9,  3,
                9,  15,
                14, 22,
                14, 23,
                14, 24,
                14, 25
            };
            if (_3D) DrawPoints3D(nosePoints, 8, false, faceScale, frameWidth, frameHeight, featurePoints2D, relative);
            else DrawPoints2D(nosePoints, 8, false, faceScale, frameWidth, frameHeight, featurePoints2D, drawQuality);

            if (trackingData.EyeClosure[1] > 0.5f)
            {
                //if eye is open, draw the iris
                gl.Color(200, 80, 0, 255);
                int[] irisPoints = {
                    3,  6
                };
                if (_3D) DrawPoints3D(irisPoints, 1, false, faceScale, frameWidth, frameHeight, featurePoints2D, relative);
                else DrawPoints2D(irisPoints, 1, false, faceScale, frameWidth, frameHeight, featurePoints2D, drawQuality);
            }

            if (trackingData.EyeClosure[0] > 0.5f)
            {
                gl.Color(200, 80, 0, 255);
                int[] irisPoints = {
                    3,  5
                };
                if (_3D) DrawPoints3D(irisPoints, 1, false, faceScale, frameWidth, frameHeight, featurePoints2D, relative);
                else DrawPoints2D(irisPoints, 1, false, faceScale, frameWidth, frameHeight, featurePoints2D, drawQuality);
            }

            int[] eyesPointsR = {
                3,  2,
                3,  4,
                3,  8,
                3,  10,
                3,  12,
                3,  14,
                12, 6,
                12, 8,
                12, 10,
                12, 12
            };
            if (_3D) DrawPoints3D(eyesPointsR, 10, trackingData.EyeClosure[1] <= 0.5f, faceScale, frameWidth, frameHeight, featurePoints2D, relative);
            else DrawPoints2D(eyesPointsR, 10, trackingData.EyeClosure[1] <= 0.5f, faceScale, frameWidth, frameHeight, featurePoints2D, drawQuality);

            int[] eyesPointsL = {
                3,  1,
                3,  3,
                3,  7,
                3,  9,
                3,  11,
                3,  13,
                12, 5,
                12, 7,
                12, 9,
                12, 11
            };
            if (_3D) DrawPoints3D(eyesPointsL, 10, trackingData.EyeClosure[0] <= 0.5f, faceScale, frameWidth, frameHeight, featurePoints2D, relative);
            else DrawPoints2D(eyesPointsL, 10, trackingData.EyeClosure[0] <= 0.5f, faceScale, frameWidth, frameHeight, featurePoints2D, drawQuality);

            int[] eyebrowPoints = {
                4,  1,
                4,  2,
                4,  3,
                4,  4,
                4,  5,
                4,  6,
                14, 1,
                14, 2,
                14, 3,
                14, 4
            };
            if (_3D) DrawPoints3D(eyebrowPoints, 10, false, faceScale, frameWidth, frameHeight, featurePoints2D, relative);
            else DrawPoints2D(eyebrowPoints, 10, false, faceScale, frameWidth, frameHeight, featurePoints2D, drawQuality);

            // visible contour
            int[] contourPointsVisible = {
                13, 1,
                13, 3,
                13, 5,
                13, 7,
                13, 9,
                13, 11,
                13, 13,
                13, 15,
                13, 17,
                13, 16,
                13, 14,
                13, 12,
                13, 10,
                13, 8,
                13, 6,
                13, 4,
                13, 2
            };
            if (_3D) DrawPoints3D(contourPointsVisible, 17, false, faceScale, frameWidth, frameHeight, featurePoints2D, relative);
            else DrawPoints2D(contourPointsVisible, 17, false, faceScale, frameWidth, frameHeight, featurePoints2D, drawQuality);

            gl.MatrixMode(OpenGL.GL_PROJECTION);
            gl.PopMatrix();
            gl.MatrixMode(OpenGL.GL_MODELVIEW);
            gl.PopMatrix();
        }

        public void DisplaySplines(FaceData trackingData, int width, int height)
        {
            gl.Viewport(0, 0, width, height);

            gl.PushAttrib(OpenGL.GL_DEPTH_BUFFER_BIT | OpenGL.GL_VIEWPORT_BIT | OpenGL.GL_ENABLE_BIT | OpenGL.GL_FOG_BIT | OpenGL.GL_STENCIL_BUFFER_BIT | OpenGL.GL_TRANSFORM_BIT | OpenGL.GL_TEXTURE_BIT);

            gl.Disable(OpenGL.GL_ALPHA_TEST);
            gl.Disable(OpenGL.GL_DEPTH_TEST);
            gl.Disable(OpenGL.GL_DITHER);
            gl.Disable(OpenGL.GL_FOG);
            gl.Disable(OpenGL.GL_SCISSOR_TEST);
            gl.Disable(OpenGL.GL_STENCIL_TEST);

            gl.MatrixMode(OpenGL.GL_MODELVIEW);
            gl.PushMatrix();
            gl.LoadIdentity();

            gl.MatrixMode(OpenGL.GL_PROJECTION);
            gl.PushMatrix();
            gl.LoadIdentity();


            gl.Ortho(0.0f, 1.0f, 0.0f, 1.0f, -10.0f, 10.0f);

            gl.PointSize(3);
            gl.LineWidth(2);

            gl.BlendFunc(OpenGL.GL_SRC_ALPHA, OpenGL.GL_ONE_MINUS_SRC_ALPHA);
            gl.Enable(OpenGL.GL_BLEND);

            FDP featurePoints2D = trackingData.FeaturePoints2D;

            gl.Color(0.690196078, 0.768627451, 0.870588235, 1.0f);
            int[] outerUpperLipPoints = {
                8, 4,
                8, 6,
                8, 9,
                8, 1,
                8, 10,
                8, 5,
                8, 3,
            };
            DrawSpline2D(outerUpperLipPoints, 7, featurePoints2D);

            int[] outerLowerLipPoints = {
                8, 4,
                8, 8,
                8, 2,
                8, 7,
                8, 3,
            };
            DrawSpline2D(outerLowerLipPoints, 5, featurePoints2D);

            int[] innerUpperLipPoints = {
                2, 5,
                2, 7,
                2, 2,
                2, 6,
                2, 4,
            };
            DrawSpline2D(innerUpperLipPoints, 5, featurePoints2D);

            int[] innerLowerLipPoints = {
                2, 5,
                2, 9,
                2, 3,
                2, 8,
                2, 4,
            };
            DrawSpline2D(innerLowerLipPoints, 5, featurePoints2D);

            int[] noseLinePoints = {
                9,  5,
                9,  3,
                9,  4
            };
            DrawSpline2D(noseLinePoints, 3, featurePoints2D);

            int[] noseLinePoints2 = {
                9,  3,
                14, 22,
                14, 23,
                14, 24,
                14, 25
            };
            DrawSpline2D(noseLinePoints2, 5, featurePoints2D);

            int[] outerUpperEyePointsR = {
                3,  12,
                3,  14,
                3,  8
            };
            DrawSpline2D(outerUpperEyePointsR, 3, featurePoints2D);

            int[] outerLowerEyePointsR = {
                3,  8,
                3,  10,
                3,  12
            };
            DrawSpline2D(outerLowerEyePointsR, 3, featurePoints2D);

            int[] innerUpperEyePointsR = {
                3,  12,
                12, 10,
                3,  2,
                12, 6,
                3,  8
            };
            DrawSpline2D(innerUpperEyePointsR, 5, featurePoints2D);

            int[] innerLowerEyePointsR = {
                3,  8,
                12, 8,
                3,  4,
                12, 12,
                3,  12
            };
            DrawSpline2D(innerLowerEyePointsR, 5, featurePoints2D);

            int[] outerUpperEyePointsL = {
                3,  11,
                3,  13,
                3,  7
            };
            DrawSpline2D(outerUpperEyePointsL, 3, featurePoints2D);

            int[] outerLowerEyePointsL = {
                3,  7,
                3,  9,
                3,  11
            };
            DrawSpline2D(outerLowerEyePointsL, 3, featurePoints2D);

            int[] innerUpperEyePointsL = {
                3,  11,
                12, 9,
                3,  1,
                12, 5,
                3,  7
            };
            DrawSpline2D(innerUpperEyePointsL, 5, featurePoints2D);

            int[] innerLowerEyePointsL = {
                3,  7,
                12, 7,
                3,  3,
                12, 11,
                3,  11
            };
            DrawSpline2D(innerLowerEyePointsL, 5, featurePoints2D);

            int[] eyebrowLinesPointsR = {
                4,  6,
                14, 4,
                4,  4,
                14, 2,
                4,  2
            };
            DrawSpline2D(eyebrowLinesPointsR, 5, featurePoints2D);

            int[] eyebrowLinesPointsL = {
                4,  1,
                14, 1,
                4,  3,
                14, 3,
                4,  5
            };
            DrawSpline2D(eyebrowLinesPointsL, 5, featurePoints2D);

            // visible contour
            int[] contourPointsVisible = {
                13, 1,
                13, 3,
                13, 5,
                13, 7,
                13, 9,
                13, 11,
                13, 13,
                13, 15,
                13, 17,
                13, 16,
                13, 14,
                13, 12,
                13, 10,
                13, 8,
                13, 6,
                13, 4,
                13, 2
            };
            DrawSpline2D(contourPointsVisible, 17, featurePoints2D);

            gl.MatrixMode(OpenGL.GL_PROJECTION);
            gl.PopMatrix();
            gl.MatrixMode(OpenGL.GL_MODELVIEW);
            gl.PopMatrix();

            gl.PopAttrib();

        }

        public void DisplayGaze(FaceData trackingData, int width, int height)
        {
            gl.Viewport(0, 0, width, height);

            gl.PushAttrib(OpenGL.GL_DEPTH_BUFFER_BIT | OpenGL.GL_VIEWPORT_BIT | OpenGL.GL_ENABLE_BIT | OpenGL.GL_FOG_BIT | OpenGL.GL_STENCIL_BUFFER_BIT | OpenGL.GL_TRANSFORM_BIT | OpenGL.GL_TEXTURE_BIT);

            gl.Disable(OpenGL.GL_ALPHA_TEST);
            gl.Disable(OpenGL.GL_DEPTH_TEST);
            gl.Disable(OpenGL.GL_DITHER);
            gl.Disable(OpenGL.GL_FOG);
            gl.Disable(OpenGL.GL_SCISSOR_TEST);
            gl.Disable(OpenGL.GL_STENCIL_TEST);

            gl.MatrixMode(OpenGL.GL_MODELVIEW);
            gl.PushMatrix();
            gl.LoadIdentity();

            gl.MatrixMode(OpenGL.GL_PROJECTION);
            gl.PushMatrix();
            gl.LoadIdentity();


            gl.Ortho(0.0f, 1.0f, 0.0f, 1.0f, -10.0f, 10.0f);

            SetupCamera(width, height, trackingData.CameraFocus);

            gl.ShadeModel(OpenGL.GL_FLAT);

            float[] vertices = {
                0.0f, 0.0f, 0.0f,
                0.0f, 0.0f, 0.04f
            };

            float[] tr = { 0, 0, 0, 0, 0, 0 };

            FDP fdp = trackingData.FeaturePoints3D;

            FeaturePoint leye = fdp.GetFP(3, 5);
            FeaturePoint reye = fdp.GetFP(3, 6);

            if (leye.Defined == 1 && reye.Defined == 1)
            {
                tr[0] = leye.Pos[0];
                tr[1] = leye.Pos[1];
                tr[2] = leye.Pos[2];
                tr[3] = reye.Pos[0];
                tr[4] = reye.Pos[1];
                tr[5] = reye.Pos[2];
            }

            float h_rot = (float)VS_RAD2DEG(trackingData.GazeDirectionGlobal[1] + VS_PI);
            float v_rot = (float)VS_RAD2DEG(trackingData.GazeDirectionGlobal[0]);
            float roll = (float)VS_RAD2DEG(trackingData.GazeDirectionGlobal[2]);

            gl.EnableClientState(OpenGL.GL_VERTEX_ARRAY);

            gl.MatrixMode(OpenGL.GL_MODELVIEW);
            gl.PushMatrix();

            gl.Translate(tr[0], tr[1], tr[2]);
            gl.Rotate(h_rot, 0.0f, 1.0f, 0.0f);
            gl.Rotate(v_rot, 1.0f, 0.0f, 0.0f);
            gl.Rotate(roll, 0.0f, 0.0f, 1.0f);

            gl.LineWidth(2);

            gl.Color(0.941176471, 0.37647058823, 0, 1.0f);

            if (trackingData.EyeClosure[0] > 0.5f)
            {
                gl.VertexPointer(3, 0, vertices);
                gl.DrawArrays(OpenGL.GL_LINES, 0, 2);
            }

            gl.PopMatrix();

            gl.PushMatrix();

            gl.Translate(tr[3], tr[4], tr[5]);
            gl.Rotate(h_rot, 0.0f, 1.0f, 0.0f);
            gl.Rotate(v_rot, 1.0f, 0.0f, 0.0f);
            gl.Rotate(roll, 0.0f, 0.0f, 1.0f);

            if (trackingData.EyeClosure[1] > 0.5f)
            {
                gl.VertexPointer(3, 0, vertices);
                gl.DrawArrays(OpenGL.GL_LINES, 0, 2);
            }

            gl.DisableClientState(OpenGL.GL_VERTEX_ARRAY);

            gl.PopMatrix();

            gl.MatrixMode(OpenGL.GL_PROJECTION);
            gl.PopMatrix();

            gl.MatrixMode(OpenGL.GL_MODELVIEW);
            gl.PopMatrix();

            gl.PopAttrib();
        }

        public void DisplayModelAxes(FaceData trackingData, int width, int height)
        {
            gl.EnableClientState(OpenGL.GL_VERTEX_ARRAY);

            gl.Viewport(0, 0, width, height);

            gl.PushAttrib(OpenGL.GL_DEPTH_BUFFER_BIT | OpenGL.GL_VIEWPORT_BIT | OpenGL.GL_ENABLE_BIT | OpenGL.GL_FOG_BIT | OpenGL.GL_STENCIL_BUFFER_BIT | OpenGL.GL_TRANSFORM_BIT | OpenGL.GL_TEXTURE_BIT);

            gl.Disable(OpenGL.GL_ALPHA_TEST);
            gl.Disable(OpenGL.GL_DEPTH_TEST);
            gl.Disable(OpenGL.GL_DITHER);
            gl.Disable(OpenGL.GL_FOG);
            gl.Disable(OpenGL.GL_SCISSOR_TEST);
            gl.Disable(OpenGL.GL_STENCIL_TEST);

            gl.MatrixMode(OpenGL.GL_MODELVIEW);
            gl.PushMatrix();
            gl.LoadIdentity();

            gl.MatrixMode(OpenGL.GL_PROJECTION);
            gl.PushMatrix();
            gl.LoadIdentity();

            gl.Ortho(0.0f, 1.0f, 0.0f, 1.0f, -10.0f, 10.0f);

            SetupCamera(width, height, trackingData.CameraFocus);

            gl.BlendFunc(OpenGL.GL_SRC_ALPHA, OpenGL.GL_ONE_MINUS_SRC_ALPHA);
            gl.Enable(OpenGL.GL_BLEND);

            gl.ShadeModel(OpenGL.GL_FLAT);

            //rotate and translate into the current coordinate system of the head
            float[] r = trackingData.FaceRotation;
            float[] t = trackingData.FaceTranslation;
            FDP fdp = trackingData.FeaturePoints3D;

            FeaturePoint fp1 = fdp.GetFP(4, 1);
            FeaturePoint fp2 = fdp.GetFP(4, 2);

            gl.Translate((fp1.Pos[0] + fp2.Pos[0]) / 2.0f, (fp1.Pos[1] + fp2.Pos[1]) / 2.0f, (fp1.Pos[2] + fp2.Pos[2]) / 2.0f);
            gl.Rotate(VS_RAD2DEG(r[1] + VS_PI), 0.0f, 1.0f, 0.0f);
            gl.Rotate(VS_RAD2DEG(r[0]), 1.0f, 0.0f, 0.0f);
            gl.Rotate(VS_RAD2DEG(r[2]), 0.0f, 0.0f, 1.0f);

            float[][] coordVertices = {
                new float[] {0.0f,  0.0f,   0.0f ,0.07f, 0.0f,  0.0f},
                new float[]{0.0f,   0.0f,   0.0f, 0.0f, 0.07f,  0.0f},
                new float[]{0.0f,   0.0f,   0.0f, 0.0f, 0.0f,   0.07f}
            };

            float[][] coordColors = {
                new float[]{1.0f, 0.0f, 0.0f, 0.25f},
                new float[]{0.0f, 0.0f, 1.0f, 0.25f},
                new float[]{0.0f, 1.0f, 0.0f, 0.25f}
            };

            gl.LineWidth(2);

            for (int i = 0; i < 3; i++)
            {

                gl.Color(coordColors[i]);
                gl.VertexPointer(3, 0, coordVertices[i]);
                gl.DrawArrays(OpenGL.GL_LINES, 0, 2);

            }

            gl.MatrixMode(OpenGL.GL_PROJECTION);
            gl.PopMatrix();

            gl.MatrixMode(OpenGL.GL_MODELVIEW);
            gl.PopMatrix();

            gl.PopAttrib();
        }

        public void DisplayWireFrame(FaceData trackingData, int width, int height)
        {
            //set image specs
            gl.Viewport(0, 0, width, height);

            gl.MatrixMode(OpenGL.GL_MODELVIEW);
            gl.PushMatrix();
            gl.LoadIdentity();

            gl.MatrixMode(OpenGL.GL_PROJECTION);
            gl.PushMatrix();
            gl.LoadIdentity();

            SetupCamera(width, height, trackingData.CameraFocus);

            gl.EnableClientState(OpenGL.GL_VERTEX_ARRAY);
            gl.ShadeModel(OpenGL.GL_FLAT);
            //set the color for the wireframe
            gl.Color(0.0f, 1.0f, 0.0f, 1.0f);
            //vertex list
            gl.VertexPointer(3, 0, trackingData.FaceModelVertices);

            gl.LineWidth(1);

            float[] r = trackingData.FaceRotation;
            float[] t = trackingData.FaceTranslation;

            gl.Translate(t[0], t[1], t[2]);
            gl.Rotate(VS_RAD2DEG(r[1] + VS_PI), 0.0f, 1.0f, 0.0f);
            gl.Rotate(VS_RAD2DEG(r[0]), 1.0f, 0.0f, 0.0f);
            gl.Rotate(VS_RAD2DEG(r[2]), 0.0f, 0.0f, 1.0f);

            //draw the wireframe
            //initialize indexes for drawing wireframe (once per model)
            if (numberOfVertices != trackingData.FaceModelVertexCount)
            {
                HashSet<KeyValuePair<int, int>> indexList = new HashSet<KeyValuePair<int, int>>();

                for (int i = 0; i < trackingData.FaceModelTriangleCount; i++)
                {
                    int[] triangle = {
                        trackingData.FaceModelTriangles[3*i+0],
                        trackingData.FaceModelTriangles[3*i+1],
                        trackingData.FaceModelTriangles[3*i+2]
                    };

                    if (triangle[0] > triangle[1])
                        Swap(ref triangle[0], ref triangle[1]);
                    if (triangle[0] > triangle[2])
                        Swap(ref triangle[0], ref triangle[2]);
                    if (triangle[1] > triangle[2])
                        Swap(ref triangle[1], ref triangle[2]);

                    indexList.Add(new KeyValuePair<int, int>(triangle[0], triangle[1]));
                    indexList.Add(new KeyValuePair<int, int>(triangle[1], triangle[2]));
                    indexList.Add(new KeyValuePair<int, int>(triangle[0], triangle[2]));
                }

                output.Clear();

                foreach (KeyValuePair<int, int> pair in indexList)
                {
                    output.Add(pair.Key);
                    output.Add(pair.Value);
                }

                numberOfVertices = trackingData.FaceModelVertexCount;
            }

            gl.DrawElements(OpenGL.GL_LINES, output.Count, (uint[])(object)output.ToArray());

            gl.DisableClientState(OpenGL.GL_VERTEX_ARRAY);

            gl.MatrixMode(OpenGL.GL_PROJECTION);
            gl.PopMatrix();

            gl.MatrixMode(OpenGL.GL_MODELVIEW);
            gl.PopMatrix();
        }

        public void DisplayResults(VSImage frame, FaceData trackingData, int trackStat, int width, int height, int drawingOptions, float[] emotions = default(float[]), int gender = -1, float age = 0, string name = "", float[] texCoords = default(float[]))
        {
            if (trackStat == (int)TRACK_STAT.OK)
            {
                if ((drawingOptions & DISPLAY_SPLINES) == DISPLAY_SPLINES)
                {
                    DisplaySplines(trackingData, width, height);
                }

                if ((drawingOptions & DISPLAY_FEATURE_POINTS) == DISPLAY_FEATURE_POINTS)
                {
                    bool drawQuality = ((drawingOptions & DISPLAY_POINT_QUALITY) == DISPLAY_POINT_QUALITY);
                    DisplayFeaturePoints(frame, trackingData, width, height, false, false, drawQuality); // draw 2D feature points
                    //DisplayFeaturePoints(trackingData, width, height, true); // draw 3D feature points
                    //DisplayFeaturePoints(trackingData, width, height, true, true); // draw relative 3D feature points
                }

                if ((drawingOptions & DISPLAY_GAZE) == DISPLAY_GAZE)
                {
                    DisplayGaze(trackingData, width, height);
                }
                if ((drawingOptions & DISPLAY_AXES) == DISPLAY_AXES)
                {
                    DisplayModelAxes(trackingData, width, height);
                }
                if ((drawingOptions & DISPLAY_WIRE_FRAME) == DISPLAY_WIRE_FRAME)
                {
                    DisplayWireFrame(trackingData, width, height);
                }

                bool display_background = true;

                if ((drawingOptions & DISPLAY_EMOTION) == DISPLAY_EMOTION && emotions != null)
                {
                    DisplayEmotion(trackingData, emotions, width, height, display_background);
                    display_background = false;
                }
                if ((drawingOptions & DISPLAY_GENDER) != DISPLAY_GENDER)
                {
                    gender = -1;

                }
                if ((drawingOptions & DISPLAY_AGE) != DISPLAY_AGE)
                {
                    age = -1;
                }
                if ((drawingOptions & DISPLAY_NAME) != DISPLAY_NAME)
                {
                    name = "";
                }
                if ((drawingOptions & DISPLAY_GENDER) == DISPLAY_GENDER || (drawingOptions & DISPLAY_AGE) == DISPLAY_AGE || (drawingOptions & DISPLAY_NAME) == DISPLAY_NAME)
                {
                    DisplayAgeGenderName(trackingData, age, gender, name, width, height, display_background);
                }
                if ((drawingOptions & DISPLAY_TEXTURED_MODEL) == DISPLAY_TEXTURED_MODEL && texCoords != null)
                {
                    DisplayTexturedModel(trackingData, width, height, texCoords);
                }
                if ((drawingOptions & DISPLAY_TRACKING_QUALITY) == DISPLAY_TRACKING_QUALITY)
                {
                    DisplayTrackingQualityBar(trackingData, width, height);
                }
            }
        }

        public void DisplayEmotion(FaceData trackingData, float[] emotions, int width, int height, bool display_background = false)
        {
            //SharpGL uses context size instead of viewPort size for rendering text
            int contextWidth = gl.RenderContextProvider.Width;
            int contextHeight = gl.RenderContextProvider.Height;

            bool _withinConstraints = withinConstraints(ref trackingData);
            float backgroundSize = (_withinConstraints) ? 0.18f : 0.15f;

            FeaturePoint position = ChooseLocation(trackingData, width, height, backgroundSize);

            if (display_background)
                DisplayBackground(position, width, height, backgroundSize);

            gl.Viewport(0, 0, contextWidth, contextHeight);

            gl.MatrixMode(OpenGL.GL_MODELVIEW);
            gl.PushMatrix();
            gl.LoadIdentity();

            gl.MatrixMode(OpenGL.GL_PROJECTION);
            gl.PushMatrix();
            gl.LoadIdentity();

            if (_withinConstraints)
            {
                DisplayEmotionText(position, width, height);
                DisplayEmotionBars(position, emotions, width, height);
            }
            else
            {
                DisplayWarningText(position, width, height);
            }

            gl.MatrixMode(OpenGL.GL_PROJECTION);
            gl.PopMatrix();

            gl.MatrixMode(OpenGL.GL_MODELVIEW);
            gl.PopMatrix();
        }

        public void DisplayAgeGenderName(FaceData trackingData, float age, int gender, string name, int width, int height, bool display_background = true)
        {
            string text = "";
            FeaturePoint fp = trackingData.FeaturePoints2D.GetFP("2.1");

            bool _withinConstraints = withinConstraints(ref trackingData);
            float backgroundSize = (_withinConstraints) ? 0.05f : 0.15f;

            FeaturePoint position = ChooseLocation(trackingData, width, height, backgroundSize);

            if (_withinConstraints)
            {
                if (name != "")
                {
                    text += name;
                }

                if (gender > -1)
                {
                    if (text != "")
                    {
                        text += ", ";
                    }
                    text += ((gender == 0) ? "FEMALE" : "MALE");
                }

                if (age > -1)
                {
                    if (text != "")
                    {
                        text += ", ";
                    }
                    text += "AGE: " + ((age > -1) ? ((int)(age + 0.5f)).ToString() : "");
                }
            }
            else
            {
                if (display_background)
                {
                    DisplayBackground(position, width, height, backgroundSize);
                }
                DisplayWarningText(position, width, height);
            }

            if (text != "" && display_background)
            {
                DisplayBackground(position, width, height, backgroundSize);
            }

            DisplayText(position, text, width, height);
        }


        public void DisplayTexturedModel(FaceData trackingData, int width, int height, float[] tex_coords)
        {
            gl.Viewport(0, 0, width, height);

            gl.MatrixMode(OpenGL.GL_MODELVIEW);
            gl.PushMatrix();
            gl.LoadIdentity();

            gl.MatrixMode(OpenGL.GL_PROJECTION);
            gl.PushMatrix();
            gl.LoadIdentity();

            SetupCamera(width, height, trackingData.CameraFocus);

            gl.Enable(OpenGL.GL_TEXTURE_2D);
            gl.EnableClientState(OpenGL.GL_VERTEX_ARRAY);
            gl.EnableClientState(OpenGL.GL_TEXTURE_COORD_ARRAY);
            gl.Enable(OpenGL.GL_CULL_FACE);
            gl.Enable(OpenGL.GL_BLEND);
            gl.CullFace(OpenGL.GL_FRONT);

            //set texture specs
            gl.TexParameter(OpenGL.GL_TEXTURE_2D, OpenGL.GL_TEXTURE_MIN_FILTER, OpenGL.GL_LINEAR);
            gl.TexParameter(OpenGL.GL_TEXTURE_2D, OpenGL.GL_TEXTURE_MAG_FILTER, OpenGL.GL_LINEAR);
            gl.TexParameter(OpenGL.GL_TEXTURE_2D, OpenGL.GL_TEXTURE_WRAP_S, OpenGL.GL_CLAMP_TO_EDGE);
            gl.TexParameter(OpenGL.GL_TEXTURE_2D, OpenGL.GL_TEXTURE_WRAP_T, OpenGL.GL_CLAMP_TO_EDGE);

            gl.ShadeModel(OpenGL.GL_FLAT);
            //set the color for the wireframe
            gl.Color(0.0f, 1.0f, 0.0f, 1.0f);
            //vertex list
            gl.VertexPointer(3, 0, trackingData.FaceModelVertices);
            //texture coordinate list
            gl.TexCoordPointer(2, OpenGL.GL_FLOAT, 0, tex_coords);

            //clear buffer
            gl.Clear(OpenGL.GL_DEPTH_BUFFER_BIT);
            //color=white -> texture gets the right colors and lighting (no effects)
            gl.Color(1.0f, 1.0f, 1.0f, 1.0f);

            ////Typical Texture Generation Using Data From The Bitmap
            gl.BindTexture(OpenGL.GL_TEXTURE_2D, model_tex_id[0]);

            float[] r = trackingData.FaceRotation;
            float[] t = trackingData.FaceTranslation;

            gl.Translate(t[0], t[1], t[2]);
            gl.Rotate(VS_RAD2DEG(r[1] + VS_PI), 0.0f, 1.0f, 0.0f);
            gl.Rotate(VS_RAD2DEG(r[0]), 1.0f, 0.0f, 0.0f);
            gl.Rotate(VS_RAD2DEG(r[2]), 0.0f, 0.0f, 1.0f);

            //draw textured model
            //initialize indexes for drawing textured model(once per model)
            gl.DrawElements(OpenGL.GL_TRIANGLES, trackingData.FaceModelTriangleCount * 3, trackingData.FaceModelTriangles.Select(x => (uint)x).ToArray());

            gl.Disable(OpenGL.GL_CULL_FACE);
            gl.Disable(OpenGL.GL_TEXTURE_2D);
            gl.DisableClientState(OpenGL.GL_VERTEX_ARRAY);
            gl.DisableClientState(OpenGL.GL_TEXTURE_COORD_ARRAY);

            gl.MatrixMode(OpenGL.GL_PROJECTION);
            gl.PopMatrix();

            gl.MatrixMode(OpenGL.GL_MODELVIEW);
            gl.PopMatrix();
        }

        public void DisplayTrackingQualityBar(FaceData trackingData, int width, int height)
        {
            gl.MatrixMode(OpenGL.GL_MODELVIEW);
            gl.PushMatrix();
            gl.LoadIdentity();

            gl.MatrixMode(OpenGL.GL_PROJECTION);
            gl.PushMatrix();
            gl.LoadIdentity();

            gl.Ortho(0.0f, 1.0f, 0.0f, 1.0f, -10.0f, 10.0f);

            int points_to_draw = 2;
            float[] vertices = new float[6];
            gl.LineWidth(10);

            vertices[0] = 0.1f;
            vertices[1] = 0.1f;
            vertices[2] = 0.0f;
            vertices[3] = 0.25f;
            vertices[4] = 0.1f;
            vertices[5] = 0.0f;
            gl.Color(0.5f, 0.5f, 0.5f, 1.0f);
            gl.EnableClientState(OpenGL.GL_VERTEX_ARRAY);
            gl.VertexPointer(3, 0, vertices);
            gl.DrawArrays(OpenGL.GL_LINES, 0, points_to_draw);
            gl.DisableClientState(OpenGL.GL_VERTEX_ARRAY);

            vertices[0] = 0.1f;
            vertices[1] = 0.1f;
            vertices[2] = 0.0f;
            vertices[3] = 0.1f + trackingData.TrackingQuality * 0.15f;
            vertices[4] = 0.1f;
            vertices[5] = 0.0f;
            gl.Color((1 - trackingData.TrackingQuality), trackingData.TrackingQuality, 0, 1.0f);
            gl.EnableClientState(OpenGL.GL_VERTEX_ARRAY);
            gl.VertexPointer(3, 0, vertices);
            gl.DrawArrays(OpenGL.GL_LINES, 0, points_to_draw);
            gl.DisableClientState(OpenGL.GL_VERTEX_ARRAY);
            gl.LineWidth(1);

            gl.MatrixMode(OpenGL.GL_PROJECTION);
            gl.PopMatrix();

            gl.MatrixMode(OpenGL.GL_MODELVIEW);
            gl.PopMatrix();
        }

        public void SetTextureImage(VSImage image)
        {
            texture = image;
        }

        void DisplayBackground(FeaturePoint fp, int width, int height, float sizePerc)
        {
            const float Y_OFFSET = 0.0f;
            const float X_OFFSET = -0.0f;
            float size = (width > height) ? width * sizePerc : height * sizePerc;

            gl.MatrixMode(OpenGL.GL_MODELVIEW);
            gl.PushMatrix();
            gl.LoadIdentity();

            gl.MatrixMode(OpenGL.GL_PROJECTION);
            gl.PushMatrix();
            gl.LoadIdentity();

            gl.Viewport(0, 0, width, height);

            gl.Ortho(0.0f, width, 0.0f, height, -10.0f, 10.0f);

            if (fp.Defined == 0)
            {
                gl.MatrixMode(OpenGL.GL_PROJECTION);
                gl.PopMatrix();

                gl.MatrixMode(OpenGL.GL_MODELVIEW);
                gl.PopMatrix();

                return;
            }

            float chinX = (int)(fp.Pos[0] * width);
            float chinY = (int)(fp.Pos[1] * height);

            gl.Translate(chinX + X_OFFSET, chinY + Y_OFFSET, 0);

            float barHeight = size;
            float barLength = BCKG_LENGTH_PERC * ((width > height) ? width : height); //1% of the viewport

            //     barWidth
            //x2 -------------- x3
            // |                | barHeight
            //x1 -------------- x4
            //
            float[] auVis = {
               0.0f, 0.0f, 0.0f,            //x1
               0.0f, -barHeight, 0.0f,      //x2
               barLength, -barHeight, 0.0f, //x3
               barLength, 0.0f, 0.0f        //x4
              };

            gl.BlendFunc(OpenGL.GL_SRC_ALPHA, OpenGL.GL_ONE_MINUS_SRC_ALPHA);
            gl.Enable(OpenGL.GL_BLEND);

            gl.EnableClientState(OpenGL.GL_VERTEX_ARRAY);
            gl.VertexPointer(3, 0, auVis);

            gl.Color(0.8f, 0.8f, 0.8f, 0.5f);
            gl.DrawArrays(OpenGL.GL_POLYGON, 0, 4);

            gl.LineWidth(1.0f);
            gl.Color(0, 0, 0, 1.0f);
            gl.DrawArrays(OpenGL.GL_LINE_LOOP, 0, 4);
            gl.LineWidth(2.0f);

            gl.DisableClientState(OpenGL.GL_VERTEX_ARRAY);

            gl.Disable(OpenGL.GL_BLEND);

            gl.MatrixMode(OpenGL.GL_PROJECTION);
            gl.PopMatrix();

            gl.MatrixMode(OpenGL.GL_MODELVIEW);
            gl.PopMatrix();
        }

        void DisplayEmotionText(FeaturePoint fp, int width, int height)
        {
            const float FONT_FACTOR = 0.10f;

            float size = (width > height) ? width * 0.2f : height * 0.2f;

            if (fp.Defined == 0)
            {
                return;
            }

            int chinX = (int)(fp.Pos[0] * width);
            int chinY = (int)(fp.Pos[1] * height);

            string[] emotions = { "Anger", "Disgust", "Fear", "Happiness", "Sadness", "Surprise", "Neutral" };

            //  Now render some text.
            for (int i = 0; i < EMOTIONS_NUM; ++i)
            {
                gl.DrawText(chinX, chinY + (-i - 3) * (int)(size * FONT_FACTOR), 0.0f, 0.0f, 0.0f, "Arial", (int)(size * FONT_FACTOR), emotions[i]);
            }
        }

        void DisplayEmotionBars(FeaturePoint fp, float[] emotions, int width, int height)
        {
            const float FONT_FACTOR = 0.1f;
            float X_OFFSET = 0.13f * ((width > height) ? width : height);

            float size = (width > height) ? width * 0.2f : height * 0.2f;

            gl.Viewport(0, 0, width, height);

            gl.Ortho(0.0f, width, 0.0f, height, -10.0f, 10.0f);

            if (fp.Defined == 0)
            {
                return;
            }

            float chinX = (int)(fp.Pos[0] * width);
            float chinY = (int)(fp.Pos[1] * height);

            float barHeight = (int)(size * FONT_FACTOR); //20% of width or height adjust with FONT_FACTOR and normalized
            float barLength = 0.1f * width; //1% of the viewport
            float spacing = (int)(barHeight * 0.1f); //space between bars - before and after

            gl.Translate(chinX + X_OFFSET, chinY - 1 * (barHeight + 2 * spacing), 0);
            barHeight = barHeight - 2 * spacing;

            //     barWidth
            //x2 -------------- x3
            // |                | barHeight
            //x1 -------------- x4
            //
            float[] auVis = {
               0.0f, 0.0f, 0.0f,            //x1
               0.0f, -barHeight, 0.0f,      //x2
               barLength, -barHeight, 0.0f, //x3
               barLength, 0.0f, 0.0f        //x4
              };

            gl.EnableClientState(OpenGL.GL_VERTEX_ARRAY);
            gl.VertexPointer(3, 0, auVis);

            for (int i = 0; i < emotions.Length; i++)
            {
                gl.Translate(0, -2 * spacing, 0);
                if (emotions[i] < 0.2)
                    continue;
                //adjust bar width
                auVis[6] = auVis[9] = emotions[i] * barLength;
                //adjust bar y position
                auVis[1] = auVis[10] = -(barHeight * (i + 1));
                auVis[4] = auVis[7] = -(barHeight * (i + 1) + barHeight);

                gl.Color(0, 0.443137255f, 0.73725490196f, 1.0f);
                gl.DrawArrays(OpenGL.GL_POLYGON, 0, 4);

                gl.LineWidth(1.3f);

                gl.Color(0, 0, 0, 1.0f);
                gl.DrawArrays(OpenGL.GL_LINE_LOOP, 0, 4);
            }

            gl.DisableClientState(OpenGL.GL_VERTEX_ARRAY);
        }

        void DisplayText(FeaturePoint fp, string text, int width, int height)
        {
            //SharpGL uses context size instead of viewPort size for rendering text
            int contextWidth = gl.RenderContextProvider.Width;
            int contextHeight = gl.RenderContextProvider.Height;

            gl.Viewport(0, 0, contextWidth, contextHeight);

            gl.MatrixMode(OpenGL.GL_MODELVIEW);
            gl.PushMatrix();
            gl.LoadIdentity();

            gl.MatrixMode(OpenGL.GL_PROJECTION);
            gl.PushMatrix();
            gl.LoadIdentity();

            const float FONT_FACTOR = 0.1f;

            float size = (width > height) ? width * 0.2f : height * 0.2f;

            if (fp.Defined == 0)
            {
                gl.MatrixMode(OpenGL.GL_PROJECTION);
                gl.PopMatrix();

                gl.MatrixMode(OpenGL.GL_MODELVIEW);
                gl.PopMatrix();

                return;
            }

            const float X_OFFSET = 1.0f;
            const float Y_OFFSET = 1.2f;

            int chinX = (int)(fp.Pos[0] * width);
            int chinY = (int)(fp.Pos[1] * height);

            gl.DrawText((int)(chinX * X_OFFSET), chinY - (int)(size * FONT_FACTOR * Y_OFFSET), 0.0f, 0.0f, 0.0f, "Arial", size * FONT_FACTOR, text);

            gl.MatrixMode(OpenGL.GL_PROJECTION);
            gl.PopMatrix();

            gl.MatrixMode(OpenGL.GL_MODELVIEW);
            gl.PopMatrix();
        }

        void DisplayWarningText(FeaturePoint fp, int width, int height)
        {
            //SharpGL uses context size instead of viewPort size for rendering text
            int contextWidth = gl.RenderContextProvider.Width;
            int contextHeight = gl.RenderContextProvider.Height;

            gl.Viewport(0, 0, contextWidth, contextHeight);

            gl.MatrixMode(OpenGL.GL_MODELVIEW);
            gl.PushMatrix();
            gl.LoadIdentity();

            gl.MatrixMode(OpenGL.GL_PROJECTION);
            gl.PushMatrix();
            gl.LoadIdentity();

            const float FONT_FACTOR = 0.10f;

            float size = (width > height) ? width * 0.2f : height * 0.2f;

            if (fp.Defined == 0)
            {
                return;
            }

            int chinX = (int)(fp.Pos[0] * width);
            int chinY = (int)(fp.Pos[1] * height);

            string[] message = { "Face analysis and face", "recognition are available", "only in frontal pose,", "and when face is not", "too small in the image." };

            //  Now render some text.
            for (int i = 0; i < message.Count(); ++i)
            {
                gl.DrawText(chinX, chinY + (-i - 2) * (int)(size * FONT_FACTOR), 1.0f, 0.0f, 0.0f, "Arial", (int)(size * FONT_FACTOR), message[i]);
            }


            gl.MatrixMode(OpenGL.GL_PROJECTION);
            gl.PopMatrix();

            gl.MatrixMode(OpenGL.GL_MODELVIEW);
            gl.PopMatrix();
        }

        FeaturePoint ChooseLocation(FaceData trackingData, int width, int height, float backgroundSize)
        {
            FDP fdp = trackingData.FeaturePoints2D;

            FeaturePoint down = fdp.GetFP("2.1");
            FeaturePoint left = fdp.GetFP("4.5");
            FeaturePoint right = fdp.GetFP("4.6");
            FeaturePoint up = fdp.GetFP("11.1");
            FeaturePoint location = new FeaturePoint();

            int temporaryPosition = 0;
            float backgroundHeight = (width > height) ? width * backgroundSize : height * backgroundSize;
            float backgroundWidth = BCKG_LENGTH_PERC * ((width > height) ? width : height);

            if (up.Pos[1] * height - backgroundHeight < height)
            {
                if (currentPosition == (int)DISPLAY_POS.UP)
                    return up;
                else
                {
                    location = up;
                    temporaryPosition = (int)DISPLAY_POS.UP;
                }

            }

            if (right.Pos[0] * width > backgroundWidth)
            {
                right.Pos[0] = right.Pos[0] - BCKG_LENGTH_PERC;

                if (currentPosition == (int)DISPLAY_POS.RIGHT)
                    return right;
                else
                {
                    location = right;
                    temporaryPosition = (int)DISPLAY_POS.RIGHT;
                }
            }

            if ((left.Pos[0] * width + backgroundWidth) < width)
            {
                if (currentPosition == (int)DISPLAY_POS.LEFT)
                    return left;
                else
                {
                    location = left;
                    temporaryPosition = (int)DISPLAY_POS.LEFT;
                }
            }

            if ((down.Pos[1] * height > backgroundHeight) && ((down.Pos[0] * width + backgroundWidth) < width))
            {
                currentPosition = (int)DISPLAY_POS.DOWN;
                return down;
            }

            currentPosition = temporaryPosition;
            return location;

        }
    }
}
