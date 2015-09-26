#include "d3d_utility.h"
#include "md2_animation.h"
// Globals
IDirect3DDevice9* Device = 0; 

const int Width  = 640;
const int Height = 480;

IDirect3DVertexBuffer9* VB = NULL; // vertex buffer to store
                                      // our triangle data.
IDirect3DIndexBuffer9* IB = NULL;

CMD2Animation *pAnimationObj;
static const char *pFileName = "./media/sydney.md2";
static const char *pTextureFile = "./media/sydney.bmp";
IDirect3DTexture9 * pTexture = NULL;

// 顶点格式:
//pID3DDevice->SetFVF(D3DFVF_XYZ | D3DFVF_NORMAL | D3DFVF_DIFFUSE | D3DFVF_TEX1);
// 如果用Device->DrawIndexedPrimitiveUp那么不需要CreateVertexBuffer会有更好的效率，因为内存申请数量不需要双倍
// 如果读者有兴趣可以自己修改练手下。
struct Vertex
{
	Vertex(){}

	Vertex(float x, float y, float z)
	{
		_x = x;	 _y = y;  _z = z;
	}

	float _x, _y, _z;
	float nx, ny, nz;
	unsigned int uColor;
	float cu, cv;

	static const DWORD FVF;
};

const DWORD Vertex::FVF = D3DFVF_XYZ | D3DFVF_NORMAL | D3DFVF_DIFFUSE | D3DFVF_TEX1;
//
// Framework Functions
//
bool Setup()
{
	pAnimationObj = new CMD2Animation();
	if( pAnimationObj == NULL || !pAnimationObj->LoadModel(pFileName) )
	{
		return false;
	}
	
	if( FAILED( Device->CreateVertexBuffer(pAnimationObj->GetVerticePerFrame() * sizeof(Vertex), 
		                       D3DUSAGE_WRITEONLY, 
							   Vertex::FVF,
							   D3DPOOL_MANAGED,
							   &VB,
							   NULL) ) )
	{
		return false;
	}
	
	if( FAILED( Device->CreateIndexBuffer(pAnimationObj->GetIndicePerFrame() * sizeof(short), 
		                       D3DUSAGE_WRITEONLY,
							   D3DFMT_INDEX16,
							   D3DPOOL_MANAGED,
							   &IB,
							   NULL) ) )
	{
		return false;
	}

	if( FAILED( D3DXCreateTextureFromFile(Device, pTextureFile, &pTexture) ) )
	{
		return false;
	}

	short *pIndex =  NULL;
	IB->Lock(0, 0, (void**)&pIndex, 0);
	memcpy(pIndex, pAnimationObj->GetCurFrameMeshData()->m_indices, pAnimationObj->GetIndicePerFrame() * sizeof(short) );
	IB->Unlock();

	D3DXMATRIX WMatrix;
	D3DXMatrixIdentity(&WMatrix);
	Device->SetTransform(D3DTS_WORLD, &WMatrix);

	D3DXMATRIX V;
	D3DXVECTOR3 eye(0.0f, 40.0f, -1.0f);
	D3DXVECTOR3 look(0.0f, 5.0f, 0.0f);
	D3DXVECTOR3 up(0.0f, 1.0f, 0.0f);
	D3DXMatrixLookAtLH(&V, &eye, &look, &up);
	Device->SetTransform(D3DTS_VIEW, &V);

	// Set the projection matrix.
	D3DXMATRIX proj;
	D3DXMatrixPerspectiveFovLH(
			&proj,                        // result
			D3DX_PI * 0.5f,               // 90 - degrees
			(float)Width / (float)Height, // aspect ratio
			1.0f,                         // near plane
			1000.0f);                     // far plane

	Device->SetTransform(D3DTS_PROJECTION, &proj);
	// 禁用光照
	Device->SetRenderState(D3DRS_LIGHTING, FALSE);
	// 绕序
	Device->SetRenderState(D3DRS_CULLMODE, D3DCULL_CW);

	/*Device->SetRenderState(D3DRS_ZWRITEENABLE, TRUE);
	Device->SetRenderState(D3DRS_ZENABLE, TRUE);*/
	// 读者可以在这里改变值动作类型观看现象
	pAnimationObj->SetAnim(CMD2Animation::STAND, timeGetTime()* 0.001f);
	return true;
}
void Cleanup()
{
	if( pAnimationObj != NULL )
	{
		pAnimationObj->Release();
		pAnimationObj = NULL;
	}
}

