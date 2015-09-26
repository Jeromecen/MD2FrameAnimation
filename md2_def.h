/*
Author:  JeromeCen
Date:  2015.8.20
Description: MD2ģ���ļ���ʽ�ĳ����ͽṹ�嶨�壬����md2_mesh_loader��ȡģ���е��ã��ο��ԣ�
https://linux.ucla.edu/~phaethon/q3a/formats/md2-schoenblum.html
http://www.cppblog.com/liangairan/articles/60607.html.
*/
#ifndef MD2_DEF_H_
#define MD2_DEF_H_
//// magic number "IDP2" or 844121161
#define MD2_IDENT                (('2'<<24) + ('P'<<16) + ('D'<<8) + 'I')
// model version
#define MD2_VERSION              8
// number of precalculated normals
#define NUMVERTEXNORMALS        162
#define MD2_ACTION_TYPE         21

// vector
typedef float vec3_t[3];

// vertex
typedef struct
{
	unsigned char   v[3];                // compressed vertex (x, y, z) coordinates
	unsigned char   lightnormalindex;    // index to a normal vector for the lighting
} vertex_t;

// texture coordinates
typedef struct
{
	short    s;
	short    t;
} texCoord_t;

// triangle
typedef struct
{
	short   index_xyz[3];    // indexes to triangle's vertices
	short   index_st[3];     // indexes to vertices' texture coorinates
} triangle_t;

// frame
typedef struct
{
	float       scale[3];       // scale values
	float       translate[3];   // translation vector
	char        name[16];       // frame name
	vertex_t    verts[1];       // first vertex of this frame
} frame_t;

// md2 header
typedef struct
{
	int     ident;              // magic number. must be equal to "IDP2"
	int     version;            // md2 version. must be equal to 8

	int     skinwidth;          // width of the texture
	int     skinheight;         // height of the texture
	int     framesize;          // size of one frame in bytes

	int     num_skins;          // number of textures����ͼ�ļ���һ�㲻��
	int     num_xyz;            // number of vertices��һ֡�������Ķ����������ڸ��������ζ��������õ�ֵ
	int     num_st;             // number of texture coordinates��һ֡�е���������ϵ�������ڸ��������ζ��������õ�ֵ
	int     num_tris;           // number of triangles,һ֡�е����ж�������Ҫnum_tris * 3
	int     num_glcmds;         // number of opengl commands,glcommand��һ֡�е�����������
	int     num_frames;         // total number of frames,�ؼ�֡��

	int     ofs_skins;          // offset to skin names (64 bytes each)
	int     ofs_st;             // offset to s-t texture coordinates
	int     ofs_tris;           // offset to triangles
	// we will uncompress vertices using frame data. Here is the data!
	int     ofs_frames;         // offset to frame data
	/*num_glcmds is the number of OpenGL command. The GL command list is an array of integers 
	that allows us to render the model using only triangle fans and 
	triangle strip (GL_TRIANGLE_STRIP andGL_TRIANGLE_FAN), instead of classic triangles (GL_TRIANGLES). */
	int     ofs_glcmds;         // offset to opengl commands
	int     ofs_end;            // offset to end of file

} md2_t;

// animation
typedef struct
{
	int     first_frame;            // first frame of the animation
	int     last_frame;             // number of frames
	int     fps;                    // number of frames per second
} anim_t;

// animation state
typedef struct
{
	int     startframe;              // first frame
	int     endframe;                // last frame
	int     fps;                     // frame per second for this animation
	int     type;                    // animation type

	float   curr_time;               // current time
	float   old_time;                // old time
	float   curr_interpol;                // percent of interpolation
	float   pre_interpol;
	

	int     curr_frame;              // current frame
	int     next_frame;              // next frame

} animState_t;

// ��Ⱦ�õ���������
typedef struct tagTexCoordForRender
{
	float u;
	float v;
}TexCoordForRender;

// һ֡�Ķ������ݣ������ʽΪ
//pID3DDevice->SetFVF(D3DFVF_XYZ | D3DFVF_NORMAL | D3DFVF_DIFFUSE | D3DFVF_TEX1);
typedef struct tagFrameMesh
{
	// ת����Ķ������ݣ���Ҫ��ֵ���
	vec3_t *m_pos;
	// ���㷨��������Ҫ��ֵ���
	vec3_t *m_normal;
	// �����������ݣ�����md2�ļ�ʱ��ȷ��
	TexCoordForRender *m_coord;
	// ��ɫ�����ﲻ���м��㣬����md2�ļ�ʱ��ȷ��
	unsigned int *m_color;

	// �������棬����md2�ļ�ʱ��ȷ��
	unsigned short *m_indices;

	tagFrameMesh()
	{
		Reset();
	};
	void Reset()
	{
		m_pos = 0;
		m_normal = 0;
		m_coord = 0;
		m_color = 0;
		m_indices = 0;
	};
	tagFrameMesh(int nPos, int nNormals, int nCoords, int nColors, int nIndices)
	{
		m_pos = new vec3_t[nPos];
		m_normal = new vec3_t[nNormals];
		m_coord = new TexCoordForRender[nCoords];
		m_color = new unsigned int[nColors];
		m_indices = new unsigned short[nIndices];
	};
	void Release()
	{
		delete [] m_pos;
		delete [] m_normal;
		delete [] m_coord;
		delete [] m_color;
		delete [] m_indices;
		Reset();
	};
}MD2FrameMesh;

template<typename T>
void SafeDeleteArray(T pArrayMemory)
{
	if( pArrayMemory != 0 )
	{
		delete [] pArrayMemory;
		pArrayMemory = 0;
	}
};

#endif