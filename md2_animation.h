/*
Author:  JeromeCen
Date:  2015.8.23
Description: MD2模型动画和插值算法的实现，用于d3d_render.cpp模型动画渲染模块中调用，参考了：
http://www.cppblog.com/liangairan/articles/60607.html 和 鬼火引擎代码 http://irrlicht.sourceforge.net/ 
比较有特色的是简单的动画插值算法（相比于市场上很多引擎）实现了平滑插值效果，详见：
Animate函数和Interpolate函数。
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
	// 动作宏定义在程序中
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
	// 预计算好的顶点法向量的值
	static vec3_t   anorms[ NUMVERTEXNORMALS ];
	// 静态的动画配置信息
	static anim_t   animlist[MD2_ACTION_TYPE];       // animation list
private:
	void    Interpolate();
	inline bool    FloatEqual(float a, float b);
private:
	CMD2Mesh_Loader m_pMeshLoader;
	// 当前动作，动态的动画配置信息
	animState_t     m_anim; 
	// scale value
	float           m_scale;            
	float          m_secondsPerKeyFrame;
	float          m_secondsPerFrame;
	// 动画插值时候的插值累计量，超过了才插值，避免不断的插值
	float          m_scaleLimit;
};
#endif