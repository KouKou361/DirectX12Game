#include "Lib.h"
#include "Lib_highResolutionTimer.h"
#include "Lib_Graphics.h"
#include "Lib_Sprite.h"
#include "Lib_SpriteRenderer.h"
#include "Lib_SpriteDebugRenderer.h"
#include "Lib_SpriteDebug.h"
#include "Lib_Model.h"
#include "Lib_LambertRenderer.h"
#include "Lib_CameraLib.h"
#include "Lib_ModelManager.h"
#include "Lib_Texture.h"
#include "Lib_PlaneRenderer.h"
#include "Lib_Font.h"
#include "Lib_FontRender.h"
#include "Audio/Lib_Audio.h"
#include "Lib_3DDebugRenderer.h"
#include "Lib_ShadowRenderer.h"
#include "Lib_CascadedShadowRenderer.h"
#include "Lib_PBRRenderer.h"
#include "Lib_PointSpriteRenderer.h"
#include "Lib_Effect3DGeometry.h"
#include "ModelCollisionManager.h"
#include "Lib_Collision.h"
#include "Lib_NavMeshRenderer.h"
#include "Lib_NoLightRenderer.h"




#include "imconfig.h"
#include "imgui.h"            
#include "imgui_impl_win32.h"
#include "imgui_impl_dx12.h"
#include "imgui_internal.h"


#include <d3d12.h>
#include <dxgi1_4.h>
#include <tchar.h>

#ifdef _DEBUG
#define DX12_ENABLE_DEBUG_LAYER
#endif

#ifdef DX12_ENABLE_DEBUG_LAYER
#include <dxgidebug.h>
#pragma comment(lib, "dxguid.lib")
#endif
//#include "Lib_3DGeometry.h"

//#include "Lib_Light.h"
//#include "Lib_PMDActor.h"
//#include "Lib_PMDRenderer.h"
//#include "Lib_PeraPolygon.h"
//#include "Lib_ShadowRenderer.h"




#include <memory>
#include <sstream>

using namespace Microsoft::WRL;
using namespace std;
using namespace DirectX;



extern LRESULT ImGui_ImplWin32_WndProcHandler(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);

struct Engine
{
    // ウィンドウハンドル
    HWND hwnd = 0;
    //フレームタイマー
    HighResolutionTimer timer = {};
    //グラフィックス
    Graphics* graphics = nullptr;
    //2Dスプライトレンダー
    SpriteRenderer* spriteRenderer = nullptr;
    //2Dデバッグレンダー
    SpriteDebugRenderer* spriteDebugRenderer = nullptr;

    LambertRenderer* lambertRenderer = nullptr;

    //カメラ
    CameraLib* cameraLib = nullptr;

    //影カメラ
    CameraLib* shadowCamera = nullptr;
    
    Lib_ModelManager* modelResourceManager = nullptr;

    PlaneRenderer* planeRender = nullptr;

    Font* font = nullptr;
    FontRenderer* fontRenderer=nullptr;

    Lib_AudioManager* audioManager = nullptr;

    GeometryDebugRenderer* geometryDebugRenderer = nullptr;

    ShadowRenderer* shadowRendere = nullptr;

    CreateShadowRenderer* createShadowRendere = nullptr;

    CascadedShadowRenderer* cascadedShadowRenderer = nullptr;

    CreateCascadedShadowRenderer* createCascadedShadowRenderer = nullptr;

    Effect3DGeometryRenderer* effect3DGeometryRenderer = nullptr;

    InputController* inputController =nullptr;

    ModelCollisionManager* modelCollisionManager = nullptr;

    PBRRenderer* pbrRenderer = nullptr;

    Lib_NavModelRenderer* navModelRenderer = nullptr;

    NoLightRenderer* noLightRenderer = nullptr;

    bool ImguiDebugFlg = false;

    bool ImguiDebugNavFlg = false;


    ////3Dジオメトリーオブジェクト
    //Model3D* model3D = nullptr;
    //

    ////ライト
    //Lib_Light* light = nullptr;
    //
    ////PMDモデルソース
    //PMDActor* pmdActor = nullptr;
    ////PMDモデルソース
    //PMDActor* pmdActor2 = nullptr;
    ////PMDレンダー
    //PMDRenderer* pmdRenderer = nullptr;
    ////ペラポリゴン
    //PeraPolygon* peraPolygon = nullptr;
    ////シャドウポリゴン
    //ShadowRenderer* shadowRenderer = nullptr;
};
Engine engine;

