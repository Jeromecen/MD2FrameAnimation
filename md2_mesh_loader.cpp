#include "md2_mesh_loader.h"
#include <stdio.h>

CMD2Mesh_Loader::CMD2Mesh_Loader()
{
	m_nKeyFrames = 0;
	m_nVerticesPerFrame = 0;
	m_nTrianglesPerFrame = 0;
	m_pVerticesForAllKeyFrame = NULL; 
	m_pLightnormalsIndexForVertiece = NULL;
	m_pCurFrameMesh = NULL;
}

CMD2Mesh_Loader::~CMD2Mesh_Loader()
{
	Release();
	//SafeDeleteArray<triangle_t*>(m_trianglesPerFrame);
}

void CMD2Mesh_Loader::Release()
{
	SafeDeleteArray<vec3_t*>(m_pVerticesForAllKeyFrame);
	SafeDeleteArray<int*>(m_pLightnormalsIndexForVertiece);
	if( m_pCurFrameMesh != NULL)
	{
		m_pCurFrameMesh->Release();
		m_pCurFrameMesh =  NULL;
	}
}

bool CMD2Mesh_Loader::LoadMD2File(const char *pFilePath)
{
	FILE *pFile = NULL;
	fopen_s(&pFile, pFilePath, "rb");
	if( pFile == NULL)
	{
		return false;
	}
	md2_t           header;
	fread(&header, sizeof(header), 1, pFile);
	if( (header.ident != MD2_IDENT) && (header.version != MD2_VERSION) )
	{
		fclose(pFile);
		return false;
	}
	m_nKeyFrames = header.num_frames;
	m_nTrianglesPerFrame = header.num_tris;
	m_nVerticesPerFrame     = m_nTrianglesPerFrame * 3;
	int nIndexCount = m_nVerticesPerFrame;

	// ��ǰһ֡����Ϣ
	m_pCurFrameMesh = new tagFrameMesh(m_nVerticesPerFrame,
		                               m_nVerticesPerFrame,
									   m_nVerticesPerFrame/*header.num_st*/, 
		                               m_nVerticesPerFrame,
									  nIndexCount);
	if( m_pCurFrameMesh == NULL )
	{
		return false;
	}

	// �������йؼ�֡�Ķ��㻺��Ͷ�����շ�����
	char * buffer = new char[ m_nKeyFrames * header.framesize ];
	if(buffer == NULL )
	{
		return false;
	}
	fseek(pFile, header.ofs_frames, SEEK_SET);
	fread(buffer, m_nKeyFrames * header.framesize, 1, pFile);
	fseek(pFile, header.ofs_tris, SEEK_SET);
	triangle_t *pTriangles = new triangle_t[m_nTrianglesPerFrame];
	fread( pTriangles, sizeof(triangle_t) * m_nTrianglesPerFrame, 1, pFile );

	m_pVerticesForAllKeyFrame = new vec3_t[m_nVerticesPerFrame * m_nKeyFrames];
	m_pLightnormalsIndexForVertiece = new int[m_nVerticesPerFrame * m_nKeyFrames];
	for( int i = 0; i < m_nKeyFrames; i++ )
	{
		// ���һ֡�Ķ�������
		frame_t *pFrameVertexData  = (frame_t *)&buffer[ header.framesize * i ];
		int nBaseIndex = i * m_nVerticesPerFrame;
		//int nIndex = nBaseIndex;
		for( int t = 0; t < header.num_tris; t++ )
		{
			int nTriangleIndex = t * 3;
			for( int k = 0; k < 3; k++ )
			{
				int nIndex = nBaseIndex + nTriangleIndex + k;
				int nVertexIndex = pTriangles[t].index_xyz[k];
				m_pVerticesForAllKeyFrame[nIndex][0] = pFrameVertexData->scale[0] * pFrameVertexData->verts[nVertexIndex].v[0]
				+ pFrameVertexData->translate[0];
				m_pVerticesForAllKeyFrame[nIndex][1] = pFrameVertexData->scale[1] * pFrameVertexData->verts[nVertexIndex].v[1]
				+ pFrameVertexData->translate[1];
				m_pVerticesForAllKeyFrame[nIndex][2] = pFrameVertexData->scale[2] * pFrameVertexData->verts[nVertexIndex].v[2]
				+ pFrameVertexData->translate[2];

				m_pLightnormalsIndexForVertiece[nIndex] = pFrameVertexData->verts[nVertexIndex].lightnormalindex;
			}
		}
		
	}

	// ���������������Ϣ
	
	//texCoord_t *pCoordsPerFrame = new texCoord_t[header.num_st];
	texCoord_t *pFileTextureCoord = new texCoord_t[header.num_st];
	fseek(pFile, header.ofs_st, SEEK_SET);
	fread(pFileTextureCoord, header.num_st * sizeof(texCoord_t), 1, pFile);

	float dmaxs = 1.0f / (header.skinwidth);
	float dmaxt = 1.0f / (header.skinheight);
	if( header.num_frames )
	{
		for( int i = 0; i < m_nTrianglesPerFrame; i++ )
		{
			for( int j = 0; j < 3; j++ )
			{
				int nIndex = i * 3 + j;
				//m_pCurFrameMesh->m_indices[nIndex] = m_pCoordsPerFrame[pTriangles[i].index_xyz[j]];
				// ����0.5����ΪDX9����Ļ����ϵ(0,0)���������ģ�Ҳ����˵�����OGL����ϵ���������¸��ƶ���0.5������
				// ��ô��������ҲҪ�����¸��ƶ�0.5�����ء�
				int nCoordIndex = pTriangles[i].index_st[j];
				if( nCoordIndex >= header.num_st )
				{
					continue;
				}
				m_pCurFrameMesh->m_coord[nIndex].u = (pFileTextureCoord[pTriangles[i].index_st[j]].s + 0.5f) * dmaxs;
				m_pCurFrameMesh->m_coord[nIndex].v = (pFileTextureCoord[pTriangles[i].index_st[j]].t + 0.5f) * dmaxt;
				m_pCurFrameMesh->m_color[nIndex] = 0xffffffff;// ��ɫ
			}
		}
	}

	// ˳��Ĵ����������
	for( int idx =  0; idx < nIndexCount; idx++ )
	{
		m_pCurFrameMesh->m_indices[idx] = idx;
	}
	
	//// gl command �����С����һ��int��ogl���ֵ����
	//file.read( (char *)m_glcmds, num_glcmds * sizeof( int ) );
	delete []pTriangles;
	delete []buffer;
	fclose(pFile);
	return true;
}