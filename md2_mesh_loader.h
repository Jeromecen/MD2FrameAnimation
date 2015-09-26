/*
Author:  JeromeCen
Date:  2015.8.21
Description: MD2模型文件格式文件的读取，用于md2_animation动画模块中调用，参考了：
http://www.cppblog.com/liangairan/articles/60607.html 和 鬼火引擎代码 http://irrlicht.sourceforge.net/ 
*/
#ifndef MD2_MESH_LOADER_H_
#define  MD2_MESH_LOADER_H_
#include "md2_def.h"

class CMD2Mesh_Loader
{
public:
	CMD2Mesh_Loader();
	~CMD2Mesh_Loader();
public:
	bool LoadMD2File(const char *pFilePath);
	void Release();
	inline int GetFrames(){return m_nKeyFrames;}
	inline int GetVerticesPerFrame(){return m_nVerticesPerFrame;}
	inline int GetTrianglesPerFrame(){return m_nTrianglesPerFrame;}
	inline vec3_t* GetVerticesForAllKeyFrame(){ return m_pVerticesForAllKeyFrame;}
	inline int* GetLightNormalsIndexForVertiece(){return m_pLightnormalsIndexForVertiece;}
	inline MD2FrameMesh* GetCurFrameMeshData() {return m_pCurFrameMesh;}
private:
	int             m_nKeyFrames;
	int             m_nVerticesPerFrame;
	int             m_nTrianglesPerFrame;

	// 顶点信息，从帧结构体信息解析出所有关键帧的顶点信息，是一个num_xyz * num_frames的数组
	vec3_t          *m_pVerticesForAllKeyFrame; 
	// 顶点法向量，从帧结构体信息解析出所有关键帧的顶点信息，是一个num_xyz * num_frames的数组
	int             *m_pLightnormalsIndexForVertiece;

	// 当前帧的数据，由md2加载好的先存放好，和分配顶点内存用于插值
	MD2FrameMesh    *m_pCurFrameMesh;
};

#endif