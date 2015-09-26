/*
Author:  JeromeCen
Date:  2015.8.21
Description: MD2ģ���ļ���ʽ�ļ��Ķ�ȡ������md2_animation����ģ���е��ã��ο��ˣ�
http://www.cppblog.com/liangairan/articles/60607.html �� ���������� http://irrlicht.sourceforge.net/ 
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

	// ������Ϣ����֡�ṹ����Ϣ���������йؼ�֡�Ķ�����Ϣ����һ��num_xyz * num_frames������
	vec3_t          *m_pVerticesForAllKeyFrame; 
	// ���㷨��������֡�ṹ����Ϣ���������йؼ�֡�Ķ�����Ϣ����һ��num_xyz * num_frames������
	int             *m_pLightnormalsIndexForVertiece;

	// ��ǰ֡�����ݣ���md2���غõ��ȴ�źã��ͷ��䶥���ڴ����ڲ�ֵ
	MD2FrameMesh    *m_pCurFrameMesh;
};

#endif