bool Display(float timeDelta)
{
	if( Device )
	{
		static int nCurAnimateState = 0;
		if (nCurAnimateState != d3d::g_AnimationState)
		{ 
			pAnimationObj->SetAnim(CMD2Animation::animType_t(d3d::g_AnimationState), timeGetTime()* 0.001f);
			nCurAnimateState = d3d::g_AnimationState;
		}
		
		pAnimationObj->Animate(timeDelta);
	    D3DCOLOR uColor = D3DCOLOR_ARGB(255, 100,101, 140);
		Device->Clear(0, 0, D3DCLEAR_TARGET | D3DCLEAR_ZBUFFER, uColor, 1.0f, 0);
		Device->BeginScene();
		MD2FrameMesh* pCurFrameData = pAnimationObj->GetCurFrameMeshData();

		Vertex *pData =  NULL;
		VB->Lock(0, 0, (void**)&pData, 0);
		for( int i = 0; i < pAnimationObj->GetVerticePerFrame(); i++ )
		{
			pData[i]._x = pCurFrameData->m_pos[i][0];
			pData[i]._y = pCurFrameData->m_pos[i][1];
			pData[i]._z = pCurFrameData->m_pos[i][2];

			pData[i].nx = pCurFrameData->m_normal[i][0];
			pData[i].ny = pCurFrameData->m_normal[i][1];
			pData[i].nz = pCurFrameData->m_normal[i][2];

			pData[i].uColor = pCurFrameData->m_color[i];
			pData[i].cu = pCurFrameData->m_coord[i].u;
			pData[i].cv = pCurFrameData->m_coord[i].v;
		}

		VB->Unlock();
		Device->SetStreamSource(0, VB, 0, sizeof(Vertex));
		Device->SetIndices(IB);
		Device->SetFVF(Vertex::FVF);
		Device->SetTexture(0, pTexture);

		Device->DrawIndexedPrimitive(D3DPT_TRIANGLELIST, 
			                         0, 
									 0,
									 pAnimationObj->GetVerticePerFrame(),
									 0, 
									 int(pAnimationObj->GetIndicePerFrame() / 3));
		Device->EndScene();
		Device->Present(0, 0, 0, 0);
	}
	return true;
}


//
// WndProc
//
LRESULT CALLBACK d3d::WndProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
	switch( msg )
	{
	case WM_DESTROY:
		::PostQuitMessage(0);
		break;
		
	case WM_KEYDOWN:
		if( wParam == VK_ESCAPE )
			::DestroyWindow(hwnd);
		break;
	}
	return ::DefWindowProc(hwnd, msg, wParam, lParam);
}

//
// WinMain
//
int WINAPI WinMain(HINSTANCE hinstance,
				   HINSTANCE prevInstance, 
				   PSTR cmdLine,
				   int showCmd)
{
	if(!d3d::InitD3D(hinstance,
		Width, Height, true, D3DDEVTYPE_REF/*D3DDEVTYPE_HAL*/, &Device))
	{
		::MessageBox(0, "InitD3D() - FAILED", 0, 0);
		return 0;
	}
		
	if(!Setup())
	{
		::MessageBox(0, "Setup() - FAILED", 0, 0);
		return 0;
	}

	d3d::EnterMsgLoop( Display );

	Cleanup();

	Device->Release();

	return 0;
}