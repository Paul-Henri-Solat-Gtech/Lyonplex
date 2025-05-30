#pragma once
class CameraManager
{
public:
	bool Init();

	void UpdateViewMatrix();

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