void CalculateFrameStats()
{
    // Code computes the average frames per second, and also the 
    // average time it takes to render one frame.  These stats 
    // are appended to the window caption bar.
    static int frames = 0;
    static float time_tlapsed = 0.0f;

    frames++;

    // Compute averages over one second period.
    if ((engine.timer.time_stamp() - time_tlapsed) >= 1.0f)
    {
        float fps = static_cast<float>(frames); // fps = frameCnt / 1
        float mspf = 1000.0f / fps;
        std::ostringstream outs;
        outs.precision(6);
        outs << "  FPS : " << fps << " / " << "Frame Time : " << mspf << " (ms)";
        SetWindowTextA(engine.hwnd, outs.str().c_str());

        // Reset for next average.
        frames = 0;
        time_tlapsed += 1.0f;
    }
}

namespace TK_Lib
{

    //ライブラリ初期化
    void LibInit(LPCTSTR caption, int screenWidth, int screenHeight)
    {
        engine.hwnd = Window::Init(caption, screenWidth, screenHeight);

        engine.graphics = new Graphics(engine.hwnd, screenWidth, screenHeight);

         Imgui::Init();
        
        //libData.graphics->Init(libData.hwnd, screenWidth, screenHeight);
        //
        //
        ////PMDモデルソース
        //libData.pmdActor = new PMDActor(libData.graphics, "./Data/Model/UserFile/Model/初音ミクmetal.pmd");
        ////PMDモデルソース
       //// libData.pmdActor2 = new PMDActor(libData.graphics, "./Data/Model/UserFile/Model/巡音ルカ.pmd");
        ////PMDレンダー
        //libData.pmdRenderer = new PMDRenderer(libData.graphics);
        //
        //ID3D12Device* device = libData.graphics->GetDevice();
        //ID3D12CommandQueue* queue = libData.graphics->GetCommandQueue();
        //ID3D12GraphicsCommandList* list = libData.graphics->GetCommandList();
        //ID3D12CommandAllocator* allocator = libData.graphics->GetCommandAllocator();
        //
        //
        //
        //allocator->Reset();
        //list->Reset(allocator, nullptr);
        //
      //engine.sprite = new Sprite(engine.graphics, "./Date/unnamed.png");
      //engine.cameraLib = new CameraLib();
        engine.audioManager = new Lib_AudioManager(engine.hwnd);

      engine.spriteRenderer = new SpriteRenderer(engine.graphics, L"./Lib/Shader/Sprite_vs.hlsl", L"./Lib/Shader/Sprite_ps.hlsl",1000);
      
      engine.fontRenderer=new FontRenderer(engine.graphics, L"./Lib/Shader/Font_vs.hlsl", L"./Lib/Shader/Font_ps.hlsl", 1000);

      engine.spriteDebugRenderer=new SpriteDebugRenderer(engine.graphics, L"./Lib/Shader/SpriteDebug_vs.hlsl", L"./Lib/Shader/SpriteDebug_ps.hlsl");
      
      engine.geometryDebugRenderer = new GeometryDebugRenderer(engine.graphics, L"./Lib/Shader/GeometryDebug_vs.hlsl", L"./Lib/Shader/GeometryDebug_ps.hlsl");
      
      engine.lambertRenderer = new LambertRenderer(engine.graphics, L"./Lib/Shader/Lambert_vs.hlsl", L"./Lib/Shader/Lambert_ps.hlsl");
      
      engine.cameraLib = new CameraLib();

      engine.shadowCamera = new CameraLib();

      engine.modelResourceManager = new Lib_ModelManager();

      engine.planeRender = new PlaneRenderer(engine.graphics, L"./Lib/Shader/Plane_vs.hlsl", L"./Lib/Shader/Plane_ps.hlsl", 1000);
        
      engine.font = new Font(engine.graphics, "./Data/Font/MS Gothic.fnt");

      engine.shadowRendere = new ShadowRenderer(engine.graphics, L"./Lib/Shader/NormalShadowMap_vs.hlsl", L"./Lib/Shader/NormalShadowMap_ps.hlsl");
      engine.createShadowRendere = new CreateShadowRenderer(engine.graphics, L"./Lib/Shader/CreateShadowMap_vs.hlsl", L"./Lib/Shader/CreateShadowMap_ps.hlsl");

      engine.cascadedShadowRenderer = new CascadedShadowRenderer(engine.graphics, L"./Lib/Shader/CascadedShadowRenderer_vs.hlsl", L"./Lib/Shader/CascadedShadowRenderer_ps.hlsl");
      engine.createCascadedShadowRenderer = new CreateCascadedShadowRenderer(engine.graphics, L"./Lib/Shader/CreateCascadedShadowRenderer_vs.hlsl", L"./Lib/Shader/CreateCascadedShadowRenderer_ps.hlsl");
      engine.effect3DGeometryRenderer =new Effect3DGeometryRenderer(engine.graphics, L"./Lib/Shader/PointSprite_vs.hlsl", L"./Lib/Shader/PointSprite_gs.hlsl", L"./Lib/Shader/PointSprite_ps.hlsl");
      engine.inputController = new InputController();
      engine.modelCollisionManager = new ModelCollisionManager(50, 50, DirectX::XMFLOAT2{100,100});

      engine.pbrRenderer = new PBRRenderer(engine.graphics, L"./Lib/Shader/PBR_vs.hlsl", L"./Lib/Shader/PBR_ps.hlsl");

      engine.navModelRenderer = new Lib_NavModelRenderer(engine.graphics, L"./Lib/Shader/NavMesh_vs.hlsl", L"./Lib/Shader/NavMesh_ps.hlsl");

      engine.noLightRenderer = new NoLightRenderer(engine.graphics, L"./Lib/Shader/NoLight_vs.hlsl", L"./Lib/Shader/NoLight_ps.hlsl");
      // //libData.graphics->Wait();
           //
          //// libData.model3D=new Model3D(libData.graphics);
          //// libData.graphics->Wait();
           //
           //libData.cameraLib = new CameraLib();
           //libData.cameraLib->SetLookAt({ 0,10,-15 }, { 0,10,0 }, { 0,1,0 });
           ////  libData.cameraLib->SetLookAt({ 0,0,-5 }, { 0,0,0 }, { 0,1,0 });
           //libData.cameraLib->SetPerspectiveFov(XM_PIDIV2, static_cast<float>(screenWidth) / static_cast<float>(screenHeight), 1.0f, 100.0f);
           //
           //libData.light = new Lib_Light();
           //libData.light->SetLightOrthongraphic(20, 20, 1.0f, 100.0f);
           //libData.light->SetLookAt(VECTOR3(-20, 20, -20), VECTOR3(0, 10, 0), VECTOR3(0, 1, 0));
           //
           //libData.peraPolygon = new PeraPolygon(libData.graphics);
           //
           //libData.shadowRenderer = new ShadowRenderer(libData.graphics);
           //
           //

    //  IMGUI_CHECKVERSION();
     // ImGui::CreateContext();
     

        engine.timer.reset();
       
 
    }

