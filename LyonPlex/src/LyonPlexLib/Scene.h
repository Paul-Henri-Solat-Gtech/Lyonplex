#pragma once

class Scene
{
public:
	virtual void Start();
	virtual void Update();
	virtual void Release();

private:

	std::string m_SceneName;

};

