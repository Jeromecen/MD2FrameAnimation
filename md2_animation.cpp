#include "md2_animation.h"
#include <iostream>
#include <fstream>
#include <Windows.h>

// precalculated normal vectors
vec3_t   CMD2Animation::anorms[ NUMVERTEXNORMALS ] = {
#include    "anorms.h"
};
// precalculated dot product results
/////////////////////���ճ�������ʱû�ã�Ԥ���Ľ�����Ϊû���ҵ�anorms_dots�еĹ���Ԥ���㶥�㷨������ָ���Դ�ı����Quake II��/////
//float    CMD2Animation::anorms_dots[ SHADEDOT_QUANT ][256] = {
//#include    "anormtab.h"
//};
//static float    *shadedots = CMD2Animation::anorms_dots[0];
//static vec3_t   lcolor;
//static const float   FLT_EPSILON_  = 1.192092896e-07F;
//vec3_t          g_lightcolor    = { 1.0, 1.0, 1.0 };
//int             g_ambientlight  = 32;
//float           g_shadelight    = 128;
//float           g_angle         = 0.0;

// ----------------------------------------------
// initialize the 21 MD2 model animations.
// ---------------------------------------------
// animation list��ʵ������л���õ��߻�csv����,md2��Ҳ�ǹ̶���
anim_t CMD2Animation::animlist[ 21 ] = 
{
	// first, last, keyframe fps
	{   0,  39,  9 },   // STAND
	{  40,  45, 10 },   // RUN
	{  46,  53, 10 },   // ATTACK
	{  54,  57,  7 },   // PAIN_A
	{  58,  61,  7 },   // PAIN_B
	{  62,  65,  7 },   // PAIN_C
	{  66,  71,  7 },   // JUMP
	{  72,  83,  7 },   // FLIP
	{  84,  94,  7 },   // SALUTE
	{  95, 111, 10 },   // FALLBACK
	{ 112, 122,  7 },   // WAVE
	{ 123, 134,  6 },   // POINT
	{ 135, 153, 10 },   // CROUCH_STAND
	{ 154, 159,  7 },   // CROUCH_WALK
	{ 160, 168, 10 },   // CROUCH_ATTACK
	{ 196, 172,  7 },   // CROUCH_PAIN
	{ 173, 177,  5 },   // CROUCH_DEATH
	{ 178, 183,  7 },   // DEATH_FALLBACK
	{ 184, 189,  7 },   // DEATH_FALLFORWARD
	{ 190, 197,  7 },   // DEATH_FALLBACKSLOW
	{ 198, 198,  5 },   // BOOM
};
// ----------------------------------------------
// constructor - reset all data.
// ----------------------------------------------
CMD2Animation::CMD2Animation( void )
{
	m_scale         = 1.0;
	m_secondsPerKeyFrame = 0.0f;
	m_scaleLimit = 1.0f / 6.0f;
	SetFPS ( 60 );
	SetAnim( 0 );
}

// ----------------------------------------------
// destructor - free allocated memory.
// ----------------------------------------------
CMD2Animation::~CMD2Animation( void )
{
	m_pMeshLoader.Release();
}

// ----------------------------------------------
// LoadModel() - load model from file.
// ----------------------------------------------
bool CMD2Animation::LoadModel( const char *filename )
{
	return m_pMeshLoader.LoadMD2File(filename);
}

void  CMD2Animation::Release()
{
	m_pMeshLoader.Release();
}

// ----------------------------------------------
// LoadSkin() - load model texture.
// ----------------------------------------------

// ----------------------------------------------
// ProcessLighting() - process all lighting calculus.
// ----------------------------------------------
//void CMD2Animation::ProcessLighting( void )
//{
//	// �û�����ǿ�Ⱥ���Ӱǿ�ȣ����������ǿ��
//	float lightvar = (float)((g_shadelight + g_ambientlight)/256.0);
//
//	// ����ǿ�ȳ��Թ���ɫ���õ�ȫ�ֹ��յ���ɫ
//	lcolor[0] = g_lightcolor[0] * lightvar;
//	lcolor[1] = g_lightcolor[1] * lightvar;
//	lcolor[2] = g_lightcolor[2] * lightvar;
//	// The formula is quite obscure, don't worry about it, it works fine it's all we want ;-) 
//	// It comes from the Quake II's source code.
//	// anorms_dots�洢���㷨�����͹�Դ�ĵ��Ҳ����cos�Ƕ�ֵԽ�������Խǿ
//	// �����±����Բ�ֵ���Ƕ�Խ��ȡ��Խ���ȡ���±�Խ�󣬵��Ǵ洢�ĽǶ�ֵȷ����anorms_dotsȷ��
//	//shadedots = anorms_dots[ ((int)(g_angle * (SHADEDOT_QUANT / 360.0))) & (SHADEDOT_QUANT - 1) ];
//}

// ----------------------------------------------
// SetAnim() - initialize m_anim from the specified
// animation.
// ----------------------------------------------
void CMD2Animation::SetAnim( int type, float curTime/* = 0.0f*/)
{
	if( (type < 0) || (type > MAX_ANIMATIONS) )
		type = 0;
	m_anim.startframe   = animlist[ type ].first_frame;
	m_anim.endframe     = animlist[ type ].last_frame;
	m_anim.curr_frame   = m_anim.startframe;
	m_anim.next_frame   = animlist[ type ].first_frame + 1;
	m_anim.fps          = animlist[ type ].fps;
	m_anim.type         = type;
	m_anim.old_time     = curTime;
	m_secondsPerKeyFrame = 1.0 / float(m_anim.fps);
	// ������ֵʱ����ٽ���ⲻ�ϵĲ�ֵ
	m_scaleLimit = m_secondsPerFrame / m_secondsPerKeyFrame;

	m_anim.curr_interpol = 0.0f;
	m_anim.pre_interpol =  -m_scaleLimit;
}