    bool LibLoop(bool isShowFrameRate)
    {
        TK_Lib::Imgui::Update();

        MSG msg = {};

        while (WM_QUIT != msg.message)
        {

            if (PeekMessage(&msg, NULL, 0, 0, PM_REMOVE))
            {
                TranslateMessage(&msg);
                DispatchMessage(&msg);
            }
            else
            {
                engine.timer.tick();
                engine.inputController->Update(engine.hwnd);
                if (isShowFrameRate)
                {
                    CalculateFrameStats();
                }
             
                return true;
            }
        }
        return false;
    }

    void End()
    {
       

        if (engine.spriteRenderer != nullptr)
        {
            delete engine.spriteRenderer;
            engine.spriteRenderer = nullptr;
        }

        if (engine.spriteDebugRenderer != nullptr)
        {
            delete  engine.spriteDebugRenderer;
            engine.spriteDebugRenderer = nullptr;
        }

        if (engine.geometryDebugRenderer != nullptr)
        {
            delete engine.geometryDebugRenderer;
            engine.geometryDebugRenderer = nullptr;
        }

        if (engine.lambertRenderer != nullptr)
        {
            delete engine.lambertRenderer;
            engine.lambertRenderer = nullptr;
        }

        if (engine.cameraLib != nullptr)
        {
            delete engine.cameraLib;
            engine.cameraLib = nullptr;
        }

        if (engine.shadowCamera != nullptr)
        {
            delete engine.shadowCamera;
            engine.shadowCamera = nullptr;
        }
        

        if (engine.modelResourceManager != nullptr)
        {
            delete engine.modelResourceManager;
            engine.modelResourceManager = nullptr;
        }

        if (engine.planeRender!=nullptr)
        {
            delete engine.planeRender;
            engine.planeRender = nullptr;
        }
        if (engine.font != nullptr)
        {
            delete engine.font;
            engine.font = nullptr;
        }
        if (engine.fontRenderer != nullptr)
        {
            delete engine.fontRenderer;
            engine.fontRenderer = nullptr;
        }
        if (engine.audioManager != nullptr)
        {
            delete engine.audioManager;
            engine.audioManager = nullptr;
        }
       
       

        if (engine.graphics != nullptr)
        {
            delete engine.graphics;
            engine.graphics = nullptr;
        }

        if (engine.createShadowRendere != nullptr)
        {
            delete engine.createShadowRendere;
            engine.createShadowRendere = nullptr;
        }

        if (engine.shadowRendere != nullptr)
        {
            delete engine.shadowRendere;
            engine.shadowRendere = nullptr;
        }


        if (engine.cascadedShadowRenderer != nullptr)
        {
            delete engine.cascadedShadowRenderer;
            engine.cascadedShadowRenderer = nullptr;
        }

        if (engine.createCascadedShadowRenderer != nullptr)
        {
            delete engine.createCascadedShadowRenderer;
            engine.createCascadedShadowRenderer = nullptr;
        }

        if (engine.pbrRenderer != nullptr)
        {
            delete engine.pbrRenderer;
            engine.pbrRenderer = nullptr;
        }

        if (engine.noLightRenderer != nullptr)
        {
            delete engine.noLightRenderer;
            engine.noLightRenderer = nullptr;
        }
        
        

        if (engine.effect3DGeometryRenderer != nullptr)
        {
            delete engine.effect3DGeometryRenderer;
            engine.effect3DGeometryRenderer = nullptr;
        }

        if (engine.inputController != nullptr)
        {
            delete engine.inputController;
            engine.inputController = nullptr;
        }
        if (engine.modelCollisionManager != nullptr)
        {
            delete engine.modelCollisionManager;
            engine.modelCollisionManager = nullptr;
        }

        if (engine.navModelRenderer != nullptr)
        {
            delete engine.navModelRenderer;
            engine.navModelRenderer = nullptr;
        }


    

        Imgui::End();

     
    }

