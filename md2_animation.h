/*
Author:  JeromeCen
Date:  2015.8.23
Description: MD2ģ�Ͷ����Ͳ�ֵ�㷨��ʵ�֣�����d3d_render.cppģ�Ͷ�����Ⱦģ���е��ã��ο��ˣ�
http://www.cppblog.com/liangairan/articles/60607.html �� ���������� http://irrlicht.sourceforge.net/ 
�Ƚ�����ɫ���Ǽ򵥵Ķ�����ֵ�㷨��������г��Ϻܶ����棩ʵ����ƽ����ֵЧ���������
Animate������Interpolate������
*/
#ifndef MD2_ANIMATION_H_
#define MD2_ANIMATION_H_
#include "md2_def.h"
#include "md2_mesh_loader.h"

class CMD2Animation
{
public:
	CMD2Animation();
	~CMD2Animation();
	// �����궨���ڳ�����
	typedef enum {
		STAND,
		RUN,
		ATTACK,
		PAIN_A,
		PAIN_B,
		PAIN_C,
		JUMP,
		FLIP,
		SALUTE,
		FALLBACK,
		WAVE,
		POINT,
		CROUCH_STAND,
		CROUCH_WALK,
		CROUCH_ATTACK,
		CROUCH_PAIN,
		CROUCH_DEATH, 
		DEATH_FALLBACK,
		DEATH_FALLFORWARD,
		DEATH_FALLBACKSLOW,
		BOOM,
		MAX_ANIMATIONS
	} animType_t;
public:
	bool    LoadModel( const char *filename );
	void    Release();

	void    SetAnim( int type, float curTime = 0.0f);
	void    SetFPS(int nFps = 60);
	void    ScaleModel( float s ) { m_scale = s; }
	void    Animate( float time );
	inline MD2FrameMesh* GetCurFrameMeshData() {return m_pMeshLoader.GetCurFrameMeshData();}
	inline int GetVerticePerFrame() { return m_pMeshLoader.GetVerticesPerFrame();}
	inline int GetIndicePerFrame() { return m_pMeshLoader.GetTrianglesPerFrame() * 3;}

public:
	// Ԥ����õĶ��㷨������ֵ
	static vec3_t   anorms[ NUMVERTEXNORMALS ];
	// ��̬�Ķ���������Ϣ
	static anim_t   animlist[MD2_ACTION_TYPE];       // animation list
private:
	void    Interpolate();
	inline bool    FloatEqual(float a, float b);
private:
	CMD2Mesh_Loader m_pMeshLoader;
	// ��ǰ��������̬�Ķ���������Ϣ
	animState_t     m_anim; 
	// scale value
	float           m_scale;            
	float          m_secondsPerKeyFrame;
	float          m_secondsPerFrame;
	// ������ֵʱ��Ĳ�ֵ�ۼ����������˲Ų�ֵ�����ⲻ�ϵĲ�ֵ
	float          m_scaleLimit;
};
#endif