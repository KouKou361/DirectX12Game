#pragma once
#include <Windows.h>
#include <DirectXMath.h>
#include "Lib_Vector.h"
#include "Input/InputController.h"

using namespace DirectX;

class CameraLib;
class Texture;
class Model;
class Lib_AudioBase;
class Lib_Effect3D;
class RayOut;
class dtNavMesh;
class dtNavMeshQuery;

enum SpriteShaderType
{
    NORMAL,
    PBR,
    LAMBERT,
    END,
};

namespace TK_Lib
{
    //ライブラリ初期化
    void LibInit(LPCTSTR caption, int screenWidth, int screenHeight);
    bool LibLoop(bool isShowFrameRate = true);
    void End();


    //Windowの設定
    namespace Window
    {
        HWND Init(const LPCTSTR& instance, const int& screenWidth, const int& screenHeight);
        float GetElapsedTime();
        HWND GetHwnd();
    }
    namespace Render
    {
        void Font(VECTOR2 pos, const  wchar_t* string);
        void SpriteRender(Texture* sprite,
            const VECTOR2& pos, const VECTOR2& scale, const VECTOR4& cut,
            const VECTOR4& color, const float& angle);
        void PlaneRender(Texture* texture, XMFLOAT4X4 mat, XMFLOAT4 cut, XMFLOAT4 color);
        void ModelRender(Model* model, const SpriteShaderType& type);
        void NavModelRender(Model* model);
        

        void BeginRender();
        void EndRender();
    }
    namespace Load
    {
        Texture* sprite(const char* filePath);
        Model* model(const char* filePath);
        Lib_AudioBase* Audio(const char* filePath, const char* key);
        Lib_Effect3D* Effect(Texture* texture, const int createNum);
    }
    namespace Audio
    {
        void Play(const char* key, bool loop=false);
        void Stop(const char* key);
        void SetVolume(const char* key, const LONG& volume);
        void SetPan(const char* key, const LONG& pan);
    }
    namespace Debug2D
    {
        void Box(VECTOR2 pos, VECTOR2 size, float angle = 0 , VECTOR4 color = { 1,0,0,1 });
        void Circle(VECTOR2 pos, float radius, VECTOR4 color = {1,0,0,1});
        void Line(VECTOR2 pos,VECTOR2 pos2, VECTOR4 color = { 1,0,0,1 });
    }
    namespace Debug3D
    {
        void Box(VECTOR3 pos, VECTOR3 size, VECTOR4 color = { 1,0,0,1 });
        void Circle(VECTOR3 pos, float radius, VECTOR4 color = { 1,0,0,1 });
        void Line(VECTOR3 pos, VECTOR3 pos2, VECTOR4 color = { 1,0,0,1 });
        void Cylinder(VECTOR3 pos, float height, float radius, VECTOR4 color = { 1,0,0,1 });
        void Nav(dtNavMesh* navMesh, dtNavMeshQuery* navQuery, DirectX::XMFLOAT4X4 world,VECTOR3 targetPos ,float targetRadius);
    }
    //==========================
    //カメラ関係
    //==========================
    namespace Camera
    {
        void SetLookAt(
            const VECTOR3& eye,
            const VECTOR3& focus,
            const VECTOR3& up);
   
        void SetPerspectiveFov(float fovY, float aspect, float nearZ, float farZ);
   
        const DirectX::XMFLOAT4X4& GetView();
   
        const DirectX::XMFLOAT4X4& GetProjection();
   
        const CameraLib* GetCamera();
   
        const VECTOR3& GetPos();
   
        const VECTOR3& GetTarget();
   
        const VECTOR3& GetUp();
   
        const VECTOR3& GetFront();
   
        const VECTOR3& GetRight();
    }

    namespace Light
    {
        void SetLookAt(
            const VECTOR3& eye,
            const VECTOR3& focus,
            const VECTOR3& up);

        void SetLightOrthongraphic(float fovY, float aspect, float nearZ, float farZ);

        const DirectX::XMFLOAT4X4& GetView();

        const DirectX::XMFLOAT4X4& GetProjection();

        const CameraLib* GetCamera();

        const VECTOR3& GetPos();

        const VECTOR3& GetTarget();

        const VECTOR3& GetUp();

        const VECTOR3& GetFront();

        const VECTOR3& GetRight();

    }


    //=========================
    //Imgui
    //=========================
    namespace Imgui
    {
        void Init();

        void Update();

        void Render();

        void End();

        void SetImguiFlg(const bool &flg);
        
        bool GetImguiFlg();

        void SetImguiNavFlg(const bool& flg);

        bool GetImguiNavFlg();
    }

    //=========================
    //KeyInput
    //=========================
    namespace KeyInput
    {
        int GetKeyState(const INPUT_KEY& key);

        POINT GetMousePos();

        void SetMousePos(const int& x, const int& y);

        // ボタン入力状態の取得
        GamePadButton GetButton();

        // ボタン押下状態の取得
        int GetButtonDown(BTN Button);

        // ボタン押上状態の取得
        GamePadButton GetButtonUp();

        // 左スティックX軸入力状態の取得
        float GetAxisLX();

        // 左スティックY軸入力状態の取得
        float GetAxisLY();

        // 右スティックX軸入力状態の取得
        float GetAxisRX();

        // 右スティックY軸入力状態の取得
        float GetAxisRY();

        // 左トリガー入力状態の取得
        float GetTriggerL();

        // 右トリガー入力状態の取得
        float GetTriggerR();
    }

    //=========================
   //Collision
   //=========================
    namespace Collision
    {
        bool ModelRayPick(const DirectX::XMFLOAT3& start, const DirectX::XMFLOAT3& end, RayOut& ray);

        void ModelRegister(Model* model);

        void ModelRender();

        bool RectVsLine(const VECTOR2& rectPos1, const VECTOR2& rectPos2, const VECTOR2& lineStart, const VECTOR2& lineEnd);

        bool CircleVsCircle(const VECTOR3& pos1, const float& radius1, const VECTOR3& pos2, const float& radius2);

    }

}