    namespace Window
    {
        const LPCWSTR CLASS_NAME = L"3dgp";

        // ウィンドウプロシージャ
        LRESULT CALLBACK fnWndProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam)
        {
            //using DirectX::Keyboard;

          // マウスホイール用
            static int nWheelFraction = 0;	// 回転量の端数

            // マウス移動用
            static POINT prevPos = POINT{ 0,0 };

           if (ImGui_ImplWin32_WndProcHandler(hwnd, msg, wParam, lParam)) return true;
               




            switch (msg)
            {
            case WM_PAINT:
            {
                PAINTSTRUCT ps;
                HDC hdc;
                hdc = BeginPaint(hwnd, &ps);
                EndPaint(hwnd, &ps);
                break;
            }
            case WM_DESTROY:
                PostQuitMessage(0);
                break;
            case WM_ACTIVATEAPP:
                //Keyboard::ProcessMessage(msg, wParam, lParam);
                nWheelFraction = 0;
                break;
            case WM_KEYDOWN:
                if (wParam == VK_ESCAPE)
                {
                    PostMessage(hwnd, WM_CLOSE, 0, 0);
                    break;
                }
            case WM_SYSKEYDOWN:
            case WM_KEYUP:
            case WM_SYSKEYUP:
                //Keyboard::ProcessMessage(msg, wParam, lParam);
                break;
            case WM_ENTERSIZEMOVE:
                // WM_EXITSIZEMOVE is sent when the user grabs the resize bars.
                engine.timer.stop();
                break;
            case WM_EXITSIZEMOVE:
                // WM_EXITSIZEMOVE is sent when the user releases the resize bars.
                // Here we reset everything based on the new window dimensions.
                engine.timer.start();
                break;
            default:
                return DefWindowProc(hwnd, msg, wParam, lParam);
            }

            return 0;
        }

        //ウィンドウ初期化
        HWND Init(const LPCTSTR& caption, const int& screenWidth, const int& screenHeight)
        {

            WNDCLASSEX wcex;
            wcex.cbSize = sizeof(WNDCLASSEX);
            wcex.style = CS_HREDRAW | CS_VREDRAW;
            wcex.lpfnWndProc = fnWndProc;
            wcex.cbClsExtra = 0;
            wcex.cbWndExtra = 0;
            wcex.hInstance = GetModuleHandle(NULL);
            wcex.hIcon = 0;
            wcex.hCursor = LoadCursor(NULL, IDC_ARROW);
            wcex.hbrBackground = (HBRUSH)(COLOR_WINDOW + 1);
            wcex.lpszMenuName = NULL;
            wcex.lpszClassName = CLASS_NAME;
            wcex.hIconSm = 0;
            RegisterClassEx(&wcex);

            // m.windowSize = VECTOR2{ static_cast<float>(screenWidth), static_cast<float>(screenHeight) };

            RECT rc = { 0, 0, screenWidth, screenHeight };
            AdjustWindowRect(&rc, WS_OVERLAPPEDWINDOW, FALSE);
            HWND hwnd = CreateWindow(CLASS_NAME, caption, WS_OVERLAPPEDWINDOW ^ WS_MAXIMIZEBOX ^ WS_THICKFRAME | WS_VISIBLE, CW_USEDEFAULT, CW_USEDEFAULT, rc.right - rc.left, rc.bottom - rc.top, NULL, NULL, GetModuleHandle(NULL), NULL);
            ShowWindow(hwnd, SW_SHOWDEFAULT);
            return hwnd;
        }

        float GetElapsedTime()
        {
            return engine.timer.time_interval();
        }

