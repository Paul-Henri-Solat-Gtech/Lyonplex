#pragma once

#include "CommandManager.h"
#include "Render3D.h"
#include "GraphicsDevice.h"
#include "InputManager.h"

struct CameraBuffer
{
	XMMATRIX view;
	XMMATRIX projection;
};

class CameraManager
{
//public:
//	bool Init(HWND windowWP, GraphicsDevice* graphicsDevice, CommandManager* commandManager, Render3D* render3d);
//
//	// CameraBuffer
//	void CreateCameraBuffer();
//	void UploadConstantBuffer();
//
//	// ViewMatrix
//	void UpdateViewMatrix();
//	void UpdateProjectionMatrix(float fovDegrees, float aspectRatio, float nearPlane, float farPlane);
//
//	const XMFLOAT4X4& GetViewMatrix() const { return m_viewMatrix; }
//	const XMFLOAT4X4& GetProjectionMatrix() const { return m_projectionMatrix; }
//
//	CameraBuffer GetCameraBuffer() const;
//
//	// Movement
//	void UpdateCamera();
//
//	// Temporaire a remplacer avec un TRANSFORM
//	void SetPosition(const XMFLOAT3& pos);
//	XMFLOAT3 GetPosition() { return m_position; };
//	void SetDirection(const XMFLOAT3& forward);
//	// deplace la camera le long de son vecteur avant
//	void MoveForward(float distance);
//	// deplace la camera le long de son vecteur droit
//	void MoveRight(float distance);
//	// deplace la camera le long de son vecteur cote
//	void MoveUp(float distance);
//
//	// Methodes de rotation
//	void Yaw(float angleRadians);
//	void Pitch(float angleRadians);
//
//	void SetSpeed(float speed) { m_cameraSpeed = speed; };
//
//	XMFLOAT3 GetPosition() const { return m_position; }
//
//private:
//	HWND m_windowWP;
//	GraphicsDevice* mp_graphicsDevice;
//	CommandManager* mp_commandManager;
//	Render3D* mp_render3d;
//
//	ComPtr<ID3D12Resource> m_cameraConstantBuffer;
//
//	// xmfloat provided with dxmath
//	// Position et orientation
//	XMFLOAT3 m_position;
//	XMFLOAT3 m_forward, m_up, m_right;
//
//	// Matrices
//	XMFLOAT4X4 m_viewMatrix{};
//	XMFLOAT4X4 m_projectionMatrix{};
//
//	bool m_viewNeedsUpdate;
//	float m_cameraSpeed;
//	float m_mouseSensitivity;   // ajustable
//	// Pour gerer la rotation a la souris
//	bool m_rotating;
//	POINT m_prevMousePos;
};