void CMD2Animation::SetFPS(int nFps/* = 60*/)
{
	if( nFps <= 0 )
	{
		return;
	}
	m_secondsPerFrame = 1.0f / nFps;
}

// ----------------------------------------------
// Animate() - calculate the current frame, next
// frame and interpolation percent.
// ----------------------------------------------
void CMD2Animation::Animate( float time )
{
	m_anim.curr_time = time;

	// calculate current and next frames
	// ��¼����ʱ��������֡������
	float fFlyTime = m_anim.curr_time - m_anim.old_time;
	if( fFlyTime > m_secondsPerKeyFrame )
	{
		m_anim.curr_frame = m_anim.next_frame;
		m_anim.next_frame++;

		if( m_anim.next_frame > m_anim.endframe )
		{
				m_anim.next_frame = m_anim.startframe;
		}
		m_anim.curr_interpol = 0.0f;
		m_anim.pre_interpol = -m_scaleLimit;
		m_anim.old_time = m_anim.curr_time;
	}
	else
	{
		float fScale = fFlyTime / m_secondsPerKeyFrame;
		// irrlicht���������֡��ֵ��ʽ����Ϊ���㾫�����⻹�ǻ�̫����Ӳ
		/*int nBigScale = int(fScale * 10000.0f);
		m_anim.curr_interpol = float( 1.0f * (nBigScale % 4) / 4.0f );*/
		m_anim.curr_interpol = fScale;
	}

	// ��֡������ʱ��Ž��в�ֵ������Ƶ����ֵ
	if( m_anim.curr_interpol - m_anim.pre_interpol >= m_scaleLimit )
	{
		Interpolate();
		m_anim.pre_interpol = m_anim.curr_interpol;
	}
}

// ----------------------------------------------
// Interpolate() - interpolate and scale vertices
// from the current and the next frame.
// ----------------------------------------------
//Normaly vertex interpolation have nothing to do with scaling, but because for the moment we are not animating, 
//theInterpolate() function will only scale vertices.
//Later we'll rewrite it to really interpolate vertices from two frames. 
void CMD2Animation::Interpolate()
{
	vec3_t  *curr_v;    // pointeur to current frame vertices
	vec3_t  *next_v;    // pointeur to next frame vertices

	// create current frame and next frame's vertex list
	// from the whole vertex list
	curr_v = &(m_pMeshLoader.GetVerticesForAllKeyFrame()[ m_pMeshLoader.GetVerticesPerFrame() * m_anim.curr_frame ]);
	next_v = &(m_pMeshLoader.GetVerticesForAllKeyFrame()[ m_pMeshLoader.GetVerticesPerFrame() * m_anim.next_frame ]);

	int *curr_Index = &(m_pMeshLoader.GetLightNormalsIndexForVertiece()[ m_pMeshLoader.GetVerticesPerFrame() * m_anim.curr_frame ]);
	int *next_Index = &(m_pMeshLoader.GetLightNormalsIndexForVertiece()[ m_pMeshLoader.GetVerticesPerFrame() * m_anim.next_frame ]);
	vec3_t curr_normal;
	vec3_t next_normal;
	for( int i = 0; i < m_pMeshLoader.GetVerticesPerFrame() ; i++ )
	{
		// ����֡���������㷨,��֡���ֵ��ʽ��
		// Xinterpolated = Xinital + InterpolationPercent * (Xfinal - Xinital)
		// m_anim.curr_interpol����0ʱ�򣬵õ���һ֡���ǵĶ������ݾ��ǵ�һ֡ͼƬ���м�֡�������ֵ��
		// �������ʾ��ʵ�ʵ�֡����
		m_pMeshLoader.GetCurFrameMeshData()->m_pos[i][0] = (curr_v[i][0] + m_anim.curr_interpol * (next_v[i][0] - curr_v[i][0])) * m_scale;
		m_pMeshLoader.GetCurFrameMeshData()->m_pos[i][1] = (curr_v[i][1] + m_anim.curr_interpol * (next_v[i][1] - curr_v[i][1])) * m_scale;
		m_pMeshLoader.GetCurFrameMeshData()->m_pos[i][2] = (curr_v[i][2] + m_anim.curr_interpol * (next_v[i][2] - curr_v[i][2])) * m_scale;

		curr_normal[0] = anorms[curr_Index[i]][0];
		curr_normal[1] = anorms[curr_Index[i]][2];
		curr_normal[2] = anorms[curr_Index[i]][1];

		next_normal[0] = anorms[next_Index[i]][0];
		next_normal[1] = anorms[next_Index[i]][2];
		next_normal[2] = anorms[next_Index[i]][1];

		m_pMeshLoader.GetCurFrameMeshData()->m_normal[i][0] = curr_normal[0] + m_anim.curr_interpol * (next_normal[0] - curr_normal[0]);
		m_pMeshLoader.GetCurFrameMeshData()->m_normal[i][1] = curr_normal[1] + m_anim.curr_interpol * (next_normal[1] - curr_normal[1]);
		m_pMeshLoader.GetCurFrameMeshData()->m_normal[i][2] = curr_normal[2] + m_anim.curr_interpol * (next_normal[2] - curr_normal[2]);
	}
}