        HWND GetHwnd()
        {
            return engine.hwnd;
        }
    }


    namespace Render
    {
        void Font(VECTOR2 pos, const wchar_t* string)
        {
            engine.font->Draw(engine.graphics, engine.fontRenderer, pos.x, pos.y, string);
        }

        void SpriteRender(Texture* texture,
            const VECTOR2& pos, const VECTOR2& scale, const VECTOR4& cut,
            const VECTOR4& color, const float& angle)
        {
            engine.spriteRenderer->SetVertex(engine.graphics,
                texture, pos, scale, cut, color, angle
            );

         
        }

        void PlaneRender(Texture* texture, XMFLOAT4X4 mat, XMFLOAT4 cut, XMFLOAT4 color)
        {
            engine.planeRender->SetVertex(engine.graphics,texture,mat,cut,color);
        }

        void ModelRender(Model* model, const SpriteShaderType& type)
        {
            switch (type)
            {
            case SpriteShaderType::NORMAL:
                engine.noLightRenderer->Register(model);
                break;
            case SpriteShaderType::PBR:
                //PBR
                engine.createCascadedShadowRenderer->Register(model);
                engine.pbrRenderer->Register(model);
                break;
            case SpriteShaderType::LAMBERT:
                engine.lambertRenderer->Register(model);
                break;

            case SpriteShaderType::END:
                break;
            }
     

            //カスケード
            //engine.createCascadedShadowRenderer->Register(model);
            //engine.cascadedShadowRenderer->Register(model);

            //シャドウマップ
            //engine.createShadowRendere->Register(model);
            //engine.shadowRendere->Register(model);



           // engine.lambertRenderer->Register(model);
        }

        void NavModelRender(Model* model)
        {
            
        }

        void BeginRender()
        {
      

          //描画開始
            engine.graphics->BeginRender();

   


            engine.graphics->CreateShadowMapMatrix(*engine.cameraLib, *engine.shadowCamera);
            engine.graphics->UpdateCamera(*engine.cameraLib, *engine.shadowCamera);


             engine.graphics->NormalShadowBeginRender();
             
           
             if (engine.createShadowRendere != nullptr)
             {
                 engine.createShadowRendere->AllRender(engine.graphics);
             }

             engine.createCascadedShadowRenderer->Begin(engine.graphics,0);

             engine.graphics->CasCadeShadowBeginRender(0);

             if (engine.createCascadedShadowRenderer != nullptr)
             {
                 engine.createCascadedShadowRenderer->AllRender(engine.graphics);
             }

             engine.createCascadedShadowRenderer->Begin(engine.graphics, 1);

             engine.graphics->CasCadeShadowBeginRender(1);

             if (engine.createCascadedShadowRenderer != nullptr)
             {
                 engine.createCascadedShadowRenderer->AllRender(engine.graphics);
             }

             engine.createCascadedShadowRenderer->Begin(engine.graphics, 2);

             engine.graphics->CasCadeShadowBeginRender(2);

             if (engine.createCascadedShadowRenderer != nullptr)
             {
                 engine.createCascadedShadowRenderer->AllRender(engine.graphics);
             }

             engine.createCascadedShadowRenderer->End(engine.graphics);

         
             
          
        
             engine.graphics->BackBufferBeginRender();
     
             if (engine.cascadedShadowRenderer != nullptr)
             {
                 engine.cascadedShadowRenderer->AllRender(engine.graphics);
             }

             if (engine.pbrRenderer!=nullptr)
             {
                 engine.pbrRenderer->AllRender(engine.graphics);
             }

             if (engine.noLightRenderer != nullptr)
             {
                 engine.noLightRenderer->AllRender(engine.graphics);
             }

             
           

             if (engine.planeRender != nullptr)
             {
                 engine.planeRender->AllRender(engine.graphics);
             }

             if (engine.lambertRenderer != nullptr)
             {
                 engine.lambertRenderer->AllRender(engine.graphics);
             }
             if (engine.navModelRenderer != nullptr)
             {
                 engine.navModelRenderer->AllRender(engine.graphics);
             }

        
             if (engine.effect3DGeometryRenderer != nullptr)
             {
                 engine.effect3DGeometryRenderer->AllRender(engine.graphics, TK_Lib::Window::GetElapsedTime());
             }
             
            if (engine.geometryDebugRenderer != nullptr)
            {
                engine.geometryDebugRenderer->AllRender(engine.graphics);
            }

            if (engine.shadowRendere != nullptr)
            {
                engine.shadowRendere->AllRender(engine.graphics);
            }

            if (engine.spriteRenderer != nullptr)
            {
                engine.spriteRenderer->AllRender(engine.graphics);
            }

            if (engine.fontRenderer != nullptr)
            {
                engine.fontRenderer->AllRender(engine.graphics);
            }

            if (engine.spriteDebugRenderer != nullptr)
            {
                engine.spriteDebugRenderer->AllRender(engine.graphics);
            }

            TK_Lib::Imgui::Render();

    

        }
        void EndRender()
        {
            engine.graphics->EndRender();

         
        }
    }

    namespace Load
    {
        Texture* sprite(const char* filePath)
        {
            return new Texture(engine.graphics,filePath);
        }
        Model* model(const char* filePath)
        {
            ModelResource* modelResource=engine.modelResourceManager->Resiger(engine.graphics, filePath);
            return new Model(engine.graphics, modelResource);
        }
        Lib_AudioBase* Audio(const char* filePath,const char* key)
        {
            return engine.audioManager->Resiger(filePath, key);
        }
        Lib_Effect3D* Effect(Texture* texture, const int createNum)
        {
            return new Lib_Effect3D(engine.graphics, engine.effect3DGeometryRenderer, texture, createNum);
        }
    }

    namespace Audio
    {
        void Play(const char* key, bool loop) { engine.audioManager->SearchAudionKey(key)->Play(loop); }
        void Stop(const char* key) { engine.audioManager->SearchAudionKey(key)->Stop(); }
        void SetVolume(const char* key, const LONG& volume) { engine.audioManager->SearchAudionKey(key)->SetVolume(volume); }
        void SetPan(const char* key,const LONG &pan) { engine.audioManager->SearchAudionKey(key)->SetPan(pan); }
    }

    namespace Camera
    {
        void SetLookAt(
            const VECTOR3& eye,
            const VECTOR3& focus,
            const VECTOR3& up)
        {
            engine.cameraLib->SetLookAt(eye, focus, up);
        }
        
        void SetPerspectiveFov(float fovY, float aspect, float nearZ, float farZ)
        {
            engine.cameraLib->SetPerspectiveFov(fovY, aspect, nearZ, farZ);
        }
        
        const DirectX::XMFLOAT4X4& GetView()
        {
            return engine.cameraLib->GetView();
        }
        
        const DirectX::XMFLOAT4X4& GetProjection()
        {
            return engine.cameraLib->GetProjection();
        }
        
        const CameraLib* GetCamera()
        {
            return engine.cameraLib;
        }
        
        // 視点取得
        const VECTOR3& GetPos()
        {
            return engine.cameraLib->GetPos();
        }
        
        // 注視点取得
        const VECTOR3& GetTarget()
        {
            return engine.cameraLib->GetTarget();
        }
        
        // 上方向取得
        const VECTOR3& GetUp()
        {
            return engine.cameraLib->GetUp();
        }
        
        // 前方向取得
        const VECTOR3& GetFront()
        {
            return engine.cameraLib->GetFront();
        }
        
        // 右方向取得
        const VECTOR3& GetRight()
        {
            return engine.cameraLib->GetRight();
        }
        
    }

    namespace Light
    {
        void SetLookAt(
            const VECTOR3& eye,
            const VECTOR3& focus,
            const VECTOR3& up)
        {
            engine.shadowCamera->SetLookAt(eye, focus, up);
        }
        
        void SetLightOrthongraphic(float Width, float Height, float Near, float Far)
        {
            engine.shadowCamera->SetOrthongraphic(Width, Height, Near, Far);
        }
        
        const DirectX::XMFLOAT4X4& GetView()
        {
            return engine.shadowCamera->GetView();
        }
        
        const DirectX::XMFLOAT4X4& GetProjection()
        {
            return engine.shadowCamera->GetProjection();
        }
        
        const CameraLib* GetCamera()
        {
            return engine.shadowCamera;
        }
        
        // 視点取得
        const VECTOR3& GetPos()
        {
            return engine.shadowCamera->GetPos();
        }
        
        // 注視点取得
        const VECTOR3& GetTarget()
        {
            return engine.shadowCamera->GetTarget();
        }
        
        // 上方向取得
        const VECTOR3& GetUp()
        {
            return engine.shadowCamera->GetUp();
        }
        
        // 前方向取得
        const VECTOR3& GetFront()
        {
            return engine.shadowCamera->GetFront();
        }
        
        // 右方向取得
        const VECTOR3& GetRight()
        {
            return engine.shadowCamera->GetRight();
        }
        
    }

    namespace Debug2D
    {
        void Box(VECTOR2 pos, VECTOR2 size, float angle,VECTOR4 color)
        {
            SpriteDebugRender spriteDebugRender;
            spriteDebugRender.pos = pos;
            spriteDebugRender.pos2 = size;
            spriteDebugRender.color = color;
            spriteDebugRender.angle = angle;
            spriteDebugRender.type = SpriteDebugType::SPRITE_DEBUG_BOX;
            engine.spriteDebugRenderer->Register(engine.graphics, spriteDebugRender);
        }

        void Circle(VECTOR2 pos, float radius, VECTOR4 color)
        {
            SpriteDebugRender spriteDebugRender;
            spriteDebugRender.pos = pos;
            spriteDebugRender.pos2.x = radius;
            spriteDebugRender.color = color;
            spriteDebugRender.type = SpriteDebugType::SPRITE_DEBUG_CIRCLE;
            engine.spriteDebugRenderer->Register(engine.graphics, spriteDebugRender);
        }

        void Line(VECTOR2 pos, VECTOR2 pos2, VECTOR4 color)
        {
            SpriteDebugRender spriteDebugRender;
            spriteDebugRender.pos = pos;
            spriteDebugRender.pos2 = pos2;
            spriteDebugRender.color = color;
            spriteDebugRender.type = SpriteDebugType::SPRITE_DEBUG_LINE;
            engine.spriteDebugRenderer->Register(engine.graphics, spriteDebugRender);
        }

       
    }

    namespace Debug3D
    {
        void Box(VECTOR3 pos, VECTOR3 size, VECTOR4 color)
        {
            if (engine.ImguiDebugFlg == false)return;
            GeometryDebugRender geometryDebugRender;
            geometryDebugRender.pos = pos;
            geometryDebugRender.pos2 = size;
            geometryDebugRender.color = color;
            geometryDebugRender.type = GeometryDebugType::Geometry_DEBUG_BOX;
            engine.geometryDebugRenderer->Register(engine.graphics, geometryDebugRender);
        }
        void Circle(VECTOR3 pos, float radius, VECTOR4 color)
        {
            if (engine.ImguiDebugFlg == false)return;
            GeometryDebugRender geometryDebugRender;
            geometryDebugRender.pos = pos;
            geometryDebugRender.pos2 = { radius ,radius ,radius };
            geometryDebugRender.color = color;
            geometryDebugRender.type = GeometryDebugType::Geometry_DEBUG_CIRCLE;
            engine.geometryDebugRenderer->Register(engine.graphics, geometryDebugRender);
        }
        void Line(VECTOR3 pos, VECTOR3 pos2, VECTOR4 color)
        {
            if (engine.ImguiDebugFlg == false)return;
            GeometryDebugRender geometryDebugRender;
            geometryDebugRender.pos = pos;
            geometryDebugRender.pos2 = pos2;
            geometryDebugRender.color = color;
            geometryDebugRender.type = GeometryDebugType::Geometry_DEBUG_LINE;
            engine.geometryDebugRenderer->Register(engine.graphics, geometryDebugRender);
        }

        void Cylinder(VECTOR3 pos, float height, float radius, VECTOR4 color )
        {
            if (engine.ImguiDebugFlg == false)return;
            GeometryDebugRender geometryDebugRender;
            geometryDebugRender.pos = pos;
            geometryDebugRender.pos2.x = radius;
            geometryDebugRender.pos2.y = height;
            geometryDebugRender.pos2.z = radius;
            geometryDebugRender.color = color;
            geometryDebugRender.type = GeometryDebugType::Geometry_DEBUG_CYLINDER;
            engine.geometryDebugRenderer->Register(engine.graphics, geometryDebugRender);
        }
        
        void Nav(dtNavMesh* navMesh, dtNavMeshQuery* navQuery, DirectX::XMFLOAT4X4 world,VECTOR3 targetPos, float targetRadius)
        {
            if (engine.ImguiDebugNavFlg == false)return;
            engine.navModelRenderer->Register(engine.graphics, navMesh, navQuery, world, targetPos, targetRadius);

        }
    }

    namespace Imgui
    {
        void Init()
        {
   
            IMGUI_CHECKVERSION();
            ImGui::CreateContext();
            ImGuiIO& io = ImGui::GetIO(); (void)io;
            io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;     // Enable Keyboard Controls
            io.ConfigFlags |= ImGuiConfigFlags_NavEnableGamepad;      // Enable Gamepad Controls

        

            io.Fonts->AddFontFromFileTTF("./External/ImGui/ipag.ttf", 16.0f, nullptr, io.Fonts->GetGlyphRangesJapanese());

            // Setup Dear ImGui style
            ImGui::StyleColorsDark();
            //ImGui::StyleColorsLight();

            Descriptor* ImguiFontDescripter = engine.graphics->GetDescriptorHeapSRV()->GetUnusedDescriptor();
            // Setup Platform/Renderer backends
            ImGui_ImplWin32_Init(engine.hwnd);
            ImGui_ImplDX12_Init(engine.graphics->GetDevice(), engine.graphics->GetBufferCount(),
                DXGI_FORMAT_R8G8B8A8_UNORM, engine.graphics->GetDescriptorHeapSRV()->GetDescriptorHeap().Get(),
                ImguiFontDescripter->GetCpuHandle(),
                ImguiFontDescripter->GetGpuHandle()
              // engine.graphics->GetDescriptorHeapSRV()->GetDescriptorHeap().Get()->GetCPUDescriptorHandleForHeapStart(),
              // engine.graphics->GetDescriptorHeapSRV()->GetDescriptorHeap().Get()->GetGPUDescriptorHandleForHeapStart()
              //  engine.graphics->GetDescriptorHeapSRV()->GetDescriptorHeap()->GetCPUDescriptorHandleForHeapStart(),
              //  engine.graphics->GetDescriptorHeapSRV()->GetDescriptorHeap()->GetGPUDescriptorHandleForHeapStart()
                ); 
        }


        void Update()
        {
          
            ImGui_ImplDX12_NewFrame();
            ImGui_ImplWin32_NewFrame();
            ImGui::NewFrame();
         
        }

        void Render()
        {
        

            ImGui::Render();
            engine.graphics->GetCommandList()->SetDescriptorHeaps(1,
                engine.graphics->GetDescriptorHeapSRV()->GetDescriptorHeap().GetAddressOf());
            ImGui_ImplDX12_RenderDrawData(ImGui::GetDrawData(), engine.graphics->GetCommandList());
       
        }

        void End()
        {
           
            ImGui_ImplDX12_Shutdown();
            ImGui_ImplWin32_Shutdown();
            ImGui::DestroyContext();
        }

        void SetImguiFlg(const bool& flg)
        {
            engine.ImguiDebugFlg = flg;
        }

        bool GetImguiFlg()
        {
            return engine.ImguiDebugFlg;
        }

        void SetImguiNavFlg(const bool& flg)
        {
            engine.ImguiDebugNavFlg = flg;
        }

        bool GetImguiNavFlg()
        {
            return engine.ImguiDebugNavFlg;
        }


    }

    namespace KeyInput
    {
        int GetKeyState(const INPUT_KEY& key)
        {
            return engine.inputController->inputControllerKeyBoard->GetKeyState(key);
        }

        POINT GetMousePos()
        {
            return engine.inputController->inputControllerMouse->GetMousePos();
        }

        void SetMousePos(const int& x, const int& y)
        {
            return engine.inputController->inputControllerMouse->SetMousePos(x,y);
        }

        // ボタン入力状態の取得
        GamePadButton GetButton() { return  engine.inputController->inputControllerJoyPad->GetButton(); }

        // ボタン押下状態の取得
        int GetButtonDown(BTN button) { return  engine.inputController->inputControllerJoyPad->GetButtonDown(button); }

        // ボタン押上状態の取得
        GamePadButton GetButtonUp() { return  engine.inputController->inputControllerJoyPad->GetButtonUp(); }

        // 左スティックX軸入力状態の取得
        float GetAxisLX() { return engine.inputController->inputControllerJoyPad->GetAxisLX(); }

        // 左スティックY軸入力状態の取得
        float GetAxisLY() { return engine.inputController->inputControllerJoyPad->GetAxisLY(); }

        // 右スティックX軸入力状態の取得
        float GetAxisRX() { return engine.inputController->inputControllerJoyPad->GetAxisRX(); }

        // 右スティックY軸入力状態の取得
        float GetAxisRY() { return engine.inputController->inputControllerJoyPad->GetAxisRY(); }

        // 左トリガー入力状態の取得
        float GetTriggerL() { return engine.inputController->inputControllerJoyPad->GetTriggerL(); }

        // 右トリガー入力状態の取得
        float GetTriggerR() { return engine.inputController->inputControllerJoyPad->GetTriggerR(); }
    }

    namespace Collision
    {
        bool ModelRayPick(const DirectX::XMFLOAT3& start, const DirectX::XMFLOAT3& end, RayOut& ray)
        {
            return engine.modelCollisionManager->RayPick(start, end, ray);
        }

        void ModelRegister(Model* model)
        {
            engine.modelCollisionManager->Register(model);
        }
        void ModelRender()
        {
            engine.modelCollisionManager->Render();
        }

        bool RectVsLine(const VECTOR2& rectPos1, const VECTOR2& rectPos2,const VECTOR2& lineStart, const VECTOR2& lineEnd)
        {
            Lib_CollisionRect rect;
            rect.pos1 = rectPos1;
            rect.pos2 = rectPos2;

            Lib_CollisionLine  line;
            line.start = lineStart;
            line.end = lineEnd;

           return  Lib_Collision::RectVsLine(rect,line);
        }
        bool CircleVsCircle(const VECTOR3& pos1,const float& radius1, const VECTOR3& pos2, const float& radius2)
        {
            Lib_CollisionCircle circle1;
            circle1.pos = pos1;
            circle1.radius = radius1;

            Lib_CollisionCircle circle2;
            circle2.pos = pos2;
            circle2.radius = radius2;

            return  Lib_Collision::CircleVsCircle(circle1, circle2);
        }
    }

}



