#pragma once

struct CameraBuffer
{
	XMMATRIX view;
	XMMATRIX projection;
};

class CameraManager
{
public:
	bool Init();

	void UpdateViewMatrix();
	void UpdateProjectionMatrix(float fovDegrees, float aspectRatio, float nearPlane, float farPlane);

	const XMFLOAT4X4& GetViewMatrix() const { return m_viewMatrix; }
	const XMFLOAT4X4& GetProjectionMatrix() const { return m_projectionMatrix; }

	CameraBuffer GetCameraBuffer() const;

	//Temporaire a remplacer avec un TRANSFORM
	void SetPosition(const XMFLOAT3& pos);
	XMFLOAT3 GetPosition() { return m_position; };
	void SetDirection(const XMFLOAT3& forward);
	// déplace la caméra le long de son vecteur avant
	void MoveForward(float distance);
	// déplace la caméra le long de son vecteur droit
	void MoveRight(float distance);
	// déplace la caméra le long de son vecteur coté
	void MoveUp(float distance);


private:
	//xmfloat provided with dxmath
	
	// Position et orientation
	XMFLOAT3 m_position;
	XMFLOAT3 m_forward, m_up, m_right;

	// Matrices
	XMFLOAT4X4 m_viewMatrix{};
	XMFLOAT4X4 m_projectionMatrix{};

	bool m_viewNeedsUpdate;
};

